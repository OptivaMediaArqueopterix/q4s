#ifndef _Q4SMESSAGETOOLS_H_
#define _Q4SMESSAGETOOLS_H_

#include <string>

bool Q4SMessageTools_isPingMessage(std::string message, int *pingNumber, unsigned long *timeStamp);

#endif //_Q4SMESSAGETOOLS_H_