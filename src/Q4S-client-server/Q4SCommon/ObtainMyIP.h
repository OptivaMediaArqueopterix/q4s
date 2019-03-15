#ifndef _OBTAIN_MY_IP_H_
#define _OBTAIN_MY_IP_H_

struct IPv4
{
    unsigned char b1, b2, b3, b4;
};

bool obtainMyIP(IPv4 & myIP);
bool obtainMyIP(char* myIP);
bool obtainMyPublicIP(char* myIP);

#endif //_OBTAIN_MY_IP_H_