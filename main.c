#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "string.h"
#include <unistd.h>

// main /dev/my_led  on
// main /dev/my_led  off


int main(int argc, char *argv[]){

    int fd;
    int length;
    char status;
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



    // if(argc == 3){

    //     if(strcmp(argv[2], "on") == 0){
    //         status = '1';
    //         length = write(fd, &status, 1);
    //         printf("write %d bytes to %s\n", length, argv[1]);
    //     }
    //     else if(strcmp(argv[2], "off") == 0){
    //         status = '0';
    //         length = write(fd, &status, 1);
    //         printf("write %d bytes to %s\n", length, argv[1]);
    //     }
    //     else{
    //         printf("Usage: %s <filename> on | off\n", argv[0]);
    //         return 1;
    //     }


    // } else if(argc == 2) {
    //     char buffer[1024];
    //     length = read(fd, buffer, sizeof(buffer));
    //     printf("Read %d bytes from %s\n", length, argv[1]);
    //     printf("%s\n", buffer);
    // }

    if(argc == 3){
        if(strcmp(argv[2], "on") == 0){
            status = '1';
            length = write(fd, &status, 1);
            printf("write %d bytes to %s  --  %d\n", length, argv[1], status);
        }
        else if(strcmp(argv[2], "off") == 0){
            status = '0';
            length = write(fd, &status, 1);
            printf("write %d bytes to %s  --  %d\n", length, argv[1], status);
        }
        else{
            printf("Usage: %s <filename> on | off\n", argv[0]);
            return 1;
        }

    }
    else if(argc == 2) {
        char buffer[1024];
        length = read(fd, buffer, sizeof(buffer));
        printf("Read %d bytes from %s\n", length, argv[1]);
        printf("%s\n", buffer);
    }


    close(fd);

    return 0;

}