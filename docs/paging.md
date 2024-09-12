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