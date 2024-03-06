#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define RD 0 //pipe的读端
#define WR 1 //pipe的写端

int main(int argc, char *argv[])
{
    //parent to child 
    int fd_ptc[2];
    int fd_ctp[2];

    char buff = 'd';
    pipe(fd_ctp);
    pipe(fd_ptc);
    int pid = fork();
    int exit_status = 0;

    if (pid < 0) {
        fprintf(2,"fork() error\n");
        close(fd_ctp[RD]);
        close(fd_ctp[WR]);
        close(fd_ptc[WR]);
        close(fd_ptc[RD]);
    } else if (pid == 0) {    // child 
        close(fd_ptc[WR]);  // close c write
        close(fd_ctp[RD]);  // close p read
        //child read from parent 
        if (read(fd_ptc[RD], &buff, sizeof(char)) != sizeof(char)) {
            fprintf(2, "child read() error\n");
            exit_status = 1;    // wrong
        } else {
            fprintf(1, "%d: received ping\n", getpid());
        }
        //child write to parent
        if (write(fd_ctp[WR], &buff, sizeof(char)) != sizeof(char)) {
            fprintf(2, "child write() error\n");
        }
        close(fd_ctp[WR]);
        close(fd_ptc[RD]);
    }else {                 //parent
        close(fd_ptc[RD]);
        close(fd_ctp[WR]);
        // parent write to child
        if (write(fd_ptc[WR], &buff, sizeof(char)) != sizeof(char)) {
            fprintf(2, "parent write() error\n");
            exit_status = 1;
        }
        if (read(fd_ctp[RD],&buff, sizeof(char)) != sizeof(char)) {
            fprintf(2,"parent read() error!\n");
            exit_status = 1;
            
        } else {
            fprintf(1, "%d: received pong\n", getpid());
        }
        close(fd_ctp[RD]);
        close(fd_ptc[WR]);
    }
    exit(exit_status);
}