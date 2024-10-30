#include "receiver.h"
#include <stdio.h>
#include <sys/mman.h>
#include <time.h>
#include <ctype.h> 
struct timespec start, end;
double time_taken;

#define SHM_SIZE 1024
#define SHM_NAME "/my_shm"
#define SEM_EMPTY "/sem_empty"
#define SEM_FULL "/sem_full"


void receive(message_t* message_ptr, mailbox_t* mailbox_ptr){
    /*  TODO: 
        1. Use flag to determine the communication method
        2. According to the communication method, receive the message
    */


    // 創建信號量
    sem_t* sem_full = sem_open(SEM_FULL, O_CREAT, 0666, 1);
    sem_t* sem_empty = sem_open(SEM_EMPTY, O_CREAT, 0666, 0);

    if (sem_full == SEM_FAILED || sem_empty == SEM_FAILED) {
        perror("sem_open failed");
        return;
    }

    if(mailbox_ptr->flag == 1)
    {

        while(1)
        {
            // sem_wait(sem_full); // 等待滿信號量

            if (msgrcv(mailbox_ptr->storage.msqid, message_ptr, sizeof(message_t), 1, 0) == -1) {
                perror("msgrcv failed");
                exit(1);
            }

            if (strcmp(message_ptr->content, "EOF") == 0) 
            {  // 檢查是否接收到結束標記
                printf("\n");
                printf("sender exit\n");

                msgctl(mailbox_ptr->storage.msqid, IPC_RMID, NULL);  // 删除消息队列

                sem_post(sem_empty); // 接收完成，釋放空信號量
                break;  // 收到 EOF，退出循環
            }

            printf("Receiving message: %s", message_ptr->content);
            sem_post(sem_empty); // 接收完成，釋放空信號量
        }
    }

    sem_close(sem_full);
    sem_close(sem_empty);

    sem_unlink(SEM_FULL);
    sem_unlink(SEM_EMPTY);
}

int main(int argc, char *argv[]){
    /*  TODO: 
        1) Call receive(&message, &mailbox) according to the flow in slide 4
        2) Measure the total receiving time
        3) Get the mechanism from command line arguments
            • e.g. ./receiver 1
        4) Print information on the console according to the output format
        5) If the exit message is received, print the total receiving time and terminate the receiver.c
    */

    
    if (argc != 2) {
        printf("Usage: %s <number> \n", argv[0]);
        return 1;
    }

    // 提取第一個參數中的數字
    char *num_str = argv[1];
    int tmp_num = 0;
    char *p = num_str;
    while (*p) {
        if (isdigit(*p)) {
            tmp_num = tmp_num * 10 + (*p - '0'); // 將字符轉換為數字
        }
        p++;
    }


    
    mailbox_t mbox;
    mbox.flag = tmp_num;  // 1 for Message Passing, 2 for Shared Memory

    key_t key = ftok("/tmp/myfile", 65);;
    mbox.storage.msqid= msgget(key, 0666 | IPC_CREAT);
    if (mbox.storage.msqid == -1) {
        perror("msgget failed");
        exit(1);
    }


    message_t* msg = malloc(sizeof(message_t));
    memset(msg->content, 0, sizeof(msg->content));  // 将 content 初始化为0




    if(mbox.flag == 1)
    {
        printf("Message Passing\n");
    }
    else
    {
        printf("Shared Memory\n");
    } 

    clock_gettime(CLOCK_MONOTONIC, &start);
    receive(msg,&mbox);
    clock_gettime(CLOCK_MONOTONIC, &end);


    time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) * 1e-9;

    printf("Total time taken in sreceiveng msg %lf s\n", time_taken);

    msgctl(mbox.storage.msqid, IPC_RMID, NULL);  // 删除消息队列

    free(msg);


    return 0;

}