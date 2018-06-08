#ifndef _Q4SMESSAGEMANAGER_H_
#define _Q4SMESSAGEMANAGER_H_

#include "Q4SCriticalSection.h"
#include "Q4SMessageInfo.h"

#include <list>
#include <string>

class Q4SMessageManager 
{
    public:

    // Constructor-Destructor
    Q4SMessageManager( );
    ~Q4SMessageManager( );

    // Init-Done
    bool    init( );
    void    done( );

    // Add message
    void addMessage( std::string &message, unsigned long timestamp = 0 );

    bool readFirst( std::string &firstMessage );
    bool readMessage( std::string& pattern, Q4SMessageInfo& messageInfo, bool erase );
    bool readPingMessage( int pingIndex, Q4SMessageInfo& messageInfo, bool erase );
    bool read200OKMessage( Q4SMessageInfo& messageInfo, bool erase );
    bool readBandWidthMessage(unsigned long &sequenceNumber, bool erase );

    int size() const;

private:

    void    clear( );

    std::list < Q4SMessageInfo >    mMessages;
    Q4SCriticalSection              mcsMessagesAccess;
    HANDLE                          mevMessageReady;
};

#endif  // _Q4SMESSAGEMANAGER_H_
