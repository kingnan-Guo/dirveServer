#include "app_mmap.h"


static int fd;
int app_mmap_init(int argc, char *argv[]){

    int val;
    int ret;
    char *buf;
    int len;
    char str[1024];
 
    if(argc < 2){
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    // 打开文件
    fd = open(argv[1], O_RDWR | O_NONBLOCK);// O_RDWR 读写方式打开文件, O_NONBLOCK 非阻塞方式打开文件
    if(fd == -1){
        printf("can not open file %s\n", argv[1]);
        return -1;
    }


    // mmap 映射 操作
    /**
     *  
     *  void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
     *  
     *  参数
     * 
     *      addr: 映射区域的首地址 ；       NULL  表示由系统自动选择起始地址
     *      length: 映射区域的长度；        1024 * 8    表示映射文件大小为 8K
     *      prot: 映射区域的保护方式；      PROT_READ | PROT_WRITE   表示映射区域可读可写
     *      flags: 映射区域的状态；        MAP_SHARED 表示映射区域与文件同步 ； MAP_PRIVATE 私有的  使用 私有的 导致 对比不成功；私有的 是只有写的能看到 ，驱动程序 也看不到
     *      fd: 文件描述符；              fd  文件描述符； 要映射哪个文件
     *      offset: 文件映射的偏移量；     0    文件偏移量
     *      
     *      
     *  返回值
     *     成功返回 映射区域的首地址
     *     失败返回 MAP_FAILED
     * 
     * 
     */
    // buf = mmap(NULL, 1024 * 8, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    buf = mmap(NULL, 1024 * 8, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (buf == MAP_FAILED) {
        printf("mmap failed 失败 \n");
        return -1;
    }
    printf("mmap address = 0x%s\n", buf);
    printf("buffer data  原始数据 还没写数据 = 0x%s\n", buf);

    // 写数据, 
    strcpy(buf, "kingnan data ");// 直接写 这块 内存

    // 读数据  并且 比较 写的数据
    read(fd, str, 1024);

    // 比较数据
    if(strcmp(buf, str) == 0){
        printf("compare ok!\n");

    } else {
        printf("compare failed!\n");
		printf("str = %s!\n", str);  /* old */
		printf("buf = %s!\n", buf);  /* new */
    }


    while (1)
    {
        printf("app_mmap_main \n");
        sleep(5);/* cat /proc/pid/maps */
    }


    munmap(buf, 1024*8);// 解除映射
    close(fd);
    return 0;
}

void app_mmap_main(int argc, char *argv[]){
    app_mmap_init(argc, argv);
}