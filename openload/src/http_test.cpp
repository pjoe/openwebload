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
long g_errorCount = 0;
long g_duration = 0;
long g_totalDuration = 0;
long g_maxDuration = 0;
float g_maTps = 0.0f;

void ResponseFunc(CHttpContext* pContext)
{
    // get response time (duration) of this request
    TReqParams* pReqParams = (TReqParams*) pContext->m_pParam;
    long endTime = getMsTime();
    long duration = endTime - pReqParams->startTime;

    // Check if we got a HTTP 200 OK response 
    if(pContext->m_pResp->status != 200)
    {
	g_errorCount++;
    }

    // Update counters
    g_count++;
    g_duration += duration;
    if(duration > g_maxDuration)
    {
	g_maxDuration = duration;
    }

    // if it's more than a second since last report, do a new one
    if(endTime - g_lastReportTime >= 1000)
    {
        g_totalCount += g_count;
	g_totalDuration += g_duration;
        float tps = g_count / ((endTime - g_lastReportTime) / 1000.0f);
	float respTime = g_duration / 1000.0f / g_count;
        if(g_maTps < 0.001f)
            g_maTps = tps;
        else
            g_maTps += (tps - g_maTps) * 0.1f;
    
        printf("MATPS %6.2f, TPS %6.2f, RESP TIME %6.3f, count %5ld\n",
	       g_maTps, tps, respTime, g_totalCount);

        g_lastReportTime = endTime;
        g_count = 0;
	g_duration = 0;
    }

    // Send new request
    pReqParams->startTime = getMsTime();
    SendRequest(pContext->m_pReq, pContext->m_pEvLoop, ResponseFunc, pReqParams);
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
    float respTime = g_totalDuration / 1000.0f / g_totalCount;
    printf("Total TPS: %6.2f\n", tps);
    printf("Avg. Response time: %6.3f sec.\n", respTime);
    printf("Max Response time:  %6.3f sec\n", g_maxDuration / 1000.0f);


    delete [] pReqParams;

    return 0;
}
