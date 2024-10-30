#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>   // For O_* constants
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>


#define SHM_NAME "/my_shared_memory"
#define SHM_SIZE 1024

int main() {
    int shm_fd;
    char *shm_addr;
    
    // 創建共享內存對象
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }
    
    // 調整共享內存大小
    if (ftruncate(shm_fd, SHM_SIZE) == -1) {
        perror("ftruncate");
        exit(1);
    }
    
    // 映射共享內存到地址空間
    shm_addr = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_addr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // 創建子進程
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        // 子進程：從共享內存讀取數據
        sleep(1);  // 確保父進程先寫入數據
        printf("子進程讀取到的數據: %s\n", shm_addr);

        // 解除映射
        if (munmap(shm_addr, SHM_SIZE) == -1) {
            perror("munmap");
            exit(1);
        }

        exit(0);
    } else {
        // 父進程：寫入數據到共享內存
        const char *message = "Hello from shared memory!";
        strncpy(shm_addr, message, SHM_SIZE);
        printf("父進程寫入數據: %s\n", message);

        // 等待子進程結束
        wait(NULL);

        // 解除映射並刪除共享內存對象
        if (munmap(shm_addr, SHM_SIZE) == -1) {
            perror("munmap");
            exit(1);
        }

        if (shm_unlink(SHM_NAME) == -1) {
            perror("shm_unlink");
            exit(1);
        }
    }

    return 0;
}