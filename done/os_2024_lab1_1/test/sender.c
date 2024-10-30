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

    strcpy(message.msg_text, "senddddd");

    // 创建唯一的key
    key = ftok("progfile", 65);

    // 创建消息队列，返回队列标识符
    msgid = msgget(key, 0666 | IPC_CREAT);
    message.msg_type = 1;

    // 使用 strcpy 将固定字符串复制到消息中
    strcpy(message.msg_text, "senddddd");

    // 发送消息到队列，并检查发送是否成功
    if (msgsnd(msgid, &message, sizeof(message), 0) == -1) {
        perror("msgsnd failed");
        exit(EXIT_FAILURE);
    }

    printf("Data sent: %s\n", message.msg_text);

    return 0;
}
