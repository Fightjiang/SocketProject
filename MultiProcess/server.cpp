#include"utili.h"

void Process_Handler(int sockConn);

void Process_Handler(int sockConn){
    OperStruct op; 
    int result;
    while(1){
        //第四个参数为0的时候， 其实是剪切数据， 而不是copy数据
        int res = recv(sockConn, &op, sizeof(op), 0); 
        if(res == 0) break ; // 当返回值是 0 时，为正常关闭连接；
        if(res == -1){
            printf("recv data fail.\n");
            continue;
        }   
        if(op.oper == ADD){
            result = op.op1 + op.op2;
        }else if(op.oper == SUB){   
            result = op.op1 - op.op2;
        }else if(op.oper == MUL){
            result = op.op1 * op.op2;
        }else if(op.oper == DIV){
            result = op.op1 / op.op2;
        }else if(op.oper == QUIT){
            break;
        }   

        res = send(sockConn, &result, sizeof(result), 0); 
        if(res == -1){
            printf("send data fail.\n");
            continue;
        }
    }
    close(sockConn);
}
/*
param1: pid=0 等待进程组号与目前进程相同的任何子进程，也就是说任何和调用waitpid()函数的进程在同一个进程组的进程。
param2: 将保存子进程的状态信息
param3: WNOHANG 如果pid指定的子进程没有结束，则waitpid()函数立即返回0，而不是阻塞在这个函数上等待；
如果结束了，则返回该子进程的进程号。
*/
void wait_child(int signo) {
    while(waitpid(0 , NULL , WNOHANG) == 0) ; 
    return ;
}
 
int main(void){
    int sockSer = socket(AF_INET, SOCK_STREAM, 0);
    if(sockSer == -1){
        perror("socket");
        return -1;
    }
    struct sockaddr_in addrSer, addrCli;
    addrSer.sin_family = AF_INET;
    addrSer.sin_port = htons(SERVER_PORT);
    addrSer.sin_addr.s_addr = inet_addr(SERVER_IP);

    socklen_t len = sizeof(struct sockaddr);
    int res = bind(sockSer, (struct sockaddr*)&addrSer, len);
    if(res == -1){
        perror("bind");
        close(sockSer);
        return -1;
   }

    listen(sockSer, LISTEN_QUEUE);

    int sockConn;
    while(1){
        printf("Server Wait Client Connect.......\n");
        sockConn = accept(sockSer, (struct sockaddr*)&addrCli, &len);
        if(sockConn == -1){
            printf("Server Accept Client Connect Fail.\n");
            continue;
        }else{
            printf("Server Accept Client Connect Success.\n");
            printf("Client IP:>%s\n", inet_ntoa(addrCli.sin_addr));
            printf("Client Port:>%d\n",ntohs(addrCli.sin_port));
        }

        pid_t pid;
        pid = fork();
        if(pid == 0){
            close(sockSer); // 关闭父进程的 listen fd
            Process_Handler(sockConn);
            printf("Client Port:>%d exit\n",ntohs(addrCli.sin_port));
            exit(0);
        }else if(pid > 0){
            close(sockConn); // 关闭子进程的 accept 返回用来连接的 fd
            signal(SIGCHLD , wait_child) ; 
            continue;    
        }else{
            printf("Create Process Fail.\n");
            continue;
        }
    }
    close(sockSer);
    return 0;
}