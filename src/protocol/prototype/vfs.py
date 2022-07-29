from idl import *

"""
Example of VFS IPC:

    1. Client send an IPC.
    {
        "rpc_function": VFS_OPEN,
        "rpc_args": {
            "path": "test.txt",
            "flags": RD_ONLY
        }
    }

    2. Server receive the IPC.

    3. Server send IPC response.
    {
        "rpc_function": VFS_OPEN,
        "rpc_success": True,
        "rpc_reponse": 4
    }
"""

@enum
class VFS_FILE_ACCESS:
    O_RDNONLY = 0x0000
    O_WRONLY = 0x0001
    O_RDWR = 0x0002
    O_CREAT = 0x0100
    O_APPEND = 0x0400

@endpoint
class VfsServer:
    def open(path: str, flags: VFS_FILE_ACCESS) -> int: pass