#include "http_client.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32

long getMsTime(void)
{
	LARGE_INTEGER freq;
    LARGE_INTEGER now;

	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&now);

	now.QuadPart *= 1000;
	now.QuadPart /= freq.QuadPart;
    return (long) now.QuadPart;
}

#else // WIN32

long getMsTime(void)
{
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}


#endif // WIN32

typedef struct TReqParams
{
    long startTime;
} TReqParams;

long g_startTime;
long g_totalCount = 0;
long g_lastReportTime;
long g_count = 0;
float g_maTps = 0.0f;

void ResponseFunc(CHttpContext* pContext)
{
    static int i=0;
    i++;

    TReqParams* pReqParams = (TReqParams*) pContext->m_pParam;
    long endTime = getMsTime();
    long duration = endTime - pReqParams->startTime;
    g_count++;

    if(endTime - g_lastReportTime >= 1000)
    {
        g_totalCount += g_count;
        float tps = g_count / ((endTime - g_lastReportTime) / 1000.0f);
        if(g_maTps < 0.001f)
            g_maTps = tps;
        else
            g_maTps += (tps - g_maTps) * 0.1f;
    
        printf("MATPS %6.2f, TPS %6.2f, count %8ld\n", g_maTps, tps, g_totalCount);

        g_lastReportTime = endTime;
        g_count = 0;
    }


    //printf("Status: %d, time %7.3f sec\n", pContext->m_pResp->m_Status, duration / 1000.0f);
    //pContext->m_pResp->m_Headers.Dump();
    //fwrite(pContext->m_pResp->m_Body, 1, pContext->m_pResp->m_Len, stdout);
    //if(i<8)
    {
        pReqParams->startTime = getMsTime();
        SendRequest(pContext->m_pReq, pContext->m_pEvLoop, ResponseFunc, pReqParams);
    }
}

int main(int argc, char* argv[])
{
    CHttpRequest req;
    CUrl url;
    char* addr = "http://192.168.0.11/cgi-bin/test.pl";
    if(argc > 1)
	addr = argv[1];
    url.parse(addr);
    req.m_Url = url;

    CEventLoop evLoop;


    int clients = 5;
    if(argc > 2)
	clients = atoi(argv[2]);
    int i;

    printf("URL: http://%s%s\n", url.host, url.path);
    printf("Clients: %d\n", clients);

    g_startTime = getMsTime();
    g_lastReportTime = g_startTime;

    TReqParams* pReqParams = new TReqParams[clients];
    for(i = 0; i < clients; i++)
    {
        pReqParams[i].startTime = getMsTime();
        SendRequest(&req, &evLoop, ResponseFunc, &pReqParams[i]);
    }

    evLoop.run();

    long now = getMsTime();

    float tps = g_totalCount /((now - g_startTime) / 1000.0f);
    printf("Total TPS: %6.2f\n", tps);


    delete [] pReqParams;

    return 0;
}
