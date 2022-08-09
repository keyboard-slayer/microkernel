#include <ipc.h>
#include <stdbool.h>
#include <unistd.h>
#include <vec.h>

#include "../inc/vfs.h"

static fs_vec_t fs;
static mount_vec_t mount;
static vfs_node_t *root = NULL;

char *basename(char const *path)
{
    char *p = strrchr(path, '/');
    return p ? p + 1 : (char *) path;
}

static vfs_node_t *vfs_find_node(vfs_node_t *node, char *name)
{
    size_t index;
    vfs_node_t *child;

    vec_foreach(&node->children, child, index)
    {
        if (strcmp(child->name, name) == 0)
        {
            return child;
        }
    }
}

static vfs_node_t *vfs_get_parent(vfs_node_t *start, char *path)
{
    vfs_node_t *node = start;
    size_t index;
    char *seg;
    vec_str_t path_vec = vec_split(path, "/");

    vec_foreach(&path_vec, seg, index)
    {
        if (strlen(seg) > 0)
        {
            if (path_vec.length == index + 2)
            {
                return node;
            }

            node = vfs_find_node(node, path_vec.data[index + 1]);
        }
        else  
        {
            if (path_vec.length == 2)
            {
                return start;
            }

            node = vfs_find_node(start, path_vec.data[index + 1]);
        }
    }

    return node;
}

static vfs_node_t *vfs_mkdir(char *path)
{
    vfs_node_t *node = calloc(1, sizeof(vfs_node_t));
    vfs_node_t *prev_dir = calloc(1, sizeof(vfs_node_t));
    vfs_node_t *curr_dir = calloc(1, sizeof(vfs_node_t));

    vec_init(&node->children);

    prev_dir->name = "..";
    curr_dir->name = ".";

    prev_dir->points_to = node->parent;
    curr_dir->points_to = node;

    vec_push(&node->children, prev_dir);
    vec_push(&node->children, curr_dir);

    if (strcmp(path, "/"))
    {
        node->parent = vfs_get_parent(root, path);
        node->name = basename(path);

        if (!node->parent)
        {
            return NULL;
        }

        vec_push(&node->parent->children, node);
    }
    else  
    {
        node->parent = node;
        node->name = "/";
    }

    return node;
}

static void vfs_main(void)
{
    ipc_t *ipc = (ipc_t *) ipc_receive_sync();
    int function = rpc_get_function(ipc);

    switch (function)
    {
        case VFS_REGISTER_FILESYSTEM:
        {
            vfs_req_register_filesystem_t req = rpc_request_register_filesystem_unpack(ipc);
            fs_t entry = {
                .name = strdup(req.fs),
                .pid = ipc->src
            };

            vec_push(&fs, entry);
            rpc_response_register_filesystem(ipc->src, 0);
            break;
        }

        case VFS_MOUNT:
        {
            size_t index;
            fs_t tmp;
            vfs_req_mount_t req = rpc_request_mount_unpack(ipc);

            vec_foreach(&fs, tmp, index)
            {
                if (strcmp(tmp.name, req.fs) == 0)
                {
                    vfs_node_t *node = vfs_mkdir(req.path);

                    mount_t entry = {
                        .node = node,
                        .fs = tmp
                    };

                    vec_push(&mount, entry);
                    rpc_response_mount(ipc->src, 0);
                    break;
                }
            }

            break;
        }

        case VFS_RPC_MKDIR:
        {
            vfs_req_rpc_mkdir_t req = rpc_request_rpc_mkdir_unpack(ipc);
            rpc_response_rpc_mkdir(ipc->src, vfs_mkdir(req.path) == NULL ? 0 : 1);
            break;
        }
    }

}

__attribute__((noreturn)) int _start(void)
{
    vec_init(&fs);
    vec_init(&mount);

    for (;;)
    {
        vfs_main();
    }
    __builtin_unreachable();
}

