#ifndef SNIFFER_
#define SNIFFER_

/**
 * Main runtime of the daemon,
 * it's basically listening to the daemon's personal
 * FIFO channel, and when it receives a request it
 * acts accordingly.
 */

// Starts the sniffing.
int StartRuntime();


#endif // SNIFFER_