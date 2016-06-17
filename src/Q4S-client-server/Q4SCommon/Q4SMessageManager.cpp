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


    return ok;
}

void Q4SMessageManager::done( )
{

}

void Q4SMessageManager::clear( )
{

}


void Q4SMessageManager::addMessage( std::string &message )
{
    mMessages.push_back( message );
}

bool Q4SMessageManager::readFirst( std::string &firstMessage)
{
    bool ok = true;

    if ( mMessages.size() > 0 )
    {
        firstMessage = mMessages.front();
        mMessages.pop_front();
    }
    else
    {
        ok = false;
    }

    return ok;
}