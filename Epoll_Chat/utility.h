#ifndef CHAT_UTILITY_H // 先测试 CHAT_UTILITY_H 是否被宏定义过了
#define CHAT_UTILITY_H // 没有则定义

#include <iostream>
#include <map>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

/***** macro defintion *****/
//server ip
#define SERVER_IP "127.0.0.1"

//server port
#define SERVER_PORT 8888

//epoll size
#define EPOLL_SIZE 5000

//message buffer size
#define BUF_SIZE 0xFFFF

#define SERVER_WELCOME "Welcome you join to the chat room!\n You name : %s \n type \"/name msg\" to chat \n type \"msg\" to broadcast \n\n"

#define SERVER_MESSAGE "Clientname %s say >> %s"

#define SERVER_EXIT "Clientname = %s exited.\nnow there are %d client in the char room\n"

#define SERVER_COME "Clientname = %s join our char room !\n"

#define SERVER_MESSAGE_ONE "Clientname = %s say to you >> %s "
//exit
#define EXIT "EXIT"

//warning
#define CAUTION "There is only you int the char root!"


/****** some function *****/
/**
 *设置非阻塞 , 边缘触发时，可以非阻塞轮询读，防止缓冲区数据没读完。
 */
int setnonblockint(int sockfd) {
    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK);
    return 0;
}

/**
 * 将文件描述符 fd 添加到 epollfd 标示的内核事件表中,
 * 并注册 EPOLLIN 和 EPOOLET 事件,
 * EPOLLIN 是数据可读事件；EPOOLET 表明是 ET 工作方式。
 * 最后将文件描述符设置非阻塞方式
 * @param epollfd:epoll句柄
 * @param fd:文件描述符
 * @param enable_et:enable_et = true,
 * 是否采用epoll的ET(边缘触发)工作方式；否则采用LT(水平触发)工作方式
 */
void addfd(int epollfd, int fd, bool enable_et) {
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN;
    if (enable_et) {
        ev.events = EPOLLIN | EPOLLET;
    }
    /*
    将 fd 设置成非阻塞，防止出现 read 阻塞，导致无法调用 epoll_wait ，进而导致无法响应客户端请求，最终死锁
    */
    setnonblockint(fd); 
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
    //printf("fd added to epoll!\n\n");
}
#endif //CHAT_UTILITY_H