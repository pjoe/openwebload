#ifndef SOCK_H_INCLUDED
#define SOCK_H_INCLUDED

#ifdef WIN32

#include <winsock.h>

#else // WIN32

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct sockaddr_in SOCKADDR_IN;
typedef int SOCKET;

#endif // WIN32

class CTcpSock;

typedef void TCallBack(CTcpSock* sock);

int get_address(const char* host, int port, SOCKADDR_IN* pAdr);
int getSocketError();


class CTcpSock
{
public:
    CTcpSock(void);
    ~CTcpSock(void);
    int create(void);
    int close(void);
    int connect(SOCKADDR_IN* name);
    int send(const char* buf, int len);
    int sendString(const char* str);
    int recvBuf(char* buf, int len);
    int readLine(char* buf, int max_len);
    int setFdSets(fd_set* rfds, fd_set* wfds, fd_set* efds);
    int checkFdSets(fd_set* rfds, fd_set* wfds, fd_set* efds);
    void deleteSock(void);

    SOCKET m_sock;
    void* m_context;
    TCallBack* m_cbConnectOk;
    TCallBack* m_cbSendOk;
    TCallBack* m_cbReadLineOk;
    TCallBack* m_cbRecvBufOk;
    const char* m_wbuf;
    int m_wlen;
    int m_sent;
    char* m_rbuf;
    int m_rlen;
    int m_read;
    int m_eof;
protected:
    enum ESockStates
    {
        start,
        created,
        connecting,
        ready,
        sending,
        recievingBuf,
        readingLine,
        closed
    } m_state;
    enum EEvents
    {
        read   = 1,
        write  = 2,
        except = 4
    };
    enum ESockInternalEvents
    {
        noEvent,
        connectOk,
        sendOk,
        readLineOk,
        recvBufOk,
        deleteEvent
    };

    int processEvent(int event);
    void readLineLoop(void);
    void recvBufLoop(void);

    int m_intStatus;
};

#endif // SOCK_H_INCLUDED
