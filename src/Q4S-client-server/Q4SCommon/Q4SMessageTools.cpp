#include "Q4SMessageTools.h"

#include <sstream>

bool Q4SMessageTools_isPingMessage(std::string message, int *pingNumber, unsigned long *timeStamp)
{
    bool ok = true;

    // Auxiliar string
    std::string extracted;

    // Convert message to a stringstream 
    std::istringstream messageStream (message);

    // Get method from message
    if ( ok )
    {
        std::getline(messageStream, extracted, ' ');
        // Check if method is ping
        if ( extracted.compare("PING") != 0)
        {
            ok = false;
        }
    }

    // Discard rest of first line
    if (ok)
    {
        std::getline(messageStream, extracted);
    }

    // Get pingNumberfrom message
    if ( ok )
    {
        std::string sequenceNumberLineFirstPart;
        std::string sequenceNumberLineSecondPart;

        // Check text
        std::string sequenceNumberText = "Sequence-Number";
        getline( messageStream, sequenceNumberLineFirstPart, '=' );
        getline( messageStream, sequenceNumberLineSecondPart);
        if ( sequenceNumberLineFirstPart.compare( sequenceNumberText ) == 0)
        {
            *pingNumber = std::stoi(sequenceNumberLineSecondPart);
        }
        else
        {
        }

    }
    
    // Get TimeStamp
    if ( ok )
    {
        std::string timestampLineFirstPart;
        std::string timestampLineSecondPart;

        // Check text
        std::string timestampText = "Timestamp";
        getline( messageStream, timestampLineFirstPart, '=' );
        getline( messageStream, timestampLineSecondPart);
        if ( timestampLineFirstPart.compare( timestampText ) == 0)
        {
            *timeStamp = std::stoi(timestampLineSecondPart);
        }
        else
        {
        }
    }

    return ok;
}