from idl import *

@enum
class node_type:
    FILE = auto()
    DIR = auto()

@struct 
class vfs_node:
    type: node_type
    name: str
    parent: c_notation("struct VFS_NODE *parent", "json_ptr")
    points_to: c_notation("struct VFS_NODE *points_to", "json_ptr")
    children: c_notation("vec(struct VFS_NODE *) children", "json_ptr")


@struct 
class filesystem:
    name: str
    read: c_notation("int (*read)(int fd, void *buf, size_t count)", "json_ptr")
    open: c_notation("int (*open)(char *path, int flags)", "json_ptr")


@endpoint
class Vfs:
    def register_filesystem(fs: filesystem) -> int: pass
    def mount(path: str, fs: filesystem) -> int: pass
    def rpc_mkdir(path: str) -> int: pass