#include "app_assembly.h"
#include "app_assembly_add.h"

static int fd;

int my_add(int a, int b){
    return a + b;
}

int my_inline_assembly_add(int a, int b){
    int sum;

    __asm__ __volatile__ (
        "add %0, %1, %2" 
        : "=r"(sum) 
        : "r"(a), "r"(b) 
        : "cc"
    );

    return sum;
}

int my_inline_assembly_add2(int a, int b){
    int sum;

    // 这里 的  result value1 value2 是符号名

    __asm__ __volatile__ (
        "add %[result], %[value1], %[value2]" 
        : [result]"=r"(sum) 
        : [value1]"r"(a), [value2]"r"(b) 
        : "cc"
    );

    return sum;
}
int my_inline_assembly_earlyclobber_test(int a, int b){
    int sum;

    __asm__ __volatile__ (
        "add %0, %1, %2 \n"
        "add %1, %1, #1 \n" // 寄存器 1 加 1
        "add %2, %2, #1 \n" // 寄存器 2 加 1
        : "=&r"(sum) //  使用 "=&r" 解决  earlyclobber 问题，分配单独的寄存器 ，不复用 之前已经 用过的寄存器
        // : "=r"(sum) // 输出 17  有问题 
        : "r"(a), "r"(b) 
        : "cc"
    );

    return sum;
}
int app_assembly_init(int argc, char *argv[]){
    // my_add(1, 2);
    // add(1, 2);
    printf("app_assembly_add %d \n", app_assembly_add(2, 3));
    printf("my_inline_assembly_add %d \n", my_inline_assembly_add(8, 3));
    printf("my_inline_assembly_earlyclobber_test %d \n", my_inline_assembly_earlyclobber_test(8, 8));
    return 0;
}

void app_assembly_main(int argc, char *argv[]){
    app_assembly_init(argc, argv);
}


