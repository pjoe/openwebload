#include "url.h"


#ifndef WIN32

extern int stricmp(const char* s1, const char* s2);
extern int strnicmp(const char* s1, const char* s2, int n);

#endif // WIN32


// ---------------- CUrl ----------------

CUrl::CUrl()
{
    host = NULL;
    port = 80;
    path = NULL;
}

CUrl::~CUrl()
{
    delete [] host;
    delete [] path;
}

int CUrl::parse(const char* url)
{
    const char* p = url;
    const char* p2;
    delete [] host;
    delete [] path;
    if(strnicmp(url, "http://", 7) == 0)
        p += 7;

    p2 = strchr(p, '/');
    if(p2)
    {
        host = new char[p2 - p + 1];
        strncpy(host, p, p2 - p);
        host[p2-p] = 0;

        path = new char[strlen(p2) + 1];
        strcpy(path, p2);
    }
    else
    {
        host = new char[strlen(p) + 1];
        strcpy(host, p);

        path = new char[2];
        strcpy(path, "/");
    }

    return get_address(host, port, &addr);
}

CUrl& CUrl::operator=(const CUrl& r)
{
    delete [] host;
    delete [] path;
    host = new char[strlen(r.host) + 1];
    strcpy(host, r.host);
    port = r.port;
    path = new char[strlen(r.path) + 1];
    strcpy(path, r.path);
    addr = r.addr;
    return *this;
}

