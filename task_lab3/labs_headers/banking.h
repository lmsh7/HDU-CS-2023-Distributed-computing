/**
 * @file     banking.h
 * @Author   Michael Kosyakov, Evgeniy Ivanov and Denis Tarakanov (ifmo.distributedclass@gmail.com)
 * @brief    Definitions of data structures and functions related to banking
 *
 * Students must not modify this file!
 */

#ifndef ITMO_HDU_DISTRIBUTED_SYSTEMS_BANKING_H
#define ITMO_HDU_DISTRIBUTED_SYSTEMS_BANKING_H

#include "message.h"

typedef int16_t balance_t;

/**
 * 1. "Main process" sends TransferOrder to process with id=s_src.
 * 2. s_src decreases its balance by s_amount and sends TransferOrder to s_dst.
 * 3. s_dst increases its balance by s_amount.
 * 4. s_dst sends ACK to "main process".
 */
typedef struct {
    local_id   s_src;           ///< transfer from process with this ID
    local_id   s_dst;           ///< transfer to process with this ID
    balance_t  s_amount;        ///< Money
} __attribute__((packed)) TransferOrder;

typedef struct {
    balance_t   s_balance;
    timestamp_t s_time; 
    balance_t   s_balance_pending_in;
} __attribute__((packed)) BalanceState;

enum {
    MAX_T = 255 ///< max possible value of timestamp generated by lamport's time
                ///< or get_physical_time()
};

/**
 * Describes balance state of process with id=s_id at each time t >= 0
 * and t < s_history_len
 */
typedef struct {
    local_id        s_id;
    uint8_t         s_history_len;
    BalanceState    s_history[MAX_T + 1]; ///< Must be used as a buffer, unused
                                          ///< part of array shouldn't be transfered
} __attribute__((packed)) BalanceHistory;

/**
 * Should contain balance histories of all processes in the distributed system
 * except parrent process.
 */
typedef struct {
    uint8_t          s_history_len; ///< should be equal to the number of children
    BalanceHistory   s_history[MAX_PROCESS_ID + 1];
} AllHistory;

//------------------------------------------------------------------------------
// Functions below must be implemented by students
//------------------------------------------------------------------------------

/** Transfer amount from src to dst.
 *
 * @param src       id of child process, which is source of transferring
 * @param dst       id of child process, which is destination of transferring
 * @param amount    amount of money, which should be transferred
 */
void transfer(local_id src, local_id dst, balance_t amount);

//------------------------------------------------------------------------------
// Functions below are implemented by lector
//------------------------------------------------------------------------------

/** Perform a number of transfers between various children with ids [1;max_id]
 *
 * @param max_id max    id of existing process, so that (max_id + 1) is the total
 *                      number of processes
 */
void bank_operations(local_id max_id);

/** Returns perfect physical time.
 *
 * Emulates physical clock (for each process).
 */
timestamp_t get_physical_time();

/** Returns physical time with skew.
 *
 * Emulates physical clock (for each process).
 */
timestamp_t get_physical_time_skew();

/** Pretty print for BalanceHistories.
 *
 *  @history    Pointer to all collected histories
 */
void print_history(const AllHistory * history);

#endif // ITMO_HDU_DISTRIBUTED_SYSTEMS_BANKING_H