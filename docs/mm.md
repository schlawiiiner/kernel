# Kernel Memory Layout
|                                           |              |
| ----------------------------------------- | ------------ |
| 0x00000000 00000000 - 0x00007fff ffffffff | user space   |
| 0xffff8000 00000000 - 0xffffff7f ffffffff | kernel space |
| 0xffffff80 00000000 - 0xffffffff ffffffff | page tables  |
