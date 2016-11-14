#ifndef _Q4SMESSAGE_H_
#define _Q4SMESSAGE_H_

#include <string>

enum Q4SMRequestOrResponse
{ Q4SMREQUESTORRESPOND_INVALID
, Q4SMREQUESTORRESPOND_REQUEST
, Q4SMREQUESTORRESPOND_RESPONSE
};

enum Q4SMType
{ Q4SMTYPE_INVALID
, Q4SMTYPE_BEGIN
, Q4SMTYPE_READY
, Q4SMTYPE_PING
, Q4SMTYPE_BWIDTH
, Q4SMTYPE_CANCEL
, Q4SMTYPE_Q4SALERT
};

class Q4SMessage
{
    public:

    // Constructor-Destructor
    Q4SMessage( );
    ~Q4SMessage( );

    // Init-Done
    bool    init(   Q4SMRequestOrResponse q4SMRequestOrResponse,
                    Q4SMType q4SMType, 
                    std::string host, 
                    std::string port);
    void    done( );

    // Get Message
    std::string getMessage();

private:

    void    clear( );

    void    makeFirstLine(Q4SMRequestOrResponse q4SMRequestOrResponse, Q4SMType q4SMType, std::string host, std::string port);
    void    makeFirstLineRequest(Q4SMType q4SMType, std::string host, std::string port);
    void    makeFirstLineRequestMethod(Q4SMType q4SMType);
    void    makeFirstLineRequestURI(std::string host, std::string port);
    void    makeFirstLineRequestVersion();
    void    makeHeaders();
    void    makeBody();

    std::string mMessage;
};

#endif  // _Q4SMESSAGE_H_
