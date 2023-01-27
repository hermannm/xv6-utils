#include "process_list.h"
#include "../../lib/user/user.h"
#include "../utils/realloc.h"
#include "sys_getprocs.h"

#define BATCH_SIZE 2

struct list_processes_result list_processes()
{
    struct list_processes_result res = {
        .processes =
            {
                .array = malloc(sizeof(struct process_info) * BATCH_SIZE),
                .length = 0,
                .capacity = BATCH_SIZE,
            },
        .error = LIST_PROCESSES_SUCCESS,
    };

    int offset = 0;
    while (1) {
        int return_code = getprocs(BATCH_SIZE, offset, res.processes.array);
        if (return_code == GETPROCS_MEMORY_COPY_ERROR) {
            res.error = LIST_PROCESSES_SYSCALL_ERROR;
            return res;
        }

        int procs_added = return_code;
        res.processes.length += procs_added;
        if (procs_added < BATCH_SIZE) {
            break;
        }

        offset += BATCH_SIZE;
        increase_process_list_capacity(&res.processes, BATCH_SIZE);
    }

    return res;
}

void destroy_process_list(struct process_list proc_list)
{
    free(proc_list.array);
}

void increase_process_list_capacity(struct process_list* proc_list, uint extra_capacity)
{
    uint old_capacity = proc_list->capacity;
    uint new_capacity = old_capacity + extra_capacity;

    proc_list->array = realloc(
        proc_list->array, sizeof(struct process_info) * old_capacity,
        sizeof(struct process_info) * new_capacity
    );

    proc_list->capacity = new_capacity;
}
