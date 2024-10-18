#include "../../src/include/uint.h"
#include "../../src/include/pci.h"
#include "../../src/include/xhci.h"
#include "../../src/include/graphics.h"
#include "../../src/include/mm/paging.h"
#include "../../src/include/mm/utils.h"
#include "../../src/include/io.h"
#include "../../src/include/msi.h"
#include "../../src/include/interrupts.h"
#include "../../src/include/apic.h"

typedef struct TRB {
   uint32_t data_buffer_ptr_low;
   uint32_t data_buffer_ptr_high;
   uint32_t status;
   uint32_t control;
} TRB;

void config_PCI(int device_number) {
   device_list.devices[device_number].PCI_Config_Space->Command |= (1 << 1) | (1 << 2);
}

void __attribute__((optimize("O0"))) port_reset(uint64_t operational_registers, int port_number) {
   uint32_t * portsc = (uint32_t*)(operational_registers + 0x400 + 0x10*port_number);
   portsc[0] |= (1 << 4);
   int timeout = 0x100000;
   while (portsc[0] & (1 << 4)) {
      timeout--;
      if (timeout == 0) {
         print("ERROR: Timeout during reset of port: ");
         printdec(port_number);
         while(1);
      }
   }
}

void __attribute__((optimize("O0"))) reset_ports(uint64_t operational_registers, int n_ports) {
   for (int port = 0; port < n_ports; port++) {
      uint32_t* portsc = (uint32_t*)(operational_registers + 0x400 + 0x10*port);
      port_reset(operational_registers, port);
      if (portsc[0] & (1 << 17)) {
         //acknowledge the CSC
         portsc[0] |= (1 << 17);
      }
   }
}

int error(uint64_t operational_registers) {
   return (read_USBSTS(operational_registers) >> 12) & 0b1;
}

/*Host Controller Reset*/
void __attribute__((optimize("O0"))) HCRST(uint64_t operational_register) {
   if (!(read_USBSTS(operational_register) & 0b1)) {
      print("ERROR: xHC is not halted before reset");
      while(1);
   }
   uint32_t usbcmd = read_USBCMD(operational_register);
   usbcmd |= (1 << 1);
   write_USBCMD(operational_register, usbcmd);

   int timeout = 0x100000;
   while (read_USBCMD(operational_register) & (1 << 1)) {
      timeout--;
      if (timeout == 0) {
         print("ERROR: Timeout during xHC reset");
         while(1);
      }
   }
}

void __attribute__((optimize("O0"))) port_info(xHC* xhc, int n_ports) {
   for (int i = 0; i < n_ports; i++) {
      uint32_t portsc = ((uint32_t*)(xhc->operational_registers + 0x400 + 0x10*i))[0];
      printbin(portsc);
      print("\n");
   }
   print("\n");
}

void __attribute__((optimize("O0"))) init_device_context_array(uint64_t operational_registers, int n_slots) {
   uint64_t base_addr = kmalloc_phys_page(1, 0, 1, 1); // Allocates a 2 MiB page
   uint64_t* dcbaap = (uint64_t*)base_addr;

   dcbaap[0] = 0; // or keep it as is if it should point to the DCBAA

   for (int i = 1; i <= n_slots; i++) {
      uint64_t context_address = (uint64_t)(base_addr + (i - 1) * 1024 + (n_slots + 1) * sizeof(uint64_t));
      dcbaap[i] = (context_address + 63) & ~((uint64_t)63); // Align to 64 bytes
   }
   for (int i = 1; i <= n_slots; i++) {
      for (int j = 0; j < 128; j++) {
         ((uint64_t*)dcbaap[i])[j] = 0x0;
      }
   }

   write_DCBAAP(operational_registers, (uint64_t)dcbaap);
}

//in qemu only 1 event ring segment is supported can be found in HCSPARAMS2 [4:7]
void __attribute__((optimize("O0"))) init_event_ring(uint64_t capability_registers, uint64_t operational_registers, uint64_t runtime_registers) {
   uint64_t hcsparams1 = read_HCSPARAMS1(capability_registers);
   int max_interrupters = (int)((hcsparams1 >> 8) & 0x3FF);
   for (int i = 0; i < max_interrupters; i++) {
      uint64_t* base_addr = (uint64_t*)kmalloc_phys_page(1, 0, 1, 1);
      // Initialize event ring segments (10 segments in total)
      int n_segments = 1;
      for (int j = 0; j < n_segments; j++) {
         base_addr[2*j] = (uint64_t)base_addr + 0x1000 * (1 + j); // Segment base
         base_addr[2*j + 1] = 0x1000; // Segment size
      }
      
      // Initialize the segments to 0
      for (int j = 0; j < n_segments; j++) {
         memset(base_addr[2*j], 0x0, (uint32_t)base_addr[2*j + 1]);
      }

      // Set ERSTSZ
      uint32_t erstsz = read_ERSTSZ(runtime_registers, i);
      write_ERSTSZ(runtime_registers, i, (erstsz & 0xffff0000) | n_segments);

      
      // Set ERSTBA
      uint64_t erstba = read_ERSTBA(runtime_registers, i);
      write_ERSTBA(runtime_registers, i, (erstba & 0b111111) | (uint64_t)base_addr);

      // Set ERDP
      uint64_t erdp = read_ERDP(runtime_registers, i);
      write_ERDP(runtime_registers, i, (erdp & 0b1111) | ((uint64_t)base_addr + 0x1000));  


      uint32_t iman = read_IMAN(runtime_registers, i);
      iman |= (1 << 1);
      write_IMAN(runtime_registers, i, iman);
   }
   if (error(operational_registers)) {
      print("ERROR: Event Ring Initialization");
      while(1);
   }
   
}

void port_status_change_event(TRB* psc, uint64_t operational_registers) {
   int port = (psc->data_buffer_ptr_low >> 24) & 0xff;
   uint32_t* portsc = (uint32_t*)(operational_registers + 0x400 + 0x10*port);
   if ((portsc[0] >> 17) & 0b1) {
      //CSC (conntect status change)
      portsc[0] |= (1 << 17);
   }
}

/*cannot handle event segment switches*/
void __attribute__((optimize("O0"))) read_event_ring(uint64_t runtime_registers, uint64_t operational_registers, int interrupter) {
   uint64_t erdp = read_ERDP(runtime_registers, interrupter);
   TRB* trb = (TRB*)(erdp & ~0b1111);
   while (trb->control & 0b1) {
      
      switch ((trb->control >> 10) & 0b111111) {
      case 32:
         print("Transfer Event\n");
         break;
      case 33:
         print("Command Completion Event\n");
         break;
      case 34:
         print("Port Status Change Event\n");
         port_status_change_event(trb, operational_registers);
         break;
      case 35:
         print("Bandwidth Request Event\n");
         break;
      case 36:
         print("Doorbell Event\n");
         break;
      case 37:
         print("Host Controller Event\n");
         break;
      case 38:
         print("Device Notification Event\n");
         break;
      case 39:
         print("MFINDEX Wrap Event\n");
         break;
      default:
         print("ERROR: TRB type not allowed");
         while(1);
         break;
      }
      trb++;
   }
   
   write_ERDP(runtime_registers, interrupter, (uint64_t)trb - sizeof(TRB));
}

void dump_event_ring(uint64_t runtime_registers, uint64_t operational_registers, int interrupter) {
   uint64_t erdp = read_ERDP(runtime_registers, interrupter);
   TRB* trb = (TRB*)(erdp & ~0b1111);
   for (int i = 0; i < 10; i++) {
      printhex(trb[i].data_buffer_ptr_low);
      print(" ");
      printhex(trb[i].data_buffer_ptr_high);
      print(" ");
      printhex(trb[i].status);
      print(" ");
      printhex(trb[i].control);
      print("\n");
   }
   print("\n");
}


void __attribute__((optimize("O0"))) interrupt_handler(uint64_t irq) {
   int device_number = irq-32;
   PCI_DEV xhci_device = device_list.devices[device_number];
   uint64_t capability_registers = xhci_device.bars[0].base_address;
   uint64_t operational_registers = capability_registers + read_CPLENGTH(capability_registers);
   uint64_t runtime_registers = capability_registers + read_RTSOFF(capability_registers);
   uint64_t doorbell_array = capability_registers + (read_DBOFF(capability_registers) & ~(uint64_t)(0b11));

   
   uint64_t hcsparams1 = read_HCSPARAMS1(capability_registers);
   int max_interrupters = (int)((hcsparams1 >> 8) & 0x3FF);
   for (int i = 0; i < max_interrupters; i++) {
      uint32_t iman = read_IMAN(runtime_registers, i);
      if (iman & (1 << 0)) {
         read_event_ring(runtime_registers, operational_registers, i);
         write_IMAN(runtime_registers, i, iman);
      }
   }

   write_USBSTS(operational_registers, read_USBSTS(operational_registers));
   send_EOI();
}

void __attribute__((optimize("O0"))) init_command_ring(xHC* xhc) {
   uint64_t base_addr = kmalloc_phys_page(1,0,1,1);
   xhc->cr_enqueue_ptr = base_addr;
   memset(base_addr, 0x0, 0x200000);
   TRB* link_trb = (TRB*)(base_addr + 0x200000 - sizeof(TRB));
   link_trb->data_buffer_ptr_low = (uint32_t)base_addr;
   link_trb->data_buffer_ptr_high = (uint32_t)(base_addr >> 32);
   link_trb->status = 0x0;
   link_trb->control = (0xe << 10) | 0x1;
   write_CRCR(xhc->operational_registers, base_addr);
   
}

void __attribute__((optimize("O0"))) place_no_op(int device_number) {
   xHC* xhc = (xHC*)device_list.devices[device_number].driver_config_space;
   TRB* trb = (TRB*)(xhc->cr_enqueue_ptr);
   trb->data_buffer_ptr_low, trb->data_buffer_ptr_high = 0,0;
   trb->status = 0;
   trb->control = (23 << 10) | 0x1;
   //ring doorbell
   uint32_t* dbr = (uint32_t*)(xhc->capability_registers + read_DBOFF(xhc->capability_registers));
   dbr[0] = 0x0;
   //increment the enqueue_ptr
   xhc->cr_enqueue_ptr += sizeof(TRB);
}

void __attribute__((optimize("O0"))) init_transfer_ring(uint64_t operational_registers) {

}


void init_driver_config_space(int device_number) {
   volatile PCI_DEV* xhci_device = &(device_list.devices[device_number]);
   uint64_t base_address = kmalloc(1, 1, 0, 0, 0);
   
   xhci_device->driver_config_space = base_address;
   xHC* driver_config_space = (xHC*)(xhci_device->driver_config_space);
   driver_config_space->capability_registers = xhci_device->bars[0].base_address;
   driver_config_space->operational_registers = driver_config_space->capability_registers + read_CPLENGTH(driver_config_space->capability_registers);
   driver_config_space->runtime_registers = driver_config_space->capability_registers + read_RTSOFF(driver_config_space->capability_registers);
   driver_config_space->doorbell_array = driver_config_space->capability_registers + (read_DBOFF(driver_config_space->capability_registers) & ~(uint64_t)(0b11));
}

void __attribute__((optimize("O0"))) init_xhci_controller(int device_number) {
   config_PCI(device_number);
   init_driver_config_space(device_number);
   xHC* xhc = (xHC*)(device_list.devices[device_number].driver_config_space);
   
   //wait until xhci controller is ready
   int timeout = 0x100000;
   while(((read_USBSTS(xhc->operational_registers) >> 11) & 0b1)) {
      if (timeout == 0) {
         print("ERROR: Timout while waiting for xHC");
         while(1);
      }
      timeout--;
   }
   
   //reset the xHC
   HCRST(xhc->operational_registers);

   //Program the Max Device Slots Enabled (MaxSlotsEn) field
   uint32_t config = read_CONFIG(xhc->operational_registers) & 0xffffff00;
   uint32_t n_ports = (read_HCSPARAMS1(xhc->capability_registers) >> 24) & 0xff;
   uint32_t n_slots = (read_HCSPARAMS1(xhc->capability_registers) & 0xff);
   if (n_slots > n_ports) {
      n_slots = n_ports;
   }
   config |= n_slots;
   write_CONFIG(xhc->operational_registers, config);

   // 1. Program the Device Context Base Address Array Pointer (DCBAAP)
   init_device_context_array(xhc->operational_registers, n_slots);
   
   // 2. Program the Event Ring
   init_event_ring(xhc->capability_registers, xhc->operational_registers,xhc->runtime_registers);
   
   // enable Interrupts
   uint32_t usbcmd = read_USBCMD(xhc->operational_registers);
   write_USBCMD(xhc->operational_registers, usbcmd | (1 << 2));

   reset_ports(xhc->operational_registers, n_ports);

   // 4. Activate the host controller
   usbcmd = read_USBCMD(xhc->operational_registers);
   usbcmd |= (1 << 0); // Run/Stop bit
   write_USBCMD(xhc->operational_registers, usbcmd);


   init_command_ring(xhc);
   place_no_op(device_number);
}