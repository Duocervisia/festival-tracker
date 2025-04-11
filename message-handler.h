#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include <iostream>

struct struct_message {
    uint32_t deviceID;
    double latitude;
    double longitude;
    time_t unixTime;
};

class MessageHandler {
public:
    MessageHandler();
    void pushMessage(const struct_message& msg);
    void printMessages() const;
    void updateOwnMessage(double latitude, double longitude, time_t unixTime);
    struct_message ownMessage;
private:
    struct_message messages[10];
    int count;
};

#endif // MESSAGE_HANDLER_H