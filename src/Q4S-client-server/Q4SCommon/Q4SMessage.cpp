#include "Q4SMessage.h"

Q4SMessage::Q4SMessage( )
{
    clear();
}

Q4SMessage::~Q4SMessage( )
{
    done();
}

bool Q4SMessage::init(Q4SMRequestOrResponse q4SMRequestOrResponse, Q4SMType q4SMType, std::string host, std::string port)
{
    done();

    bool ok = true;

    // FirstLine
    makeFirstLine(q4SMRequestOrResponse, q4SMType, host, port);
    // Headers
    makeHeaders();
    //CRLF
    mMessage.append("\n");
    // Body
    makeBody();

    return ok;
}

bool Q4SMessage::init(Q4SMRequestOrResponse q4SMRequestOrResponse,Q4SMType q4SMType, std::string host, std::string port, Q4SSDP q4SSDP)
{
    bool ok = true;

    // init
    ok &= init(q4SMRequestOrResponse, q4SMType, host, port);

    // SDP
    mMessage.append( q4SSDP.getMessage());

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

void Q4SMessage::makeFirstLine(Q4SMRequestOrResponse q4SMRequestOrResponse, Q4SMType q4SMType, std::string host, std::string port)
{
    switch (q4SMRequestOrResponse)
    {
        case Q4SMREQUESTORRESPOND_REQUEST:
        {
            makeFirstLineRequest(q4SMType, host, port);
        }
        break;

        case Q4SMREQUESTORRESPOND_RESPONSE:
        {
            // TODO
        }
        break;

        default:
        {
        }
        break;
    }
}

void Q4SMessage::makeFirstLineRequest(Q4SMType q4SMType, std::string host, std::string port)
{
    // Method
    makeFirstLineRequestMethod(q4SMType);
    mMessage.append(" ");

    // Request-URI
    makeFirstLineRequestURI(host, port);
    mMessage.append(" ");

    // Q4S-Version
    mMessage.append(" ");
    makeFirstLineRequestVersion();
}

void Q4SMessage::makeFirstLineRequestMethod(Q4SMType q4SMType)
{
    std::string stringType = "NOT_DEFINED_TYPE";
    switch (q4SMType)
    {
        case Q4SMTYPE_BEGIN:
        {
            stringType = "BEGIN";
        }
        break;

        case Q4SMTYPE_READY:
        {
            stringType = "READY";
        }
        break;

        case Q4SMTYPE_PING:
        {
            stringType = "PING";
        }
        break;

        case Q4SMTYPE_BWIDTH:
        {
            stringType = "BWIDHT";
        }
        break;

        case Q4SMTYPE_CANCEL:
        {
            stringType = "CANCEL";
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

void Q4SMessage::makeFirstLineRequestURI(std::string host, std::string port)
{
    std::string stringUri = "q4s:";
    stringUri.append("//");
    stringUri.append(host);
    stringUri.append(":");
    stringUri.append(port);

    mMessage.append(stringUri);
}

void Q4SMessage::makeFirstLineRequestVersion()
{
    mMessage.append("Q4S/1.0");
}

void Q4SMessage::makeHeaders()
{
    // TODO
}

void Q4SMessage::makeBody()
{
    // TODO
}

