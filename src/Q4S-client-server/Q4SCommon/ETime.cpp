#include "ETime.h"

#ifdef _WIN32
  #include <windows.h>
#endif //_WIN32

#ifdef WIN32
  #pragma comment( lib, "winmm.lib" )
#endif //WIN32

unsigned long ETime_getTime()
{
  unsigned long currentTime = 0;
  
  #ifdef _WIN32

    static bool initdone = false;
    static bool supported;
    static LARGE_INTEGER frequency;

    if ( !initdone )
    {
      supported = QueryPerformanceFrequency( &frequency )?true:false;
      initdone = true;
    }

    if ( supported )
    {
      LARGE_INTEGER count;
      QueryPerformanceCounter( &count );      
      currentTime = (unsigned long)((count.QuadPart*1000)/frequency.QuadPart);
    }
    else
    {
      currentTime = GetTickCount();
    }
    
  #else //_WIN32
    // TO PORT!!!!!!!
    int currentTime = 0;
  #endif //_WIN32

	return currentTime;
}

void ETime_delay( int msecs )
{
  #ifdef _WIN32
    Sleep ( msecs );
  #else //_WIN32
    // TO PORT!!!!!!!
  #endif //_WIN32
}

float ETime_getPrecisseTime()
{
  #ifdef _WIN32

    static bool initdone = false;
    static bool supported;
    static LARGE_INTEGER frequency;

    if ( !initdone )
    {
      supported = QueryPerformanceFrequency( &frequency )?true:false;
      initdone = true;
    }

    if ( supported )
    {
      LARGE_INTEGER count;
      QueryPerformanceCounter( &count );
      float secs = (float)( (double)count.QuadPart / ((double)frequency.QuadPart) );
      return secs*1000.f;
    }
    else
    {
      return (float) ETime_getTime();
    }

  #else //_WIN32

    // TO PORT!!!!!!!
    return 0;

  #endif //_WIN32
  
}

void ETime_getDecomposedTime ( unsigned long msecs, ETime_decomposedTime* decomposedTime )
{
  decomposedTime->totalmiliseconds = msecs;
  decomposedTime->totalseconds     = decomposedTime->totalmiliseconds/1000;
  decomposedTime->totalminutes     = decomposedTime->totalseconds/60;
  decomposedTime->totalhours       = decomposedTime->totalminutes/60;
  decomposedTime->totaldays        = decomposedTime->totalhours/24;

  decomposedTime->days             = decomposedTime->totaldays;
  decomposedTime->hours            = decomposedTime->totalhours       - (decomposedTime->totaldays*24);
  decomposedTime->minutes          = decomposedTime->totalminutes     - (decomposedTime->totalhours*60);
  decomposedTime->seconds          = decomposedTime->totalseconds     - (decomposedTime->totalminutes*60);
  decomposedTime->miliseconds      = decomposedTime->totalmiliseconds - (decomposedTime->totalseconds*1000);
}

