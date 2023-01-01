#include "utility.h"

#define error(msg) \
    do {perror(msg); exit(EXIT_FAILURE); } while (0)

//clients_list save all the clients's socket
map<int , string > clients_list ; 
map<string , int > name_list ; 

int sendBroadcastmessage(int clientfd , char* message ){
      
    map<int , string >::iterator it;
    for (it = clients_list.begin(); it != clients_list.end(); ++it) {
        if (it->first != clientfd) {
            if (send(it->first, message, BUF_SIZE, 0) < 0) {
                perror("error");
                exit(-1);
            }
        }
    }
    return 1 ; 
}

int send_to_one(int clientfd , char* message){
    
    if (send(clientfd, message, BUF_SIZE, 0) < 0) {
            perror("error");
            exit(-1);
    }

    return 1 ; 
}

//发送广播
int sendmessage(int clientfd) {
    char message[BUF_SIZE];
    char buf[BUF_SIZE]; 
    bzero(buf, BUF_SIZE);
    bzero(message, BUF_SIZE);

    printf("read from client(clientID = %d) \n", clientfd );
    int len = recv(clientfd, buf, BUF_SIZE, 0);
    int result = -1 ; 

    if (0 == len) {
       
        printf("ClientID = %d closed.\n now there are %d client in the char room\n",
        clientfd, (int)clients_list.size() - 1 );
        sprintf(message, SERVER_EXIT, const_cast<char *>(clients_list[clientfd].c_str()) , (int) clients_list.size() - 1);
        result = sendBroadcastmessage(0 , message) ; 
        close(clientfd);
        clients_list.erase(clientfd);
        
    } else {
        if (1 == clients_list.size()) {
            send(clientfd, CAUTION, strlen(CAUTION), 0);
            return 0;
        }

        if(buf[0] == '/'){ // 个人
            string name = "" ; 
            int i = 1 , j = 0; 
            while(buf[i] != ' '){
                name += buf[i++] ; 
            }
            char mes[BUF_SIZE] ; 
            bzero(mes, BUF_SIZE);

            while(buf[i] != '\0'){
                mes[j++] = buf[i++] ; 
            }

            sprintf(message, SERVER_MESSAGE_ONE, const_cast<char *>(clients_list[clientfd].c_str()) , mes);
            result = send_to_one(name_list[name] , message ) ; 

        }else { // 群发
            sprintf(message, SERVER_MESSAGE, const_cast<char *>(clients_list[clientfd].c_str()) , buf);
            result = sendBroadcastmessage(clientfd , message) ; 
        }
    }
    return result ;
}

int main(int argc, char *argv[]) {
    /**
     * TCP的服务器端socket基本流程socket->bind->listen->accept->send/recv->closesocket
     * TCP服务端通信
     * 1：使用 socket()创建 TCP 套接字（socket）
     * 2：将创建的套接字绑定到一个本地地址和端口上（bind）
     * 3：将套接字设为监听模式，准备接收客户端请求（listen）
     * 4：等待客户请求到来: 当请求到来后，接受连接请求，返回一个对应于此次连接的新的套接字（accept）
     * 5：用 accept 返回的套接字和客户端进行通信（使用write()/send()或send()/recv())
     * 6：返回，等待另一个客户请求
     * 7：关闭套接字
     */

    /**
     * 1:创建套接字socket
     * param1:指定地址族为IPv4;param2:指定传输协议为流式套接字;param3:指定传输协议为TCP,可设为0,由系统推导
     */
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener < 0) {
        error("socket error");
    }
    printf("listen socket created \n");

    //地址复用
    int on = 1;
    /**
     * int setsockopt( int s, int level, int option_name,const void *option_value, size_t ，ption_len);
     * setsockopt()用来设置参数s 所指定的 socket 状态. 参数 level 代表欲设置的网络层, 一般设成 SOL_SOCKET 以存取socket 层 ; 参数optname 代表欲设置的选项,
     * SO_REUSEADDR 允许在bind() 过程中本地地址可重复使用
     * 
     SO_REUSEADDR可以用在以下四种情况下。 (摘自《Unix网络编程》卷一，即UNPv1)
        1、当有一个有相同本地地址和端口的socket1处于TIME_WAIT状态时，而你启动的程序的socket2要占用该地址和端口，你的程序就要用到该选项。
        2、SO_REUSEADDR允许同一port上启动同一服务器的多个实例(多个进程)。但每个实例绑定的IP地址是不能相同的。在有多块网卡或用IP Alias技术的机器可以测试这种情况。
        3、SO_REUSEADDR允许单个进程绑定相同的端口到多个socket上，但每个socket绑定的ip地址不同。这和2很相似，区别请看UNPv1。
        4、SO_REUSEADDR允许完全相同的地址和端口的重复绑定。但这只用于UDP的多播，不用于TCP。
    */
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        error("setsockopt");
    }

    /*
    struct sockaddr_in{
        sa_family_t sin_family; //地址族（Address Family），也就是地址类型
        uint16_t sin_port;      //16位的端口号
        struct in_addr sin_addr; //32位IP地址
        char sin_zero[8];       //不使用，一般用0填充
    };
    */
    // 填充sockaddr结构,指定ip与端口
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    /*
    * 服务器端要用 bind() 函数将套接字与特定的IP地址和端口绑定起来，只有这样，流经该IP地址和端口的数据才能交给套接字处理；
    */
    if (bind(listener, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
        error("bind error");
    }

    //监听
    if (listen(listener, SOMAXCONN) < 0) {
        error("listen error");
    }

    printf("Start to listen: %s\n", SERVER_IP);

    //在内核中创建事件表 , 建议内核需要监听 size 个 fd , epfd 红黑树树根
    int epfd = epoll_create(EPOLL_SIZE);
    if (epfd < 0) {
        error("epfd error");
    }
    printf("epoll created, epollfd = %d\n\n", epfd);

    /*
    typedef union epoll_data {
        void        *ptr;
        int          fd;
        uint32_t     u32;
        uint64_t     u64;
    } epoll_data_t;

    struct epoll_event {
        uint32_t     events;      // Epoll events 
        epoll_data_t data;        // User data variable 
    };
    */
    static struct epoll_event events[EPOLL_SIZE];
    //往内核事件表里添加事件
    addfd(epfd, listener, true); 
   
    //主循环
    while (1) {
        //epoll_events_count表示就绪事件的数目
        int epoll_events_count = epoll_wait(epfd, events, EPOLL_SIZE, -1);
        // 可以把epoll_wait得到的描述符用多线程处理
        if (epoll_events_count < 0) {
            perror("epoll failure");
            break;
        }

        printf("epoll_events_count = %d\n", epoll_events_count);
        //处理这epoll_events_count个就绪事件
        for (int i = 0; i < epoll_events_count; ++i) {
            int sockfd = events[i].data.fd;
            //新用户连接
            if (sockfd == listener) {
                struct sockaddr_in client_address;
                socklen_t client_addrLength = sizeof(struct sockaddr_in);
                
                int clientfd = accept(listener, (struct sockaddr *) &client_address, &client_addrLength);

                printf("client connection from: %s : % d(IP : port), clientfd = %d \n",
                       inet_ntoa(client_address.sin_addr), // 将网络地址转换成“.”点隔的字符串格式
                       ntohs(client_address.sin_port), // 
                       clientfd);
                sleep(1) ; // 第一次客户端链接之后，客服端会发送名字，服务器阻塞 1 秒，不要注册 clientfd
                // 可以通过判断 map 是否存在，来判断是不是第一次传输数据过来，如果是第一次传输数据，那么一定会是 name ，然后再发送欢迎消息即可，这样就不用阻塞 1 秒
                addfd(epfd, clientfd, true);

                 

                // 服务端发送欢迎信息
                printf("welcome chatting\n");
                char message[BUF_SIZE];
                bzero(message, BUF_SIZE);
                sprintf(message, SERVER_WELCOME );
                int ret = send(clientfd, message, BUF_SIZE, 0);

                if (ret < 0) {
                    error("send error");
                }

            } else {     
                //处理用户发来的消息，并广播，使其他用户收到信息
                // 这个可以再加一个多线程处理
                int ret = sendmessage(sockfd);
                if (ret < 0) {
                    error("error");
                }
            }
        }
    }
    close(listener); //关闭socket
    close(epfd);    //关闭内核
    return 0;
}