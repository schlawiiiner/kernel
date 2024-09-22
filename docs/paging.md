# Page Directory Pointer Table Entry

| Bits      | Description               |
| --------- | ------------------------- |
| 0         | Present                   |
| 1         | Read/Write                |
| 2         | User/Supervisor           |
| 3         | Write-Through             |
| 4         | Cache Disable             |
| 5         | Accessed                  |
| 6         | Dirty                     |
| 7         | Page Size                 |
| 8         | Global                    |
| 9 ...11   | Available                 |
| 12        | Page Attribute Table      |
| 13...29   | Reserved (0)              |
| 30...M-1  | Bits 30-(M-1) of address  |
| M ...51   | Reserved (0)              |
| 52...58   | Available                 |
| 59...62   | Protection Key            |
| 63        | Execute Disable           |

# Page Directory Entry

| Bits      | Description               |
| --------- | ------------------------- |
| 0         | Present                   |
| 1         | Read/Write                |
| 2         | User/Supervisor           |
| 3         | Write-Through             |
| 4         | Cache Disable             |
| 5         | Accessed                  |
| 6         | Dirty                     |
| 7         | Page Size                 |
| 8         | Global                    |
| 9 ...11   | Available                 |
| 12        | Page Attribute Table      |
| 13...20   | Reserved (0)              |
| 21...M-1  | Bits 21-(M-1) of address  |
| M ...51   | Reserved (0)              |
| 52...58   | Available                 |
| 59...62   | Protection Key            |
| 63        | Execute Disable           |

# Page Table Entry

| Bits      | Description               |
| --------- | ------------------------- |
| 0         | Present                   |
| 1         | Read/Write                |
| 2         | User/Supervisor           |
| 3         | Write-Through             |
| 4         | Cache Disable             |
| 5         | Accessed                  |
| 6         | Dirty                     |
| 7         | Page Size                 |
| 8         | Global                    |
| 9 ...11   | Available                 |
| 12...M-1  | Bits 12-(M-1) of address  |
| M ...51   | Reserved (0)              |
| 52...58   | Available                 |
| 59...62   | Protection Key            |
| 63        | Execute Disable           |

- Present: If set the page is actually (present) in physical memory, else the page may be swapped out, or indicating a non valid entry.
-  Read/Write: If set the page can be read and written to, else the page is read-only.  The WP bit in CR0 determines if this is only applied to userland, always giving the kernel write access (the default) or both userland and the kernel
- User/Supervisor:  If the bit is set, then the page may be accessed by all; if the bit is not set, however, only the supervisor can access it. For a page directory entry, the user bit controls access to all the pages referenced by the page directory entry. Therefore if you wish to make a page a user page, you must set the user bit in the relevant page directory entry as well as the page table entry.
- Write-Through: If the bit is set, write-through caching is enabled. If not, then write-back is enabled instead.