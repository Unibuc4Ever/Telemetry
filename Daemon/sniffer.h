#ifndef SNIFFER_
#define SNIFFER_

#include "callback_storage.h"
#include "history_storage.h"

#include "request_processors.h"
#include "senders.h"

#include "standard.h"

/**
 * Main runtime of the daemon,
 * it's basically listening to the daemon's personal
 * FIFO channel, and when it receives a request it
 * acts accordingly.
 */

// Starts the sniffing.
int StartRuntime();


#endif // SNIFFER_