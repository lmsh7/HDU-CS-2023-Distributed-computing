#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

#include "labs_headers/message.h"
#include "labs_headers/log.h"
#include "labs_headers/process.h"
#include "labs_headers/banking.h"

//export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/workspaces/HDU-CS-2023-Distributed-computing/task_lab2"
//LD_PRELOAD=./libdistributedmodel.so ./lab -l 2 -p 3 10 20 30

timestamp_t Lamport_time = 0;

int max(int a, int b) {
    return (a > b) ? a : b;
}


void update_history(BalanceHistory *history, timestamp_t pending_start_time, timestamp_t pending_end_time, balance_t amount, balance_t pending_money)
{
    int last_time = history->s_history[history->s_history_len - 1].s_time;
    int last_balance = history->s_history[history->s_history_len - 1].s_balance;
    for (int i = last_time + 1; i < pending_start_time; i++)
    {
        history->s_history[history->s_history_len].s_time = i;
        history->s_history[history->s_history_len].s_balance = last_balance;
        history->s_history[history->s_history_len].s_balance_pending_in = 0;
        history->s_history_len++;
    }
    for (int i = pending_start_time; i < pending_end_time; i++)
    {
        history->s_history[history->s_history_len].s_time = i;
        history->s_history[history->s_history_len].s_balance = last_balance;
        history->s_history[history->s_history_len].s_balance_pending_in = pending_money;
        history->s_history_len++;
    }
    history->s_history[history->s_history_len].s_balance = amount;
    history->s_history[history->s_history_len].s_time = pending_end_time;
    history->s_history[history->s_history_len].s_balance_pending_in = 0;
    history->s_history_len++;
}

balance_t now_balance(BalanceHistory *history)
{
    return history->s_history[history->s_history_len - 1].s_balance;
}

void parent_work(int count_nodes)
{
    AllHistory all_history;
    all_history.s_history_len = count_nodes - 1;

    /* STUDENT IMPLEMENTATION STARTED */
    /* Implement starting synchronization */
    for (int i = 1; i < count_nodes; i++)
    {
        Message msg;
        receive(i, &msg);
        Lamport_time = max(Lamport_time, msg.s_header.s_local_time) + 1;
        if (msg.s_header.s_magic == MESSAGE_MAGIC && msg.s_header.s_type == STARTED)
        {
            continue;
        }
    }
    /* Useful work */
    bank_operations(count_nodes - 1);

    /* Implement finishing synchronization and collecting AllHistory */
    /* 广播 STOP 消息*/
    Message msg;
    fill_message(&msg, STOP, ++Lamport_time, NULL, 0);
    send_multicast(&msg);
    /* 收到所有 DONE 消息 */
    for (int i = 1; i < count_nodes; i++)
    {
        Message msg;
        receive(i, &msg);
        Lamport_time = max(Lamport_time, msg.s_header.s_local_time) + 1;
        if (msg.s_header.s_magic == MESSAGE_MAGIC && msg.s_header.s_type == DONE)
        {
            continue;
        }
    }
    /* 收集所有 BalanceHistory */
    for (int i = 1; i < count_nodes; i++)
    {
        Message msg;
        receive(i, &msg);
        Lamport_time = max(Lamport_time, msg.s_header.s_local_time) + 1;
        if (msg.s_header.s_magic == MESSAGE_MAGIC && msg.s_header.s_type == BALANCE_HISTORY)
        {
            BalanceHistory *history = (BalanceHistory *)msg.s_payload;
            memcpy(&all_history.s_history[i - 1], history,msg.s_header.s_payload_len);
        }
    }
    print_history(&all_history);
    return;
}

void child_work(struct child_arguments args)
{
    /* Child arguments */
    local_id self_id = args.self_id;
    int count_nodes = args.count_nodes;
    uint8_t balance = args.balance;

    /* BalanceHistory initialization */
    BalanceHistory history;
    history.s_history_len = 1;
    history.s_id = self_id;
    memset(history.s_history, 0, sizeof(history.s_history));
    history.s_history[0].s_balance = balance;
    for (int i = 0; i < MAX_T; ++i) {
        history.s_history[i].s_time = i;
    }

    /* System process identifiers used for logs */
    pid_t self_pid = getpid();
    pid_t parent_pid = getppid();

    /* STUDENT IMPLEMENTATION STARTED */
    char buf[BUF_SIZE];
    Message msg;
    timestamp_t current = ++Lamport_time;
    snprintf(buf, BUF_SIZE, log_started_fmt, current, self_id, self_pid, parent_pid, balance);
    fill_message(&msg, STARTED, current, buf, strlen(buf));
    send_multicast(&msg);
    shared_logger(buf);

    Message recv_msg;
    int cnt = 0;
    for (int i = 1; i < count_nodes; i++) {
        if (i == self_id)
            continue;
        receive(i, &recv_msg);
        Lamport_time = max(Lamport_time, recv_msg.s_header.s_local_time) + 1;
        if (recv_msg.s_header.s_magic == MESSAGE_MAGIC && recv_msg.s_header.s_type == STARTED) {
            cnt++;
        }
    }
    if (cnt == count_nodes - 2) {
        current = Lamport_time;
        snprintf(buf, BUF_SIZE, log_received_all_started_fmt, current, self_id);
        shared_logger(buf);
    }

    /* 阶段 2 等待并处理任何 TRANSFER 和 STOP 消息 */
    cnt = 0;
    while (true) {
        Message req_msg;
        receive_any(&req_msg);
        Lamport_time = max(Lamport_time, req_msg.s_header.s_local_time) + 1;
        if (req_msg.s_header.s_magic == MESSAGE_MAGIC && req_msg.s_header.s_type == TRANSFER)
        {   
            TransferOrder *order = (TransferOrder *)req_msg.s_payload;
            if (order->s_src == self_id) {
                current = ++Lamport_time;

                update_history(&history, current, current, now_balance(&history) - order->s_amount, 0);

                snprintf(buf, BUF_SIZE, log_transfer_out_fmt, current, self_id, order->s_amount, order->s_dst);
                shared_logger(buf);

                Message response_msg;
                fill_message(&response_msg, TRANSFER, current, order, strlen((const char*)order));
                send(order->s_dst, &response_msg);
            } else if (order->s_dst == self_id) {
                current = max(Lamport_time, req_msg.s_header.s_local_time) + 1;
                update_history(&history, req_msg.s_header.s_local_time, current, now_balance(&history) + order->s_amount, order->s_amount);

                snprintf(buf, BUF_SIZE, log_transfer_in_fmt, current, self_id, order->s_amount, order->s_src);
                shared_logger(buf);

                Message response_msg;
                fill_message(&response_msg, ACK, current, NULL, 0);
                send(0, &response_msg);
            }
        } else if (req_msg.s_header.s_magic == MESSAGE_MAGIC && req_msg.s_header.s_type == STOP) {
            current = max(Lamport_time, req_msg.s_header.s_local_time) + 1;

            /* 阶段 3 发送 DONE 消息 */
            snprintf(buf, BUF_SIZE, log_done_fmt, current, self_id, now_balance(&history));
            shared_logger(buf);

            Message response_msg;
            fill_message(&response_msg, DONE, current, buf, strlen(buf));
            send_multicast(&response_msg);
            break;
        } else if (req_msg.s_header.s_magic == MESSAGE_MAGIC && req_msg.s_header.s_type == DONE) {
            cnt++;
        }
    }


    /* 接收 DONE 消息 */
    while (cnt != count_nodes - 2)
    {
        Message msg;
        receive_any(&msg);
        Lamport_time = max(Lamport_time, msg.s_header.s_local_time) + 1;
        if (msg.s_header.s_magic == MESSAGE_MAGIC && msg.s_header.s_type == DONE)
        {
            cnt++;
        }
    }

    if (cnt == count_nodes - 2)
    {
        current = Lamport_time;
        snprintf(buf, BUF_SIZE, log_received_all_done_fmt, current, self_id);
        shared_logger(buf);
    }

    /* 发送 BALANCE_HISTORY 消息 */
    Message history_msg;
    current = ++Lamport_time;
    fill_message(&history_msg, BALANCE_HISTORY, current, &history, sizeof(history));
    send(0, &history_msg);
}

void transfer(local_id src, local_id dst,
              balance_t amount)
{
    TransferOrder order = {src, dst, amount};

    /* STUDENT IMPLEMENTATION STARTED */


    /* 发送 TRANSFER 消息 */
    Message msg;

    fill_message(&msg, TRANSFER, ++Lamport_time, &order, sizeof(order));
    send(src, &msg);
    /* 接受 dst 的 ACK 消息*/

    receive(dst, &msg);
    Lamport_time = max(Lamport_time, msg.s_header.s_local_time) + 1;
    if (msg.s_header.s_magic == MESSAGE_MAGIC && msg.s_header.s_type == ACK)
    {
        return;
    }
}

/* STUDENTS SHOULD NOT CHANGE THIS FUNCTION */
__attribute__((weak)) void bank_operations(local_id max_id)
{
    for (int i = 1; i < max_id; ++i) {
        transfer(i, i + 1, i);
    }
    if (max_id > 1) {
        transfer(max_id, 1, 1);
    }
}
