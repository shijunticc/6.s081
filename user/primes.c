#include "kernel/types.h"
#include "user/user.h"

#define RD 0
#define WR 1

const uint INT_LEN = sizeof(int);

/**
 * @brief 读取左邻居的第一个数据
 * @param lpipe 左邻居的管道符
 * @param pfirst 用于存储第一个数据的地址
 * @return 如果没有数据返回-1,有数据返回0
 */
int lpipe_first_data(int lpipe[2], int *dst)
{
    if(read(lpipe[RD], dst,sizeof(int)) == sizeof(int)) {
        printf("prime %d\n", *dst);
        return 0;
    }
    return -1;
}


/**
 * @brief 读取左邻居的数据，将不能被first整除的写入右邻居
 * @param lpipe 左邻居的管道符
 * @param rpipe 右邻居的管道符
 * @param first 左邻居的第一个数据
 */
void transmit_data(int lpipe[2], int rpipe[2], int first)
{
    int data;
    //read from lpipe
    while(read(lpipe[RD], &data, sizeof(int)) == sizeof(int)) {
        //choose data that can't divide first 
        if(data % first) {
            write(rpipe[WR], &data, sizeof(int));
        }
    }
    close(lpipe[RD]);
    close(rpipe[WR]);
}

/**
 * @brief 寻找素数
 * @param lpipe 左邻居管道
 */
void primes(int lpipe[2])
{
    close(lpipe[WR]);
    int first;
    if(lpipe_first_data(lpipe, &first) == 0) {
        int p[2];
        pipe(p);
        transmit_data(lpipe, p, first);
        
        if(fork() == 0) {
            primes(p);
        } else {
            close(p[RD]);
        }
    }
    exit(0);
}

int main(int argc, char const *argv[])
{
    int p[2];
    pipe(p);

    for(int i = 2; i <= 35 ; i++)
        write(p[WR], &i, sizeof(i));
    if(fork() == 0) {
        primes(p);
    } else {
        close(p[WR]);
        close(p[RD]);
        wait(0);
    }
    exit(0);
}