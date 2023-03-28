#include"socket.h"
void errexit(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	WSACleanup();
	exit(1);
}

SOCKET connectsock(const char* host, const char* service, const char* transport)
{
    struct hostent* phe; /*pointer to host information entry*/
    struct servent* pse;/*pointer to service information entry*/
    struct protoent* ppe;/*pointer to protocol information entry*/
    struct sockaddr_in sin;/*an Internet endpoint address*/
    int s, type;        /*socket descriptor and socket type*/

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;

    //设置协议类型
    if ((ppe = getprotobyname(transport)) == 0) 
    {
        std::cerr << "Can't get " << service << "entry\n" << std::endl;
        WSACleanup();
        return INVALID_SOCKET;
    }
    //设置套接字类型
    if (strcmp(transport, "udp") == 0) {
        type = SOCK_DGRAM;
    }
    else {
        type = SOCK_STREAM;
    }
    //将主机名翻译成IP地址
    if ((phe = gethostbyname(host)) == 0)
    {
        std::cerr << "Can't get " << host << " host entry\n" << std::endl;
        WSACleanup();
        return INVALID_SOCKET;
    }
    //将服务名翻译成端口号
    if ((pse = getservbyname(service, transport)) == 0)
    {
        std::cerr << "Can't get "<<transport<< " service entry\n" << std::endl;
        WSACleanup();
        return INVALID_SOCKET;
    }
    memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
    sin.sin_port = pse->s_port;
    //分配一个socket
    s = socket(PF_INET, type, ppe->p_proto);
    if (s == INVALID_SOCKET) {
        std::cerr << "Can't allocate a socket" << std::endl;
        WSACleanup();
        return INVALID_SOCKET;
    }
    //连接socket
    if (connect(s, (sockaddr*)&sin, sizeof(sin)) == SOCKET_ERROR){
        std::cerr << "Can't connect to " << host << " " << service << std::endl;
        closesocket(s);
        WSACleanup();
        return INVALID_SOCKET;
    }
    return s;
}

SOCKET connectudp(const char* host, const char* service)
{
    return connectsock(host, service, "udp");
}

SOCKET connecttcp(const char* host, const char* service)
{
    return connectsock(host, service, "tcp");
}
