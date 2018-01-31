#ifndef _Q4SAGENTPROTOCOL_H_
#define _Q4SAGENTPROTOCOL_H_

#include "Q4SAgentSocket.h"
#include "..\Q4SCommon\Q4SMessageManager.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <fstream>


class Q4SAgentProtocol
{
public:

    // Constructor-Destructor
    Q4SAgentProtocol ();
    ~Q4SAgentProtocol ();

	template<size_t N>
	std::string convert(char const(&data)[N])
	{
	   return std::string(data, std::find(data, data + N, '\0'));
	}


	template <typename ValueType>
    ValueType getValueOfKey(const std::string &key, ValueType const &defaultValue = ValueType()) const
    {
	    if (contents.find(key) != contents.end())
		    return defaultValue;

	    return Convert::string_to_T<ValueType>(contents.find(key)->second);
    }


    // Init-Done
    bool    init();
    void    done();

    // Q4S Methods
	bool	listen();
    bool    Startlisten();
    void    end();

private:

    void    clear();

    bool    openConnectionListening();
	bool	openListen();
    void    closeConnectionListening();
    void    closeConnections();

    Q4SAgentSocket              mAgentSocket;
    HANDLE                      marrthrListenHandle;
	HANDLE                      marrthrListenHandle2;
    HANDLE                      marrthrDataHandle;

    bool                        manageUdpConnection( );
    static DWORD WINAPI         manageUdpConnectionsFn( LPVOID lpData );

    bool                        manageUdpReceivedData( );
    static DWORD WINAPI         manageUdpReceivedDataFn( LPVOID lpData );

	//bool						listen();
	static DWORD WINAPI			listenFn( LPVOID lpData );

    Q4SMessageManager           mReceivedMessages;

	std::map<std::string, std::string> contents;
	

};




#endif  // _Q4SAGENTPROTOCOL_H_