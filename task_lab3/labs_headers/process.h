/**
 * @file     process.h
 * @Author   Michael Kosyakov, Evgeniy Ivanov and Denis Tarakanov (ifmo.distributedclass@gmail.com)
 * @brief    Definitions of data structures and functions related to presentation of processes
 *
 * Students must not modify this file!
 */

#ifndef ITMO_HDU_DISTRIBUTED_SYSTEMS_PROCESS_H
#define ITMO_HDU_DISTRIBUTED_SYSTEMS_PROCESS_H

#include <stdbool.h>
#include "message.h"

struct child_arguments {
    local_id self_id;   // Internal id of current process
    int count_nodes;    // Total count of processes (child and parent)
    uint8_t balance;    // For banking system labs
    bool mutex_usage;   // For mutual exclusion labs
};

//------------------------------------------------------------------------------
// Functions below must be implemented by students
//------------------------------------------------------------------------------
/** Represents the main execution function of parent process
 *
 * @param count_nodes     Total count of all processes (child and parent)
 */
void parent_work(int count_nodes);

//------------------------------------------------------------------------------

/** Represents the main execution function of any child process
 *
 * @param args  Arguments of this child process (see description of structure above)
 */
void child_work(struct child_arguments args);

#endif // ITMO_HDU_DISTRIBUTED_SYSTEMS_PROCESS_H
