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

#ifndef WIN32

extern int stricmp(const char* s1, const char* s2);
extern int strnicmp(const char* s1, const char* s2, int n);

#endif // WIN32

#endif // URL_H

