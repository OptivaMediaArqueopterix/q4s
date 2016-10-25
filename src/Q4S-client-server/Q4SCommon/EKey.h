/**
 *
 * @file EKey.h
 *
 * @brief Función que manejará las teclas
 *
*/
#ifndef _EKEY_H_
#define _EKEY_H_

#include "EKeyCodes.h"

#ifdef WIN32
  #include <windows.h>
#endif //WIN32

// System inmediate key query ( see EKeyCodes.h )

void EKey_enabled     ( bool enable );
bool EKey_getKeyState ( unsigned long keyCode );


#endif //_EKEY_H_
