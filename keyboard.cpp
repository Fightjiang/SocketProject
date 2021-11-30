#include <sys/time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <error.h>
#include <sys/select.h>
#include <unistd.h>
int main() {
    int keyboard = open("/dev/tty" , O_RDONLY | O_NONBLOCK) ; 
    assert(keyboard > 0)  ; 
    printf("starting input !\n\n") ; 
    while(1) {
        struct timeval timeout ; 
        timeout.tv_sec = 5 ; 
        timeout.tv_usec = 0 ; 
        fd_set readfd ; 
        FD_ZERO(&readfd) ; 
        FD_SET(keyboard , &readfd) ; 
        int ret = select(keyboard + 1 , &readfd , NULL , NULL , &timeout) ; 
        if(ret == 0) printf("time out!\n") ; 
        if(FD_ISSET(keyboard , &readfd)) {
            char ch ; 
            int i = read(keyboard , &ch , 1) ; 
            if(ch == '\n') continue ;
            printf("The input is %c\n" , ch) ; 
            if(ch == 'q') break ;
        }
    }
    return 0 ; 
}