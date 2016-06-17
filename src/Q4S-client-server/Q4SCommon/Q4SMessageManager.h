#ifndef _Q4SMESSAGEMANAGER_H_
#define _Q4SMESSAGEMANAGER_H_

#include "Q4SCriticalSection.h"

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
    void addMessage( std::string &message );

    bool readFirst( std::string &firstMessage);

private:

    void    clear( );

    std::list < std::string >   mMessages;
    Q4SCriticalSection          mcsMessagesAccess;
    HANDLE                      mevMessageReady;
};

#endif  // _Q4SMESSAGEMANAGER_H_
