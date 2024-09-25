#include "../../src/include/uint.h"
#include "../../src/include/pci.h"
#include "../../src/include/xhci.h"
#include "../../src/include/graphics.h"
#include "../../src/include/cpaging.h"
#include "../../src/include/io.h"
#include "../../src/include/msi.h"

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
         printf("ERROR: Timeout during reset of port: ");
         printdec(port_number);
         while(1);
      }
   }
}

/*Host Controller Reset*/
void __attribute__((optimize("O0"))) HCRST(uint64_t operational_register) {
   if (!(read_USBSTS(operational_register) & 0b1)) {
      printf("ERROR: xHC is not halted before reset");
      while(1);
   }
   uint32_t usbcmd = read_USBCMD(operational_register);
   usbcmd |= (1 << 1);
   write_USBCMD(operational_register, usbcmd);

   int timeout = 0x100000;
   while (read_USBCMD(operational_register) & (1 << 1)) {
      timeout--;
      if (timeout == 0) {
         printf("ERROR: Timeout during xHC reset");
         while(1);
      }
   }
}

void __attribute__((optimize("O0"))) init_device_context_array(uint64_t operational_registers, int n_slots) {
   uint64_t base_addr = kmalloc(1, 1, 0, 0, 0); // Allocates a 2 MiB page
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

void __attribute__((optimize("O0"))) init_command_ring(uint64_t operational_registers) {
   uint64_t* base_addr = (uint64_t*)kmalloc(1,1,0,0,0);
   for (int i = 0; i < 0x200000/8; i++) {
      base_addr[i] = 0x0;
   }

   write_CRCR(operational_registers, (uint64_t)base_addr);
}

void __attribute__((optimize("O0"))) init_event_ring(uint64_t capability_registers, uint64_t runtime_registers) {
   uint32_t hcsparams1 = read_HCSPARAMS1(capability_registers);
   int max_interrupters = (int)((hcsparams1 >> 8) & 0x3FF);
   for (int i = 0; i < max_interrupters; i++) {
      uint64_t* base_addr = (uint64_t*)kmalloc(1, 1, 0, 0, 0);
      // Initialize event ring segments (10 segments in total)
      int n_segments = 10; // Adjust as needed for your design
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
      write_ERSTSZ(runtime_registers, i, (erstsz & 0xffff00) | n_segments);

      // Set ERSTBA
      uint64_t erstba = read_ERSTBA(runtime_registers, i);
      write_ERSTBA(runtime_registers, i, (erstba & 0b11111) | (uint64_t)base_addr);
      
      // Set ERDP
      uint64_t erdp = read_ERDP(runtime_registers, i);
      write_ERDP(runtime_registers, i, (erdp & 0b1111) | ((uint64_t)base_addr + 0x1000));

      write_IMAN(runtime_registers, i, read_IMAN(runtime_registers, i) | (1 << 0));
   }
   
}

void __attribute__((optimize("O0"))) read_event_ring(uint64_t runtime_registers, int interrupter) {
   uint64_t erdp = read_ERDP(runtime_registers, interrupter);
   TRB* trb = (TRB*)(erdp & ~0b1111);
   printhex(trb->data_buffer_ptr_low);
   printf("\n");
   printhex(trb->data_buffer_ptr_high);
   printf("\n");
   printhex(trb->status);
   printf("\n");
   printhex(trb->control);
   printf("\n\n");
   write_ERDP(runtime_registers, interrupter, erdp + sizeof(TRB));
}


void __attribute__((optimize("O0"))) port_info(uint64_t operational_registers, int n_ports) {
   for (int i = 0; i < n_ports; i++) {
      uint32_t* portsc = (uint32_t*)(operational_registers + 0x400 + 0x10*i);
      port_reset(operational_registers, i);
      if (portsc[0] & (1 << 17)) {
         //acknowledge the CSC
         portsc[0] |= (1 << 17);
      }
      printbin(portsc[0]);
      printf("\n");
   }
   printf("\n");
}


void __attribute__((optimize("O0"))) init_xhci_controller(int device_number) {
   config_PCI(device_number);

   PCI_DEV xhci_device = device_list.devices[device_number];
   uint64_t capability_registers = xhci_device.bars[0].base_address;
   uint64_t operational_registers = capability_registers + read_CPLENGTH(capability_registers);
   uint64_t runtime_registers = capability_registers + read_RTSOFF(capability_registers);

   
   //wait until xhci controller is ready
   while(((read_USBSTS(operational_registers) >> 11) & 0b1));
   
   HCRST(operational_registers);

   //Program the Max Device Slots Enabled (MaxSlotsEn) field
   uint32_t config = read_CONFIG(operational_registers) & 0xffffff00;
   uint32_t n_ports = (read_HCSPARAMS1(capability_registers) >> 24) & 0xff;
   uint32_t n_slots = (read_HCSPARAMS1(capability_registers) & 0xff);
   if (n_slots > n_ports) {
      n_slots = n_ports;
   }
   config |= n_slots;
   write_CONFIG(operational_registers, config);

   // 1. Program the Device Context Base Address Array Pointer (DCBAAP)
   init_device_context_array(operational_registers, n_slots);

   // 2. Program the Command Ring Dequeue Pointer
   init_command_ring(operational_registers);
   
   // 3. Program the Event Ring
   init_event_ring(capability_registers, runtime_registers);

   // enable Interrupts
   uint32_t usbcmd = read_USBCMD(operational_registers);
   write_USBCMD(operational_registers, usbcmd | (1 << 2));

   // 4. Activate the host controller
   usbcmd = read_USBCMD(operational_registers);
   usbcmd |= (1 << 0); // Run/Stop bit
   write_USBCMD(operational_registers, usbcmd);

   port_info(operational_registers, n_ports);
   
   enable_MSIX(device_number);
}