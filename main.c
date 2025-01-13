#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "string.h"

int main(int argc, char *argv[]){

    int fd;
    int length;
    if(argc < 2){
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    // 打开文件
    fd = open(argv[1], O_RDWR);

    if(fd == -1){
        printf("Error: \n");
        return 1;
    }

    if(argc == 3){
        length = write(fd, argv[2], strlen(argv[2]) + 1);
        printf("write %d bytes to %s\n", length, argv[1]);
    }
    else {
        char buffer[1024];
        length = read(fd, buffer, sizeof(buffer));
        printf("Read %d bytes from %s\n", length, argv[1]);
        printf("%s\n", buffer);
    }
    close(fd);

    return 0;

}