#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

// 定义消息队列中的消息结构体
struct msg_buffer {
    long msg_type;
    char msg_text[100];
};

int main() {
    key_t key;
    int msgid;
    struct msg_buffer message;

    // 创建唯一的key
    key = ftok("progfile", 65);

    // 获取消息队列的标识符
    msgid = msgget(key, 0666 | IPC_CREAT);

    // 接收来自队列的消息
    msgrcv(msgid, &message, sizeof(message), 1, 0);

    // 打印接收到的消息
    printf("Data received: %s\n", message.msg_text);

    // 删除消息队列
    msgctl(msgid, IPC_RMID, NULL);

    return 0;
}
