#include "kernel/types.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

void find(char* path, const char*filename) {
    char buf[512],*p;
    int fd;
    struct dirent de;   // save inum and name of file/directory
    struct stat st;     // save the status information of the file

    if((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }
    
    if(fstat(fd, &st) < 0) {   //get status information of file from fd and save in st
        fprintf(2, "find: cannot fstat %s\n",path);
        close(fd);
        return;
    }
    // parameter error, func first param must be directory
    if(st.type != T_DIR) {
        fprintf(2, "usage: find <DIRECTORY> <filename>\n");
        return;
    }
    strcpy(buf, path);  // copy path to buf
    p = buf + strlen(buf);
    *p++ = '/'; //p指向最后一个'/'之后
    while(read(fd, &de,sizeof(de)) == sizeof(de)) {
        if(de.inum == 0) {
            continue;
        }
        memmove(p, de.name, DIRSIZ); // move de.name  to p
        p[DIRSIZ] = 0;
        if (stat(buf, &st) < 0) {
            fprintf(2, "find: cannot stat %s\n", buf);
            continue;
        }
        //不要在“.”和“..”目录中递归
        if (st.type == T_DIR && strcmp(p, ".") != 0 && strcmp(p, "..") != 0) {
            find(buf, filename);
        } else if (strcmp(filename, p) == 0)
            printf("%s\n", buf);
    }
    close(fd);
}

int main(int argc, char *argv[]) {
    if(argc < 3) {
        fprintf(2, "usage: find <directory> <filename>\n");
        exit(1);
    }
    find(argv[1],argv[2]);
    exit(0);
}