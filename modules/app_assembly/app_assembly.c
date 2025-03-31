#include "app_assembly.h"
#include "app_assembly_add.h"

static int fd;

int my_add(int a, int b){
    return a + b;
}
int app_assembly_init(int argc, char *argv[]){
    // my_add(1, 2);
    // add(1, 2);
    printf("app_assembly_add %d \n", app_assembly_add(2, 3));
    return 0;
}

void app_assembly_main(int argc, char *argv[]){
    app_assembly_init(argc, argv);
}
