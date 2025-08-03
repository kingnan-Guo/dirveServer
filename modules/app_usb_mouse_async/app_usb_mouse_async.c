#include "app_usb_mouse_async.h"


// 使用 libusb 异步读取 usb 数据

struct usb_mouse
{
    struct libusb_device_handle *handle; // libusb 设备句柄
    int interface; // 接口号
    int endpoint; // 端点号
    struct usb_mouse *next; // 链表 下一个 usb 鼠标节点
    unsigned char buf[16]; // 缓冲区 用于存储读取到的数据
    struct libusb_transfer *transfer; // libusb 传输结构体
    int transferred; // 实际传输的字节数
};

static struct usb_mouse *usb_mouse_list; // usb 鼠标链表



void free_usb_mouses(struct usb_mouse *usb_mouse_list){
    struct usb_mouse *pmouse;
    while (usb_mouse_list){
        pmouse = usb_mouse_list->next; // 记录下一个节点
        free(usb_mouse_list); // 释放当前节点
        usb_mouse_list = pmouse; // 指向下一个节点
    }

}

// 从list 里面 解析 有多少个 设备
int get_usb_mouses(libusb_device **devs, int num_devices, struct usb_mouse **usb_mouse_list){
    int err;
    // libusb_device **devs;// 定义一个 libusb_device 指针数组
    libusb_device *dev;// 定义一个 libusb_device 指针

    int interface_num = 0;// 接口号
    int interfcace_count = 0;// 接口 数量

    struct libusb_config_descriptor *config_desc;// 配置描述符 结构体
    // struct libusb_interface *interface;// 接口 结构体
    // struct libusb_interface_descriptor *interface_desc;// 接口描述符 结构体


    struct libusb_device_handle *dev_handle = NULL;// libusb 设备句柄

    int endpoint = 0;// 端点号
    int endpoint_interrupt_in = 0;// 中断输入端点



    struct usb_mouse *pmouse;
    struct usb_mouse *list = NULL; // 链表头
    int mouse_count = 0; // 鼠标数量
    
    
    // 循环设备列表  从每一个设备描述符 查找 配置描述符
    for (int i = 0; i < num_devices; i++)
    {
        
        dev = devs[i];// 记录设备
        // 获取这个设备的 第0 个配置 ？？ 因为一个设备有多个 配置描述符，先在 设备描述符中获取配置描述符的数量，然后在 进行for 循环 每一个 配置描述符，这里 为了简单只 获取第一个 配置描述符
        err = libusb_get_config_descriptor(devs[i], 0, &config_desc);// 获取到 的 config_desc 是一个指向 libusb_config_descriptor 结构体的指针
        if (err) {
            fprintf(stderr, "could not get configuration descriptor\n");
            continue;
        }
        // 从配置描述符中找到接口描述符
        interfcace_count = config_desc->bNumInterfaces;// 获取配置描述符中的 接口数量
        for (int interfcace_index = 0; interfcace_index < interfcace_count; interfcace_index++)
        {
            //  altsetting 是一个数组，表示这个接口有多个设置
            const struct libusb_interface_descriptor *interface_desc = &config_desc->interface[interfcace_index].altsetting[0];// 获取接口描述符
            // interface_num = interface_desc->bInterfaceNumber;// 获取接口号

            //   对于鼠标来说 binInterfaceClass = 3   binInterfaceProtocol（协议） = 2
            // bInterfaceClass = 0x03 (HID Class)
            // bInterfaceSubClass = 0x01 (Boot Interface)
            // bInterfaceProtocol = 0x02 (Mouse)
            if (interface_desc->bInterfaceClass == 3 && interface_desc->bInterfaceProtocol == 2) {
                printf("find mouse interface\n");
                // 接口下面 有多个端点， 获取中断端点的 端点号， 因为要使用中断传输 





                int endpoints_count   = interface_desc->bNumEndpoints;// 获取端点数量
                for (int ep = 0; ep < endpoints_count; ep++)
                {
                    // 判断是 中断传输的  端点 ； 同时判断 是 输入端点
                    if(
                        // interface_desc->endpoint[ep].bmAttributes  bmAttributes端点的属性
                        // 3 = 0b11  ; & 3 表示 端点的属性的低两位是传输类型
                        // LIBUSB_TRANSFER_TYPE_INTERRUPT   中断传输类型
                        (interface_desc->endpoint[ep].bmAttributes & 3) == LIBUSB_TRANSFER_TYPE_INTERRUPT || 
                        
                        //  因为要想读鼠标的数据所以要找到方向 是输入的 端点
                        // bEndpointAddress 端点地址
                        //  LIBUSB_ENDPOINT_IN 是 0x80 端点地址的高位是1表示输入端点
                        // & 0x80 是为了获取端点地址的高位，如果高位是1则表示输入端点
                        (interface_desc->endpoint[ep].bEndpointAddress & 0x80) == LIBUSB_ENDPOINT_IN
                    ){
                        // 找到 输入方向的 中断端点
                        // 记录下来
                        printf("find mouse endpoint\n");
                            // 从接口描述符中 获取到 鼠标 的 endpoint
                        endpoint_interrupt_in = interface_desc->endpoint[ep].bEndpointAddress;// 获取端点地址

                        interface_num = interface_desc->bInterfaceNumber;// 获取接口号



                        pmouse = malloc(sizeof(struct usb_mouse));// 分配内存

                        if(!pmouse){
                            fprintf(stderr, "can not malloc\n");
                            return -1;
                        }

                        // 使用 libusb 打开设备

                        err = libusb_open(dev, &dev_handle);
                        if(err){
                            fprintf(stderr, "failed to open usb mouse\n");
                            return -1;
                        }


                        pmouse->endpoint = endpoint_interrupt_in;// 记录端点地址
                        pmouse->interface = interface_num;// 记录接口号
                        pmouse->handle = dev_handle;// 记录设备句柄
                        pmouse->next = NULL;// 下一个节点为空

                        // 将这个节点插入到 链表头
                        if(list == NULL){
                            // 如果没有记录那么 记录当前的 pmouse
                            list = pmouse;// 这个是 链表的 第一个插入的 数据； 但是 如果有多个数据那么是这是 整个链表的 最后一个数据
                        }
                        else {
                            // 每次插入都将当前的 pmouse 的 next 指向 链表头
                            pmouse->next = list; // 将当前的 pmouse 的 next 指向 链表头
                            list = pmouse; // 将链表头指向当前的 pmouse； 这样就把多个找到的鼠标 串联到了一起，然后 第一个找到的 在链表 的最末尾
                        }




                        mouse_count++;// 鼠标数量加1


                        break;// 找到就跳出循环

                    }
                }
                




            }

        }
        

        // 释放配置描述符
        libusb_free_config_descriptor(config_desc);


    }

    *usb_mouse_list = list;// 将链表头返回
    // if(mouse_count <= 0){
    //     fprintf(stderr, "no usb mouse found\n");
    //     return -1;
    // }
    
    
    return mouse_count;
}



/**
 * @brief  中断传输的回调函数
 * 
 * 
 * * @param transfer 
 *      指向 libusb_transfer 结构体的指针
 *      这个结构体包含了传输的状态、数据等信息
 */
static void mouse_irq(struct libusb_transfer *transfer){

    static int count = 0;
    //  LIBUSB_TRANSFER_COMPLETED 代表传输完成
    if(transfer->status == LIBUSB_TRANSFER_COMPLETED){
        printf("mouse_irq transfer completed\n");
        printf("%04d datas: ", count++);
        //  获取传输的数据
            // 对于鼠标可以得到 4 个字节的  数据
            printf("Received data: ");
            //  actual_length 表示实际传输的字节数
            for (int i = 0; i < transfer->actual_length; i++)
            {
                printf("%02x ",transfer->buffer[i]);
            }
            printf("\n");

    }

    // 再次 提交
    if(libusb_submit_transfer(transfer) < 0){
        fprintf(stderr, "libusb_submit_transfer err\n");
    }

}


int app_usb_mouse_async_init(int argc, char *argv[]){
    int err;
    libusb_device **devs;// 定义一个 libusb_device 指针数组
    libusb_device *dev;// 定义一个 libusb_device 指针
    int num_devices;// 设备 数量
    int mouce_count;// 鼠标数量
    struct usb_mouse *pmouse;// usb 鼠标指针




    //  初始化 libusb
    err = libusb_init(NULL);
    if (err < 0) {
        fprintf(stderr, "failed to initialise libusb %d - %s\n", err, libusb_strerror(err));
        exit(1);
    }

    // 获取 设备列表
    num_devices = libusb_get_device_list(NULL, &devs);
    if(num_devices < 0){
        fprintf(stderr, "failed to get device list\n");
        libusb_exit(NULL);
        exit(1);
    }

    // 循环设备列表  从每一个设备描述符 查找 配置描述符
    // 获取 到所有的鼠标 链表
    mouce_count = get_usb_mouses(devs, num_devices, &usb_mouse_list);


    if(mouce_count <= 0){
        fprintf(stderr, "no usb mouse found\n");
        // 释放设备列表
        libusb_free_device_list(devs, 1);
        libusb_exit(NULL);
        exit(1);
    }
    fprintf(stdout, "get %d usb mouse\n", mouce_count);


    // 释放掉








    // 释放设备列表
    libusb_free_device_list(devs, 1);// 释放设备列表，第二个参数为1表示释放设备列表中的设备描述符

    // claim interface  认领接管接口

    pmouse = usb_mouse_list;// 获取链表头

    while (pmouse)
    {
        libusb_set_auto_detach_kernel_driver(pmouse->handle, 1);// 自动分离内核驱动
        err = libusb_claim_interface(pmouse->handle, pmouse->interface );// 认领接口
        if (err)
        {
            fprintf(stderr, "failed to libusb_claim_interface\n");
            exit(1);
        }

        //  然后 在指向 下一个 usb 鼠标
        pmouse = pmouse->next;
    }
    fprintf(stdout, "libusb_claim_interface ok\n");



    //  为 每一个 鼠标设备   ， alloc mouse,  去填充他的 transfer 结构体; 提交 transfer
    pmouse = usb_mouse_list;

    while (pmouse)
    {
        // alloc transfer
        pmouse->transfer = libusb_alloc_transfer(0);// 分配传输结构体

        // 填充 transfer 结构体
        // 要做中断传输
        libusb_fill_interrupt_transfer(
            pmouse->transfer,
            pmouse->handle,
            pmouse->endpoint,
            pmouse->buf,
            sizeof(pmouse->buf),
            mouse_irq,
            pmouse, // 用户数据， mouse_irq 被调用的时候 的传入 的参数
            0
        );

        // 提交
        libusb_submit_transfer(pmouse->transfer);


       pmouse = pmouse->next;
    }
    


    
    


    fprintf(stdout, "libusb_claim_interface ok\n");// 认领成功

    // 发起传输 获取 鼠标数据
    // libusb_interrupt_transfer 中断传输数据
    unsigned char buffer[16]; // 缓冲区 用于存储读取到的数据
    int transferred; // 实际传输的字节数
    while (1)
    {
        struct timeval tv = { 5, 0 }; // 设置超时时间为5秒
        int err;

        //  libusb_handle_events_timeout 是 阻塞的，会等待事件发生或者超时； 是 处理事件 函数
        err = libusb_handle_events_timeout(NULL, &tv);// 处理事件，等待5秒，如果有事件发生则返回0，否则返回-1



        if (err < 0) {
            fprintf(stderr, "libusb_handle_events_timeout err\n");
            break;
        }

    }
    

    pmouse = usb_mouse_list;
    while (pmouse)
    {
        // 关闭
        libusb_release_interface(pmouse->handle, pmouse->interface);// 释放接口
        libusb_close(pmouse->handle);// 关闭设备句柄
        libusb_exit(NULL);// 退出 libusb

        pmouse = pmouse->next;
    }

    // 释放 usb 鼠标链表
    free_usb_mouses(usb_mouse_list);// 释放链表

    

    return 0;
}


void app_usb_mouse_async_main(int argc, char *argv[]){
    app_usb_mouse_async_init(argc, argv);
}