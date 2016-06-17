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


void Q4SMessageManager::addMessage( std::string &message )
{
    bool signal = false;
    mcsMessagesAccess.enter( );
    if( mMessages.size( ) == 0 )
    {
        signal = true;
    }
    //printf( "Writing first message\n" );
    mcsMessagesAccess.leave( );
    if( signal == true )
    {
        SetEvent( mevMessageReady );
    }
    mMessages.push_back( message );
}

bool Q4SMessageManager::readFirst( std::string &firstMessage)
{
    bool    ok = true;
    DWORD   waitResult;

    //printf( "Waiting for first message\n" );
    waitResult = WaitForSingleObject( mevMessageReady, INFINITE );
    mcsMessagesAccess.enter( );

    //printf( "Reading first message\n" );
    firstMessage = mMessages.front();
    //printf( "First message: <%s>\n", firstMessage.c_str( ) );
    mMessages.pop_front();
    ResetEvent( mevMessageReady );

    mcsMessagesAccess.leave( );

    return ok;
}