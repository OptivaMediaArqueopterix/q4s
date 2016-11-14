#include "Q4SSDP.h"

Q4SSDP::Q4SSDP( )
{
    clear();
}

Q4SSDP::~Q4SSDP( )
{
    done();
}


bool Q4SSDP::init(Q4SSDPParams q4SSDPParams)
{
    done();

    bool ok = true;

    // FirstLine
    // Headers
    //CRLF
    mMessage.append("\n");
    // Body

    return ok;
}

std::string Q4SSDP::getMessage()
{
    return mMessage;
}

void Q4SSDP::done( )
{
}

// ---private----------------------------------------------------------------------------------------------------------------

void Q4SSDP::clear( )
{
    mMessage.clear();
}

