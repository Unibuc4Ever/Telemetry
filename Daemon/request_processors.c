#include "request_processors.h"

void ProcessBroadcastRequest(FifoParser* parser)
{
    printf("Received broadcast request:\n");
    char channel[1000], message[1000];
    memset(channel, 0, sizeof channel);
    memset(message, 0, sizeof message);

    int err, channel_length, message_length;
    err = ParseInt(parser, &channel_length);
    if (!err)
        err |= ParseString(parser, channel, channel_length);
    if (!err)
        err |= ParseInt(parser, &message_length);
    if (!err)
        err |= ParseString(parser, message, message_length);

    if (err) {
        printf("Received error %d\n", err);
        return;
    }

    printf("    Message length: %d\n    Message: %s\n", message_length, message);
    printf("    Channel length: %d\n    Channel: %s\n", channel_length, channel);

    if (!isValidPath(channel)) {
        printf(" !!! Broadcast denied because channel invalid:\n    `%s`\n", channel);
        return ;
    }

    /// Tell the clients to call their callbacks
    Callback* callbacks;
    int number_of_callbacks;
    err = StorageGetCallbacks(channel, &callbacks, &number_of_callbacks);

    // Nothing to do.
    if (err) {
        printf("Found no callbacks: %d\n", err);
        return;
    } 

    for (int i = 0; i < number_of_callbacks; i++)
        SendCallback(callbacks[i].PID, callbacks[i].token,
                     channel, message);

    /// Save the message in history
    HistoryStorageAdd(channel, message);
}

void ProcessCallbackRequest(FifoParser* parser)
{
    char channel[1000];
    int pid, token;
    memset(channel, 0, sizeof channel);

    int err, channel_length;
    err = ParseInt(parser, &token);
    if (!err)
        err |= ParseInt(parser, &pid);
    if (!err)
        err |= ParseInt(parser, &channel_length);
    if (!err)
        err |= ParseString(parser, channel, channel_length);
    
    printf("Received callback request at:\n");
    printf("    channel: \'%s\'\n", channel);
    printf("    token: %d\n", token);
    printf("    PID: %d\n", pid);
    fflush(stdout);

    if (!isValidPath(channel)) {
        printf(" !!! Callback Token Request denied, for wrong channel:\n\
    `%s`\n", channel);
        return ;
    }

    Callback callback = { pid, token };
    StorageAdd(callback, channel);
}

void ProcessCallbackCancelRequest(FifoParser* parser)
{
    printf("Received callback cancel request:\n");
    int pid, token;

    int err;
    err = ParseInt(parser, &token);
    if (!err)
        err |= ParseInt(parser, &pid);
    
    printf("Received callback cancel request at:\n");
    printf("    token: %d\n", token);
    printf("    PID: %d\n", pid);
    fflush(stdout);

    Callback callback = { pid, token };
    StorageRemove(callback);
}

void ProcessHistoryRequest(FifoParser* parser)
{
    printf("Received History request:\n");
    /// Read the request
    int max_entries, channel_length, personal_fifo_id;
    char channel[100];

    int err = 0;
    err |= ParseInt(parser, &personal_fifo_id);
    err |= ParseInt(parser, &max_entries);
    err |= ParseInt(parser, &channel_length);
    if (!err)
        err |= ParseString(parser, channel, channel_length);

    if (err)
        return ;

    if (!isValidPath(channel)) {
        printf(" !!! History Request denied, for wrong channel:\n\
    `%s`\n", channel);
        return ;
    }

    // print request
    const char** history_channels;
    const char** history_messages;
    int nr_entries;
    err |= HistoryStorageQuery(max_entries, channel, 
                &history_channels, &history_messages, &nr_entries);

    if (err)
        return ;

    printf("Performed history request, nr results: %d\n", nr_entries);

    SendHistory(personal_fifo_id, nr_entries, history_channels, history_messages);

    free(history_channels);
    free(history_messages);
    // TODO: Fix memory leaks.
}
