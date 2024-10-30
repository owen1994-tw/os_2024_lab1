#include "sender.h"
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


void send(message_t* message, mailbox_t* mailbox_ptr){
    /*  TODO: 
        1. Use flag to determine the communication method
        2. According to the communication method, send the message
    */



   if(mailbox_ptr->flag == 1)
   {
		printf("Sending message: %s",message->content);

   }

}

int main(int argc, char *argv[]){
    /*  TODO: 
        1) Call send(message, &mailbox) according to the flow in slide 4
        2) Measure the total sending time
        3) Get the mechanism and the input file from command line arguments
            • e.g. ./sender 1 input.txt
                    (1 for Message Passing, 2 for Shared Memory)
        4) Get the messages to be sent from the input file
        5) Print information on the console according to the output format
        6) If the message form the input file is EOF, send an exit message to the receiver.c
        7) Print the total sending time and terminate the sender.c
    */


    if (argc != 3) {
        printf("Usage: %s <number> <filename>\n", argv[0]);
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
    mbox.flag = tmp_num;
    mbox.storage.shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (mbox.storage.shm_fd  == -1) {
        perror("shm_open");
        exit(1);
    }

    // 調整共享內存大小
    if (ftruncate(mbox.storage.shm_fd , SHM_SIZE) == -1) {
        perror("ftruncate");
        exit(1);
    }

    // 映射共享內存到地址空間
    message_t* msg = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mbox.storage.shm_fd , 0);
    if (msg == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }


    // 提取第二個參數中的文件名
    char *filename = argv[2];

    // 打開文件
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }


    if(mbox.flag == 1)
    {
        printf("Message Passing\n");
    }
    else
    {
        printf("Shared Memory\n");
    } 



    // 創建信號量
    sem_t* sem_full = sem_open(SEM_FULL, O_CREAT, 0666, 0);
    sem_t* sem_empty = sem_open(SEM_EMPTY, O_CREAT, 0666, 1);


    // 逐行讀取文件內容
    char line[50]; // 定義一個足夠大的緩衝區來存儲每一行
    while (fgets(line, sizeof(line), file)) 
    {

        strcpy(msg->content, line);
        send(msg,&mbox);
        sem_wait(sem_empty); // 等待滿信號量
        clock_gettime(CLOCK_MONOTONIC, &start);
        sem_post(sem_full); // 接收完成，釋放空信號量
    }
    fclose(file);


    printf("\n");
	strcpy(msg->content, "EOF");
	send(msg, &mbox);

    printf("\nEnd of input file exit\n");

    clock_gettime(CLOCK_MONOTONIC, &end);

    sem_close(sem_full);
    sem_close(sem_empty);

    time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) * 1e-9;

    printf("Total time taken in sending msg %lf s\n", time_taken);

    munmap(msg, SHM_SIZE);
    close(mbox.storage.shm_fd );




    return 0;

    
}