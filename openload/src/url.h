#ifndef URL_H
#define URL_H

#include "sock.h"

class CUrl
{
public:
    char* host;
    int port;
    char* path;
    SOCKADDR_IN addr;

    CUrl();
    virtual ~CUrl();
    int parse(const char* url);
    CUrl& operator=(const CUrl& r);
};



#endif // URL_H

