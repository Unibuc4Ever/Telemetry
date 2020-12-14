#ifndef DAEMON_STORAGE_
#define DAEMON_STORAGE_

/**
 * This storage allows the daemon to store all the registered callbacks
 * made by users. This storage can be seen as a DS with the following
 * posibilities:
 *      1. Bind a callback (pair of type PID/callbackID) to a path
 *      2. Add a message in the DS. This returns all the affected callbacks
 *      3. Request top X messages sent at a certain path
 */

#endif // DAEMON_STORAGE_
