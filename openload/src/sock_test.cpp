#include "sock.h"
#include "event_loop.h"

#include <stdio.h>

#define CRLF "\x00d\x00a"

void ConnectOk(CTcpSock* sock)
{
    printf("Sending request\n");
    sock->sendString("HEAD / HTTP/1.0" CRLF CRLF);
}

void SendOk(CTcpSock* sock)
{
    printf("Reading response\n");
    //sock->readLine((char *) sock->m_context, 4000);
    sock->recvBuf((char *) sock->m_context, 2000);
}

void ReadLineOk(CTcpSock* sock)
{
    char* line = (char*) sock->m_context;
    //printf("%s\n", line);
    if(!sock->m_eof)
    {
        sock->readLine(line, 4000);
    }
    else
    {
        printf("DONE reading\n");
    }
    
}

void RecvBufOk(CTcpSock* sock)
{
    printf("Received %d bytes\n", sock->m_read);
    fwrite(sock->m_context, 1, sock->m_read, stdout);
    printf("\n");
}

int main(int argc, char* argv[])
{
    char line_buf[4000];
    char line_buf2[4000];
    CTcpSock sock;
    CTcpSock sock2;
    SOCKADDR_IN addr;
    printf("starting\n");
    char* szAddr = "192.168.0.11";
    if(argc > 1)
	szAddr = argv[1];
    get_address(szAddr, 80, &addr);
    printf("the address is: %s\n", inet_ntoa(addr.sin_addr));

    sock.create();
    sock.m_context = line_buf;
    sock.m_cbConnectOk = ConnectOk;
    sock.m_cbSendOk = SendOk;
    sock.m_cbReadLineOk = ReadLineOk;
    sock.m_cbRecvBufOk = RecvBufOk;

    sock2.create();
    sock2.m_context = line_buf2;
    sock2.m_cbConnectOk = ConnectOk;
    sock2.m_cbSendOk = SendOk;
    sock2.m_cbReadLineOk = ReadLineOk;
    sock2.m_cbRecvBufOk = RecvBufOk;


    sock.connect(&addr);
    sock2.connect(&addr);

    CEventLoop evLoop;
    evLoop.addSock(&sock);
    evLoop.addSock(&sock2);

    evLoop.run();

    return 0;
}

