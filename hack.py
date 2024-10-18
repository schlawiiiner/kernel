"""
This script is a terrible hack.

It is used to replace the the 'p_vaddr' and 'p_paddr' for a certain segment in the ELF header of the kernel binary.
This tricks grub2 to load the segment to a differnt physical addr. So we can prevent gurb from trying to load a high half kernel
to the address specified in the elf header. Just run the following command

    python hack.py bin/mykernel.bin load.txt

where 'load.txt' has the following syntax:

    <old_load_addr> -> <new_load_addr>

Example:
    0xffff800000000 -> 0x200000

"""


from elftools.elf.elffile import ELFFile
import sys

def read(file, offset, bytes):
    file.seek(offset)
    bytes = file.read(bytes)
    return int.from_bytes(bytes, byteorder='little')

def write(file, offset, bytes, value):
    file.seek(offset)
    file.write(value.to_bytes(bytes, byteorder='little'))


def replace_load_addrs(file, addrs):
    with open(file, 'r+b') as f:
        elf = ELFFile(f)
        e_phoff = elf['e_phoff']            # Segment header table offset
        e_phentsize = elf['e_phentsize']    # Size of each segment header
        
        for n, segment in enumerate(elf.iter_segments()):
            for i, addr in enumerate(addrs):

                if (segment['p_vaddr'] == addr[0]) or (segment['p_paddr'] == addr[0]):
                    if (segment['p_vaddr'] != segment['p_paddr']):
                        raise ValueError("Expects p_vaddr and p_paddr to be equal")
                    write(f, e_phoff + e_phentsize*n + 0x10, 8, addrs[i][1])
                    write(f, e_phoff + e_phentsize*n + 0x18, 8, addrs[i][1])

def parse_load_table(file):
    lines = []
    with open(file, 'r+') as f:
        lines = f.readlines()

    if len(lines) == 0:
        return
    
    for i, line in enumerate(lines):

        # split
        addrs = line.split("->")
        if len(addrs) != 2:
            raise SyntaxError(f"Syntax Error in line {i+1}")
        
        # ignore chars
        ignore = "\n "
        for char in ignore:
            addrs[0] = addrs[0].replace(char, '')
            addrs[1] = addrs[1].replace(char, '')

        
        addrs[0] = int(addrs[0], 0)
        addrs[1] = int(addrs[1], 0)
        lines[i] = addrs
    return lines
        
  
if __name__ == "__main__":
    argv = sys.argv
    if len(argv) != 3:
        raise ValueError("Expecting 3 arguments")
    reload = parse_load_table(argv[2])
    replace_load_addrs(argv[1], reload)


