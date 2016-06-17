#include "Q4SCriticalSection.h"

void Q4SCriticalSection::clear()
{
    ok = false;
    mutex = 0;
}

bool Q4SCriticalSection::init( int cfgMode )
{
    if( ok )
    {
        done( );
    }

    mode = cfgMode;
    switch( mode )
    {
        case FMCS_CSMODE:
            InitializeCriticalSection ( &criticalSection );
            ok = true;
            break;
        case FMCS_MTXMODE:
            mutex = CreateMutex(NULL, FALSE, NULL);
            ok = true;
            break;

    }

    return ok;
}

void Q4SCriticalSection::done( )
{
    if( ok ) 
    {
        switch( mode )
        {
            case FMCS_CSMODE:
                DeleteCriticalSection( &criticalSection );
                break;
            case FMCS_MTXMODE:
                CloseHandle( mutex );
                break;
        }
    }

    clear();
}

void Q4SCriticalSection::enter( ) const
{
    if( ok ) 
    {
        switch( mode )
        {
            case FMCS_CSMODE:
                EnterCriticalSection( &criticalSection );
                break;
            case FMCS_MTXMODE:
                WaitForSingleObject( mutex, INFINITE );
                break;
        }
    }
}

void Q4SCriticalSection::leave( ) const
{
    if( ok ) 
    {
        switch( mode )
        {
            case FMCS_CSMODE:
                LeaveCriticalSection( &criticalSection );
                break;
            case FMCS_MTXMODE:
                ReleaseMutex( mutex );
                break;
        }
    }
}
