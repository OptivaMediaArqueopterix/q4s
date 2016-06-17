#ifndef _Q4SCriticalSection_h_
#define _Q4SCriticalSection_h_

#include <windows.h>

class Q4SCriticalSection
{
    public:

        Q4SCriticalSection( )                               { clear( );  }
        ~Q4SCriticalSection( )                              { done( );   }


        // Init-Done

        bool    init     ( int cfgMode = FMCS_CSMODE );
        void    done     ( );


        // Enter-Leave

        void    enter    ( ) const;
        void    leave    ( ) const;

        enum    Mode { FMCS_CSMODE = 0, FMCS_MTXMODE };

  //----------------------------------------------------------------------------------------------------

    private:
  
        void    clear( );

        bool                         ok;
        mutable CRITICAL_SECTION     criticalSection;
        HANDLE                       mutex;
        int                          mode;
};

#endif //_Q4SCriticalSection_h_
