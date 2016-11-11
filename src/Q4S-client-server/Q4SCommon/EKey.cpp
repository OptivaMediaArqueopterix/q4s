#include "EKey.h"
#include <windows.h>
#include <conio.h>

static bool enabled = true;

void EKey_enabled ( bool enable )
{
  enabled = enable;
}

/**
 *
 * Devuelve el estado de la tecla
 *
 * Devuelve true en caso de que la tecla esté pulsada
 *
 * @param[in] keyCode Valor que describe una tecla 
 *
 * @return devuelve true en caso de que la tecla este pulsada
*/
bool EKey_getKeyState ( unsigned long keyCode )
{
  if ( enabled ) 
  {
    return (GetKeyState(keyCode)&8000)!=0;
  }
  else
  {
    return false;
  }
}

bool EKey_anyKey ()
{
  return _kbhit ()?true:false;
}
