/**
 * @file     logger.h
 * @Author   Michael Kosyakov, Evgeniy Ivanov and Denis Tarakanov (ifmo.distributedclass@gmail.com)
 * @brief    Definitions of data structures and functions related to required logging
 *
 * Students must not modify this file!
 */

#ifndef ITMO_HDU_DISTRIBUTED_SYSTEMS_LOG_H
#define ITMO_HDU_DISTRIBUTED_SYSTEMS_LOG_H

/*
 * <timestamp> process <local id> (pid <PID>, paranet <PID>) has STARTED with balance $<id>
 */
static const char * const log_started_fmt =
    "%d: process %1d (pid %5d, parent %5d) has STARTED with balance $%2d\n";

static const char * const log_received_all_started_fmt =
    "%d: process %1d received all STARTED messages\n";

static const char * const log_done_fmt =
    "%d: process %1d has DONE with balance $%2d\n";

static const char * const log_received_all_done_fmt =
    "%d: process %1d received all DONE messages\n";

/* For banking system laboratory works */
static const char * const log_transfer_out_fmt =
    "%d: process %1d transferred $%2d to process %1d\n";

/* For banking system laboratory works */
static const char * const log_transfer_in_fmt =
    "%d: process %1d received $%2d from process %1d\n";

/* For mutual exclusion laboratory works
 * Iteration enumerated starting from 1, i.e.
 * 1, 2, 3, 4 out of 4.
 */
static const char * const log_loop_operation_fmt =
    "process %1d is doing %d iteration out of %d\n";

//------------------------------------------------------------------------------
// Functions below are implemented by lector
//------------------------------------------------------------------------------

/** Prints string to required log outputs (events.log and stdout)
 *
 * @param msg     String to be printed to log outputs
 */
void shared_logger(const char * msg);

//------------------------------------------------------------------------------

/** Prints log_loop_operation_fmt inside critical section
 *
 * Should be used only for mutual exclusion laboratory works
 *
 * @param s     String to be printed in CS
 */
void print(const char * s);

#endif // ITMO_HDU_DISTRIBUTED_SYSTEMS_LOG_H
