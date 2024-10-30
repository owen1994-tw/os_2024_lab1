#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <time.h>

typedef struct {
    int flag;      // 1 for message passing, 2 for shared memory
    union{
        int msqid;  //meassage passing system V api
        int shm_fd; //shared memory POSIX api
        char* shm_addr;
    }storage;
} mailbox_t;


typedef struct {
    /*  TODO: 
        Message structure for wrapper
    */
    char content[50];     // 消息內容
} message_t;

void send(message_t* message, mailbox_t* mailbox_ptr);