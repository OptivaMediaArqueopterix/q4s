#include "ObtainMyIP.h"

#include <winsock2.h>
#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <vector>
#include <locale>
#include <sstream>
#pragma comment(lib,"ws2_32.lib")

std::string website_HTML;
std::locale local;
void get_Website(std::string url);
char lineBuffer[200][80] = { ' ' };
char buffer[10000];
char ip_address[16];
int i = 0, bufLen = 0, j = 0, lineCount = 0;
int lineIndex = 0, posIndex = 0;

bool obtainMyIP(IPv4 &myIP)
{
    char szBuffer[1024];

    #ifdef WIN32
    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(2, 0);
    if(::WSAStartup(wVersionRequested, &wsaData) != 0)
        return false;
    #endif


    if(gethostname(szBuffer, sizeof(szBuffer)) == SOCKET_ERROR)
    {
      #ifdef WIN32
      WSACleanup();
      #endif
      return false;
    }

    struct hostent *host = gethostbyname(szBuffer);
    if(host == NULL)
    {
      #ifdef WIN32
      WSACleanup();
      #endif
      return false;
    }

    //Obtain the computer's IP
    myIP.b1 = ((struct in_addr *)(host->h_addr))->S_un.S_un_b.s_b1;
    myIP.b2 = ((struct in_addr *)(host->h_addr))->S_un.S_un_b.s_b2;
    myIP.b3 = ((struct in_addr *)(host->h_addr))->S_un.S_un_b.s_b3;
    myIP.b4 = ((struct in_addr *)(host->h_addr))->S_un.S_un_b.s_b4;

    #ifdef WIN32
    WSACleanup();
    #endif
    return true;
}

bool obtainMyIP(char* myIP)
{
	bool ok = true;
	IPv4 ip;

	ok = obtainMyIP(ip); 
	if (ok)
	{
		sprintf_s(myIP, 15, "%d.%d.%d.%d", ip.b1, ip.b2, ip.b3, ip.b4);
	}
	else
	{
		sprintf_s(myIP, 4, "myIP");
	}

	return ok;
}

bool obtainMyPublicIP(char* myIP)
{
	bool ok = true;
	
	get_Website("api.ipify.org");
    for (size_t i = 0; i<website_HTML.length(); ++i) website_HTML[i] = tolower(website_HTML[i], local);

    std::istringstream ss(website_HTML);
    std::string stoken;

    while (std::getline(ss, stoken, '\n')) 
	{
        strcpy_s(lineBuffer[lineIndex], stoken.c_str());
        int dot = 0;
        for (unsigned int ii = 0; ii < strlen(lineBuffer[lineIndex]); ii++)
		{

            if (lineBuffer[lineIndex][ii] == '.') dot++;
            if (dot >= 3)
			{
                dot = 0;
                strcpy_s(ip_address, lineBuffer[lineIndex]);
				sprintf_s(myIP, 16, "%s", ip_address);
            }
        }

        lineIndex++;
    }

	return ok;
}

void get_Website(std::string url)
{
    WSADATA wsaData;
    SOCKET Socket;
    SOCKADDR_IN SockAddr;
    int lineCount = 0;
    int rowCount = 0;
    struct hostent *host;
    std::string get_http;


    get_http = "GET / HTTP/1.1\r\nHost: " + url + "\r\nConnection: close\r\n\r\n";

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){
        system("pause");
        //return 1;
    }

    Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    host = gethostbyname(url.c_str());

    SockAddr.sin_port = htons(80);
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);

    if (connect(Socket, (SOCKADDR*)(&SockAddr), sizeof(SockAddr)) != 0){
        system("pause");
        //return 1;
    }
    send(Socket, get_http.c_str(), strlen(get_http.c_str()), 0);

    int nDataLength;
    while ((nDataLength = recv(Socket, buffer, 10000, 0)) > 0){
        int i = 0;
        while (buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r'){

            website_HTML += buffer[i];
            i += 1;
        }
    }

    closesocket(Socket);
    WSACleanup();

}