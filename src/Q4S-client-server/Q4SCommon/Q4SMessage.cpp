#include "Q4SMessage.h"

Q4SMessage::Q4SMessage( )
{
    clear();
}

Q4SMessage::~Q4SMessage( )
{
    done();
}


bool Q4SMessage::init(Q4SMRequestOrResponse q4SMRequestOrResponse, Q4SMType q4SMType)
{
    done();

    bool ok = true;

    // FirstLine
    makeFirstLine(q4SMRequestOrResponse, q4SMType);
    // Headers
    makeHeaders();
    //CRLF
    mMessage.append("\n");
    // Body
    makeBody();

    return ok;
}

std::string Q4SMessage::getMessage()
{
    return mMessage;
}

void Q4SMessage::done( )
{
}

// ---private----------------------------------------------------------------------------------------------------------------

void Q4SMessage::clear( )
{
    mMessage.clear();
}

void Q4SMessage::makeFirstLine(Q4SMRequestOrResponse q4SMRequestOrResponse, Q4SMType q4SMType)
{
    switch (q4SMRequestOrResponse)
    {
        case Q4SMREQUESTORRESPOND_REQUEST:
        {
            makeFirstLineRequest(q4SMType);
        }
        break;

        case Q4SMREQUESTORRESPOND_RESPONSE:
        {
        }
        break;

        default:
        {
        }
        break;
    }
}

void Q4SMessage::makeFirstLineRequest(Q4SMType q4SMType)
{
    std::string stringType = "NOT_DEFINED_TYPE";
    switch (q4SMType)
    {
        case Q4SMTYPE_BEGIN:
        {
        }
        break;

        case Q4SMTYPE_READY:
        {
        }
        break;

        case Q4SMTYPE_PING:
        {
        }
        break;

        case Q4SMTYPE_BWIDTH:
        {
        }
        break;

        case Q4SMTYPE_CANCEL:
        {
        }
        break;

        case Q4SMTYPE_Q4SALERT:
        {
            stringType = "Q4S-ALERT";
        }
        break;
    }

    mMessage.append(stringType);
}

void Q4SMessage::makeHeaders()
{
}

void Q4SMessage::makeBody()
{
}

