from idl import *

@enum
class node_type:
    FILE = auto()
    DIR = auto()

@enum
class vfs_operation:
    VFS_OPEN = auto()
    VFS_READ = auto()

@struct 
class vfs_node:
    type: node_type
    name: str
    parent: c_notation("struct VFS_NODE *parent", "json_ptr")
    points_to: c_notation("struct VFS_NODE *points_to", "json_ptr")
    children: c_notation("vec(struct VFS_NODE *) children", "json_ptr")

@endpoint
class Vfs:
    def register_filesystem(fs: str) -> int: pass
    def mount(path: str, fs: str) -> int: pass
    def rpc_mkdir(path: str) -> int: pass