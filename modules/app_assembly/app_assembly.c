#include "app_assembly.h"

static int fd;


int add(int a, int b){
    return a + b;
}
int app_assembly_init(int argc, char *argv[]){

    add(1, 2);
    return 0;
}

void app_assembly_main(int argc, char *argv[]){
    app_assembly_init(argc, argv);
}
