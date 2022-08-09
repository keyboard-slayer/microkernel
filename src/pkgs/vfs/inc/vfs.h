#ifndef VFS_INC_VFS_H
#define VFS_INC_VFS_H

#include <vec.h>
#include <unistd.h>

#include <idl/inc/vfs.h>

typedef struct 
{
    char *name;
    pid_t pid;
} fs_t;

typedef struct 
{
    fs_t fs;
    vfs_node_t *node;
} mount_t;

typedef vec(fs_t) fs_vec_t;
typedef vec(mount_t) mount_vec_t;

#endif /* !VFS_INC_VFS_H */
