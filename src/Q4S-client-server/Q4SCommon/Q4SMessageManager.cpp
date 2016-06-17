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
    mcsMessagesAccess.enter( );
    mMessages.push_back( message );
    mcsMessagesAccess.leave( );
}

bool Q4SMessageManager::readFirst( std::string &firstMessage)
{
    bool ok = true;

    mcsMessagesAccess.enter( );
    if ( mMessages.size() > 0 )
    {
        firstMessage = mMessages.front();
        mMessages.pop_front();
    }
    else
    {
        ok = false;
    }
    mcsMessagesAccess.leave( );

    return ok;
}