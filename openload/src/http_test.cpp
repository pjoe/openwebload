#include "http_client.h"
#include "verifier.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    RM_NORMAL = 0,
    RM_TEST
};

// Output modes
//  NORMAL - human readable
//  CSV - prints a single line in csv format, ideal for importing in spreadsheets
enum EOutputModes
{
    OM_NORMAL = 0,
    OM_CSV
};

typedef struct TReqParams
{
    long startTime;
    EReqModes mode;
    int clientId;
    int sequenceNumber;
} TReqParams;

long g_timeLimit = 0;
long g_startTime;
long g_totalCount = 0;
long g_lastReportTime;
long g_count = 0;
long g_errorCount = 0;
long g_duration = 0;
long g_totalDuration = 0;
long g_maxDuration = 0;
float g_maTps = 0.0f;

int g_fVerify = 0;
CVerifier g_verifier;
char g_originalPath[1025];

void ResponseFunc(CHttpContext* pContext)
{
    // Get request parameters
    TReqParams* pReqParams = (TReqParams*) pContext->m_pParam;

    // For TEST mode show body and stop
    if(pReqParams->mode == RM_TEST)
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
    else
    {
        // verify if needed
        if(g_fVerify)
        {
            char magic[30];
            sprintf(magic, "%d-%d", pReqParams->clientId, pReqParams->sequenceNumber);
            if(!g_verifier.Verify(pContext->m_pResp->m_Body, magic))
                g_errorCount++;

            // increment sequence number
            pReqParams->sequenceNumber++;
        }
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
    
        fprintf(stderr, "MaTps %6.2f, Tps %6.2f, Resp Time %6.3f, Err %3ld%%, "
           "Count %5ld\n",
           g_maTps, tps, respTime, g_errorCount * 100 / g_count,
           g_totalCount);

        g_lastReportTime = endTime;
        g_count = 0;
        g_errorCount = 0;
        g_duration = 0;
    }

    // check if we have reached the time limit
    if(g_timeLimit != 0 && endTime - g_startTime > g_timeLimit)
    {
        pContext->m_pEvLoop->stop();
    }

    // if verifying is enabled, update the url
    if(g_fVerify)
    {
        char path[1050];
        sprintf(path, "%smagic=%d-%d", g_originalPath,
            pReqParams->clientId, pReqParams->sequenceNumber);
        pContext->m_pReq->m_Url.setPath(path);
    }


    // Send new request
    pReqParams->startTime = getMsTime();
    SendRequest(pContext->m_pReq, pContext->m_pEvLoop, ResponseFunc, pReqParams);
}

int main(int argc, char* argv[])
{
    CHttpHeaderList Headers;
    CHttpRequest* reqs;
    CUrl url;
    char* addr = NULL;
    int clients = 5;
    int i;
    EReqModes rMode = RM_NORMAL;
    EOutputModes oMode = OM_NORMAL;

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
                    // test mode
                    clients = 1;
                    rMode = RM_TEST;
                    break;
                case 'h':
                    // add request header
                    i++;
                    if(i+1 < argc)
                        Headers.Add(argv[i], argv[i+1]);
                    i++;
                    break;
                case 'l':
                    // time limit
                    i++;
                    if(i < argc)
                        g_timeLimit = atoi(argv[i]);
                    break;
                case 'o':
                    // output mode
                    i++;
                    if(i < argc)
                    {
                        if(stricmp("csv", argv[i]) == 0)
                            oMode = OM_CSV;
                    }
                    break;
                case 'v':
                    // verifier
                    i++;
                    if(i < argc)
                    {
                        if(g_verifier.LoadTemplate(argv[i], "MAGIC") != 0)
                        {
                            printf("unable to load verifier template\n");
                        }
                        else
                        {
                            g_fVerify = 1;
                        }
                    }
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

    if(url.parse(addr))
    {
        printf("Error: DNS unable to find remote host\n");
        return 1;
    }

    strncpy(g_originalPath, url.path, 1024);

    CEventLoop evLoop;

    fprintf(stderr, "URL: http://%s:%d%s\n", url.host, url.port, url.path);
    fprintf(stderr, "Clients: %d\n", clients);
    if(g_fVerify)
    {
        fprintf(stderr, "Verifying enabled\n");
    }
    if(g_timeLimit > 0)
    {
        fprintf(stderr, "Time Limit: %d sec.\n", g_timeLimit);
    }
    // convert time limit to ms
    g_timeLimit *= 1000;

    g_startTime = getMsTime();
    g_lastReportTime = g_startTime;

    TReqParams* pReqParams = new TReqParams[clients];
    reqs = new CHttpRequest[clients];
    char path[1050];
    for(i = 0; i < clients; i++)
    {
        pReqParams[i].startTime = getMsTime();
        pReqParams[i].mode = rMode;
        pReqParams[i].clientId = i;
        pReqParams[i].sequenceNumber = 1;
        reqs[i].m_Url = url;
        sprintf(path, "%smagic=%d-%d", g_originalPath,
            pReqParams[i].clientId, pReqParams[i].sequenceNumber);
        reqs[i].m_Url.setPath(path);
        reqs[i].m_pHeaders = &Headers;

        SendRequest(&reqs[i], &evLoop, ResponseFunc, &pReqParams[i]);
    }

    evLoop.run();

    if(rMode == RM_NORMAL)
    {
        long now = getMsTime();

        float tps = g_totalCount /((now - g_startTime) / 1000.0f);
        float respTime = 0;
        if(g_totalCount > 0)
            respTime = g_totalDuration / 1000.0f / g_totalCount;
        if(oMode == OM_NORMAL)
        {
            printf("Total TPS: %6.2f\n", tps);
            printf("Avg. Response time: %6.3f sec.\n", respTime);
            printf("Max Response time:  %6.3f sec\n", g_maxDuration / 1000.0f);
        }
        if(oMode == OM_CSV)
        {
            printf("http://%s:%d%s,%d,%.2f,%.3f,%.3f,%d\n", url.host, url.port, url.path,
                clients, tps, respTime, g_maxDuration / 1000.0f, g_totalCount);
        }
    }


    delete [] reqs;
    delete [] pReqParams;

    return 0;
}
