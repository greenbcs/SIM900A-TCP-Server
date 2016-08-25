/*
 *
 * *\ 服务器端的源代码
 *
 * */
#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/epoll.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <strings.h>
#include <iostream>
using namespace std;
#define MAXFDS 256
#define EVENTS 100
#define PORT 9999

int epfd;
bool setNonBlock(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    if(-1 == fcntl(fd, F_SETFL, flags))
        return false;
    return true;
}

int main()
{
    int sockfd, nfds, confd, cfd, ret, sig;
    int on = 1;
    char buffer[512],stra[6],strb[6];
    struct sockaddr_in saddr, caddr;
    socklen_t socklen = sizeof(struct sockaddr_in);
    struct epoll_event ev, events[EVENTS];

    sockfd=socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == sockfd)
    {
        /*printf("创建套接字出错啦");*/
	/*::cout << "创建套接字出错啦" << std::endl;*/
        return -1;
    }

    setNonBlock(sockfd);

    epfd = epoll_create(MAXFDS);

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons((short)(PORT));
    saddr.sin_addr.s_addr = INADDR_ANY;
    if(-1 == bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr)))
    {
        /*printf("套接字不能绑定到服务器上\n");*/
	/*std::cout << "套接字不能绑定到服务器上" << std::endl;*/
        return -1;
    }

    if(-1 == listen(sockfd, 32))
    {
        /*printf("监听套接字的时候出错了\n");*/
	/*std::cout << "监听套接字的时候出错了" << std::endl;*/
        return -1;
    }

    ev.data.fd = sockfd;
    ev.events = EPOLLIN | EPOLLET ;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);
    
    /*std::cout << sockfd << std::endl;    */

    while(true)
    {
        nfds = epoll_wait(epfd, events, MAXFDS, -1);

        for(int i = 0; i < nfds; ++ i)
        {
            if(sockfd == events[i].data.fd)
            {
                memset(&caddr, 0, sizeof(caddr));
		cfd = accept(sockfd, (struct sockaddr *)&caddr, &socklen);
                if(-1 == cfd)
                {
		    /*printf("服务器接收套接字的时候出问题了\n");*/
                    /*std::cout << "服务器接收套接字的时候出问题了" << std::endl;*/
                    break;
                }

                setNonBlock(cfd);

                ev.data.fd = cfd;
                ev.events = EPOLLIN | EPOLLET;
                epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
		/*printf("有连接请求，将其交给socket描述符:%s\n",cfd);
	        printf("户端地址:%s, 端口:%s.\n",inet_ntoa(caddr.sin_addr),caddr.sin_port);*/
		std::cout << "有连接请求，将其交给socket描述符" << cfd << std::endl;
                std::cout << "客户端地址" << inet_ntoa(caddr.sin_addr) << "客户端端口" << caddr.sin_port << std::endl;
            }
            /*else if(events[i].data.fd & EPOLLIN)*/
	    else
            {
                bzero(&buffer, sizeof(buffer));
	        /*setNonBlock(events[i].data.fd);*/
                ret = recv(events[i].data.fd, buffer, sizeof(buffer), 0);
                /*if(ret < 0)
                {
                    std::cout << "服务器收到的消息出错了,ERROR ID:" << ret << std::endl;
                    return -1;
                }*/
		if (ret > 0)
		{
                std::cout << "事件编号" << i << "事件对应的socket描述符" << events[i].data.fd << std::endl;
                std::cout << "服务器端要读取客户端发过来的消息" << std::endl;
                std::cout << "字符串长度" << ret << "接收到的消息为：" << (char *) buffer << std::endl;
                strncpy(stra, buffer + 0, 6);
		strncpy(strb, buffer + 6, ret-7);
		std::cout << "字符串a:" << stra << "  字符串b:" << strb << std::endl;
		/*printf("字符串a:%s, 字符串b:%s\n",stra,strb);*/
		/*ev.data.fd = events[i].data.fd-1;
                ev.events = EPOLLOUT | EPOLLET;
                epoll_ctl(epfd, EPOLL_CTL_MOD, ev.data.fd, &ev);*/
		}
		
            }
            /*else if(events[i].data.fd & EPOLLOUT)
            {
                bzero(&buffer, sizeof(buffer));
                bcopy("Send data OK!", buffer, sizeof("Send data OK!"));
                /*setNonBlock(events[i].data.fd);*/
                /*ret = send(events[i].data.fd, buffer, sizeof(buffer), 0);
                if(ret < 0)
                {
                    std::cout << "服务器发送消息给客户端的时候出错啦" << std::endl;
                    return -1;
                }
                std::cout << "事件编号" << i << "socket描述符" << events[i].data.fd << "发送的消息OK" << std::endl;
		ev.data.fd = events[i].data.fd;
		epoll_ctl(epfd, EPOLL_CTL_DEL, ev.data.fd, &ev);
            }*/
        }
    }
    if(sockfd > 0)
    {
        shutdown(sockfd, SHUT_RDWR);
        close(sockfd);
    }
}
