#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

#include "message.h"
#include "log.h"
#include "process.h"

void parent_work(int count_nodes)
{
    // printf("Parent process started , %d children\n", count_nodes - 1);
    /* STUDENT IMPLEMENTATION STARTED */
    for (int i = 1; i < count_nodes; i++)
    {
        Message msg;
        receive(i, &msg);
        if (msg.s_header.s_magic == MESSAGE_MAGIC && msg.s_header.s_type == STARTED)
        {
            continue;
        }
    }
    for (int i = 1; i < count_nodes; i++)
    {
        Message msg;
        receive(i, &msg);
        if (msg.s_header.s_magic == MESSAGE_MAGIC && msg.s_header.s_type == DONE)
        {
            continue;
        }
    }
}

void child_work(struct child_arguments args)
{
    /* Child arguments */
    local_id self_id = args.self_id;
    int count_nodes = args.count_nodes;

    /* System process identifiers used for logs */
    pid_t self_pid = getpid();
    pid_t parent_pid = getppid();

    /* STUDENT IMPLEMENTATION STARTED */
    char buf[BUF_SIZE];
    Message msg;
    snprintf(buf, BUF_SIZE, log_started_fmt, 0, self_id, self_pid, parent_pid, 0);
    fill_message(&msg, STARTED, 0, buf, strlen(buf));
    send_multicast(&msg);
    shared_logger(buf);

    /* 接收消息 */
    Message recv_msg;
    int cnt = 0;
    for (int i = 1; i < count_nodes; i++)
    {
        if (i == self_id)
            continue;
        receive(i, &recv_msg);
        if (recv_msg.s_header.s_magic == MESSAGE_MAGIC && recv_msg.s_header.s_type == STARTED)
        {
            cnt++;
        }
    }
    if (cnt == count_nodes - 2)
    {
        snprintf(buf, BUF_SIZE, log_received_all_started_fmt, 0, self_id);
        shared_logger(buf);
    }

    /* 发送DONE消息 */
    snprintf(buf, BUF_SIZE, log_done_fmt, 0, self_id, 0);
    fill_message(&msg, DONE, 0, buf, strlen(buf));
    send_multicast(&msg);
    shared_logger(buf);

    /* 接收DONE消息 */
    cnt = 0;
    for (int i = 1; i < count_nodes; i++)
    {
        if (i == self_id)
            continue;
        receive(i, &recv_msg);
        if (recv_msg.s_header.s_magic == MESSAGE_MAGIC && recv_msg.s_header.s_type == DONE)
        {
            cnt++;
        }
    }
    if (cnt == count_nodes - 2)
    {
        snprintf(buf, BUF_SIZE, log_received_all_done_fmt, 0, self_id);
        shared_logger(buf);
    }
}
