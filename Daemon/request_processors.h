#ifndef REQUEST_PROCESSORS_
#define REQUEST_PROCESSORS_

#include <stdio.h>
#include <string.h>

#include "standard.h"
#include "fifo_parser.h"
#include "callback_storage.h"
#include "history_storage.h"
#include "senders.h"

void ProcessBroadcastRequest(FifoParser* parser);

void ProcessCallbackRequest(FifoParser* parser);

void ProcessCallbackCancelRequest(FifoParser* parser);

void ProcessHistoryRequest(FifoParser* parser);

#endif // REQUEST_PROCESSORS_