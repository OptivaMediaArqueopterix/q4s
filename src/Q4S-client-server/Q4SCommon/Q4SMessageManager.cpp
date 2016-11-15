#include "Q4SMessageManager.h"

Q4SMessageManager::Q4SMessageManager( )
{
    clear();
}

Q4SMessageManager::~Q4SMessageManager( )
{
    done();
}


bool Q4SMessageManager::init( )
{
    done();

    bool ok = true;
    mcsMessagesAccess.init( Q4SCriticalSection::FMCS_MTXMODE );
    mevMessageReady = CreateEvent( NULL, TRUE, FALSE, NULL );

    return ok;
}

void Q4SMessageManager::done( )
{
    mcsMessagesAccess.done( );
}

void Q4SMessageManager::clear( )
{

}


void Q4SMessageManager::addMessage( std::string &message, unsigned long timestamp )
{
    bool signal = false;
    mcsMessagesAccess.enter( );
    if( mMessages.size( ) == 0 )
    {
        signal = true;
    }
    //printf( "Writing first message\n" );
    if( signal == true )
    {
        SetEvent( mevMessageReady );
    }
    Q4SMessageInfo messageInfo;
    messageInfo.message = message;
    messageInfo.timeStamp = timestamp;
    mMessages.push_back( messageInfo );
    mcsMessagesAccess.leave( );
}

bool Q4SMessageManager::readFirst( std::string &firstMessage )
{
    bool    ok = true;
    DWORD   waitResult;

    //printf( "Waiting for first message\n" );
    waitResult = WaitForSingleObject( mevMessageReady, INFINITE );
    mcsMessagesAccess.enter( );

    //printf( "Reading first message\n" );
    if( mMessages.size( ) == 0 )
    {
        printf( "FATAL ERROR. Message available but stolen by another thread.\n" );
        ok = false;
    }
    else
    {
        firstMessage = mMessages.front().message;
        //printf( "First message: <%s>\n", firstMessage.c_str( ) );
        mMessages.pop_front();
        if( mMessages.size( ) == 0 )
        {
            ResetEvent( mevMessageReady );
        }
    }

    mcsMessagesAccess.leave( );

    return ok;
}

bool Q4SMessageManager::readMessage( std::string& pattern, Q4SMessageInfo& messageInfo, bool erase )
{
    bool    found = false;
    std::list< Q4SMessageInfo >::iterator  itr_msg;

    WaitForSingleObject( mevMessageReady, INFINITE );
    mcsMessagesAccess.enter( );

    for( itr_msg = mMessages.begin( ); ( found == false ) && ( itr_msg != mMessages.end( ) ); itr_msg++ )
    {
        if( itr_msg->message.substr( 0, pattern.size( ) ).compare( pattern ) == 0 )
        {
            // Message found.
            found = true;
            messageInfo.message = itr_msg->message;
            messageInfo.timeStamp = itr_msg->timeStamp;
        }
    }

    if (erase && found)
    {
        itr_msg--;
        mMessages.erase(itr_msg);
        if( mMessages.size( ) == 0 )
        {
            ResetEvent( mevMessageReady );
        }
    }

    mcsMessagesAccess.leave( );

    return found;
}

bool Q4SMessageManager::readPingMessage( int pingIndex, Q4SMessageInfo& messageInfo, bool erase )
{
    bool    found = false;
    std::list< Q4SMessageInfo >::iterator  itr_msg;
    std::string pattern = "PING";

    WaitForSingleObject( mevMessageReady, INFINITE );
    mcsMessagesAccess.enter( );

    for( itr_msg = mMessages.begin( ); ( found == false ) && ( itr_msg != mMessages.end( ) ); itr_msg++ )
    {
        if( itr_msg->message.substr( 0, pattern.size( ) ).compare( pattern ) == 0 )
        {
            std::string sequenceNumber = "Sequence-Number=";
            sequenceNumber.append(std::to_string((long double)pingIndex));
            //Buscar a ver si est� la cadena
            if ( itr_msg->message.find( sequenceNumber ) != std::string::npos)
            {
                // Message found.
                found = true;
                messageInfo.message = itr_msg->message;
                messageInfo.timeStamp = itr_msg->timeStamp;
            }
        }
    }

    if (erase && found)
    {
        itr_msg--;
        mMessages.erase(itr_msg);
        if( mMessages.size( ) == 0 )
        {
            ResetEvent( mevMessageReady );
        }
    }

    mcsMessagesAccess.leave( );

    return found;
}

int Q4SMessageManager::size()
{
    return mMessages.size();
}
