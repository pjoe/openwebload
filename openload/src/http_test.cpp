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

// Request modes
//  NORMAL - normal measurement run
//  TEST - test run which displays the response
enum EReqModes
{
    NORMAL = 0,
    TEST
};

typedef struct TReqParams
{
    long startTime;
    EReqModes mode;
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
    // Get request parameters
    TReqParams* pReqParams = (TReqParams*) pContext->m_pParam;

    // For TEST mode show body and stop
    if(pReqParams->mode == TEST)
    {
        // Show response
        printf("------------------------\n");
        printf("Status: %d\n", pContext->m_pResp->m_Status);
        printf("---- Headers -----------\n");
        pContext->m_pResp->m_Headers.Dump();
        printf("---- Body %5d bytes --\n", pContext->m_pResp->m_Len);
        fwrite(pContext->m_pResp->m_Body, 1, pContext->m_pResp->m_Len, stdout);
        printf("---- End ---------------\n");
        return;
    }

    // Get response time (duration) of this request
    long endTime = getMsTime();
    long duration = endTime - pReqParams->startTime;

    // Check if we got a HTTP 200 OK response 
    if(pContext->m_pResp->m_Status != 200)
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

        // Calculate moving average of TPS
        if(g_maTps < 0.001f)
            g_maTps = tps;
        else
            g_maTps += (tps - g_maTps) * 0.1f;
    
        printf("MaTps %6.2f, Tps %6.2f, Resp Time %6.3f, Err %3ld%%, "
           "Count %5ld\n",
           g_maTps, tps, respTime, g_errorCount * 100 / g_count,
           g_totalCount);

        g_lastReportTime = endTime;
        g_count = 0;
        g_errorCount = 0;
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
    char* addr = NULL;
    int clients = 5;
    int i;
    EReqModes mode = NORMAL;

    // parse arguments
    char* arg;
    for(i = 1; i < argc; i++)
    {
        arg = argv[i];
        if(arg[0] == '-')
        {
            switch(arg[1])
            {
                case 't':
                    clients = 1;
                    mode = TEST;
                    break;
                default:
                    // unknown option
                    printf("Error: unknown option %s\n", arg);
                    return 1;
                    break;
            }
        }
        else
        {
            if(addr == NULL)
                addr = arg;
            else
                clients = atoi(arg);
        }
    }

    // Sanity check args
    if(addr == NULL)
    {
        printf("Error: no url specified\n");
        return 1;
    }
    if(clients < 1)
    {
        printf("Error: must have at least 1 client\n");
        return 1;
    }

    url.parse(addr);
    req.m_Url = url;

    CEventLoop evLoop;

    printf("URL: http://%s:%d%s\n", url.host, url.port, url.path);
    printf("Clients: %d\n", clients);

    g_startTime = getMsTime();
    g_lastReportTime = g_startTime;

    TReqParams* pReqParams = new TReqParams[clients];
    for(i = 0; i < clients; i++)
    {
        pReqParams[i].startTime = getMsTime();
        pReqParams[i].mode = mode;
        SendRequest(&req, &evLoop, ResponseFunc, &pReqParams[i]);
    }

    evLoop.run();

    long now = getMsTime();

    float tps = g_totalCount /((now - g_startTime) / 1000.0f);
    float respTime = 0;
    if(g_totalCount > 0)
	respTime = g_totalDuration / 1000.0f / g_totalCount;
    printf("Total TPS: %6.2f\n", tps);
    printf("Avg. Response time: %6.3f sec.\n", respTime);
    printf("Max Response time:  %6.3f sec\n", g_maxDuration / 1000.0f);


    delete [] pReqParams;

    return 0;
}
