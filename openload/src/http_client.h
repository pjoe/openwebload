#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include "sock.h"
#include "event_loop.h"

#define CRLF "\x00d\x00a"

typedef enum
{
    METHOD_GET = 0,
    METHOD_POST = 1,
    METHOD_HEAD = 2,
    METHOD_PUT = 3,
    METHOD_DELETE = 4
} eHttpMethod;

class CHttpHeader
{
public:
    char* name;
    char* value;

    CHttpHeader();
    void Set(const char* n, const char* v);
    virtual ~CHttpHeader();

    CHttpHeader* pNext;
    CHttpHeader* pPrev;
};

class CHttpHeaderList
{
public:
    CHttpHeader* pFirst;
    CHttpHeader* pLast;

    CHttpHeaderList();
    virtual ~CHttpHeaderList();

    void Insert(CHttpHeader* pNew);
    void Add(const char* name, const char* value);
    void Add(const char* line);
    CHttpHeader* Find(const char* name);
    const char* FindValue(const char* name);
    void Delete(const char* name);

    void Dump();
};

class CUrl
{
public:
    char* host;
    char* path;
    SOCKADDR_IN addr;

    CUrl();
    virtual ~CUrl();
    int parse(const char* url);
    CUrl& operator=(const CUrl& r);
};

class CHttpRequest
{
public:
    CHttpRequest();
    virtual ~CHttpRequest();

    eHttpMethod m_Method;
    CUrl m_Url;
    CHttpHeaderList m_Headers;
    char* m_Body;
    int m_Len;
};

class CHttpResponse
{
public:
    CHttpResponse();
    virtual ~CHttpResponse();

    int m_Status;
    CHttpHeaderList m_Headers;
    char* m_Body;
    int m_Len;
};

class CHttpContext;

typedef void TResponseFunc(CHttpContext* pContext);

class CHttpContext
{
public:
    CTcpSock* m_pSock;
    CHttpRequest* m_pReq;
    CHttpResponse* m_pResp;
    CHttpHeader* m_pHeader;
    CEventLoop* m_pEvLoop;
    TResponseFunc* m_pCallBack;
    void* m_pParam;
};



int SendRequest(CHttpRequest* pReq, CEventLoop* pEvLoop, TResponseFunc* pCallBack, void* pParam);

#endif // HTTP_CLIENT_H

