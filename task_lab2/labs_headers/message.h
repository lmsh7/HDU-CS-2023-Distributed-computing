/**
 * @file     message.h
 * @Author   Michael Kosyakov, Evgeniy Ivanov and Denis Tarakanov (ifmo.distributedclass@gmail.com)
 * @brief    Definitions of data structures and functions related to communication in system
 *
 * Students must not modify this file!
 */

#ifndef ITMO_HDU_DISTRIBUTED_SYSTEMS_MESSAGE_H
#define ITMO_HDU_DISTRIBUTED_SYSTEMS_MESSAGE_H

#include <stddef.h>
#include <stdint.h>

//------------------------------------------------------------------------------

typedef int8_t local_id;
typedef int16_t timestamp_t;

enum {
    MESSAGE_MAGIC = 0xAFAF,
    MAX_MESSAGE_LEN = 4096,
    PARENT_ID = 0,
    MAX_PROCESS_ID = 15,
    BUF_SIZE = 256
};

typedef enum {
    STARTED = 0,     ///< message with string (doesn't include trailing '\0')
    DONE,            ///< message with string (doesn't include trailing '\0')
    ACK,             ///< empty message
    STOP,            ///< empty message
    TRANSFER,        ///< message with TransferOrder
    BALANCE_HISTORY, ///< message with BalanceHistory
    CS_REQUEST,      ///< empty message
    CS_REPLY,        ///< empty message
    CS_RELEASE       ///< empty message
} MessageType;

typedef struct {
    uint16_t     s_magic;        ///< magic signature, must be MESSAGE_MAGIC
    uint16_t     s_payload_len;  ///< length of payload
    int16_t      s_type;         ///< type of the message
    timestamp_t  s_local_time;   ///< set by sender, depends on particular lab
} __attribute__((packed)) MessageHeader;

enum {
    MAX_PAYLOAD_LEN = MAX_MESSAGE_LEN - sizeof(MessageHeader)
};

typedef struct {
    MessageHeader s_header;
    char s_payload[MAX_PAYLOAD_LEN]; ///< Must be used as a buffer, unused "tail"
                                     ///< shouldn't be transfered
} __attribute__((packed)) Message;

//------------------------------------------------------------------------------

/** Helper function, automatically sets some Message fields.
 *
 * Should initialize message length, type, payload, magic and timestamp.
 *
 * @param msg       Message structure allocated by caller to be inited
 * @parame type     Type of message
 * @param time      Timestamp of message (0 for lab1, physical time for lab2 and Lamport's time for other).
 * @param payload   Pointer to payload, which will be copied to message, can be NULL
 * @param psize     Size of payload, can be 0 for empty payload (STOP message)
 */
void fill_message(Message * msg, MessageType type, timestamp_t time, void * payload, size_t psize);

//------------------------------------------------------------------------------

/** Send a message to the process specified by id.
 *
 * @param dst     ID of recepient
 * @param msg     Message to send
 *
 * @return 0 on success, terminate model on errors
 */
int send(local_id dst, const Message * msg);

//------------------------------------------------------------------------------

/** Send multicast message.
 *
 * Send msg to all other processes including parent.
 *
 * @param msg     Message to multicast.
 *
 * @return 0 on success, terminate model on errors
 */
int send_multicast(const Message * msg);

//------------------------------------------------------------------------------

/** Receive a message from the process specified by id.
 *
 * @param from    ID of the process to receive message from
 * @param msg     Message structure allocated by the caller
 *
 * @return 0 on success, terminate model on errors
 */
int receive(local_id from, Message * msg);

//------------------------------------------------------------------------------

/** Receive a message from any process.
 *
 * @param msg     Message structure allocated by the caller
 *
 * @return id of process, which message was received
 */
int receive_any(Message * msg);

//------------------------------------------------------------------------------

#endif // ITMO_HDU_DISTRIBUTED_SYSTEMS_MESSAGE_H
