#ifndef _ETIME_H_
#define _ETIME_H_

struct ETime_decomposedTime
{
  unsigned long miliseconds;
  unsigned long seconds;
  unsigned long minutes;
  unsigned long hours;
  unsigned long days;

  unsigned long totalmiliseconds;
  unsigned long totalseconds;
  unsigned long totalminutes;
  unsigned long totalhours;
  unsigned long totaldays;
};

unsigned long   ETime_getTime           ( );
float           ETime_getPrecisseTime   ( );

void            ETime_delay             ( int msecs );

void            ETime_getDecomposedTime ( unsigned long msecs, ETime_decomposedTime* decomposedTime );

#endif //_ETIME_H_
