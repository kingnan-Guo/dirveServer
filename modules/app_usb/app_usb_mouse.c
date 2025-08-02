#include "app_usb_mouse.h"
#include <libusb-1.0/libusb.h>
// 使用 libusb 同步读取 usb 数据
int app_usb_mouse_init(int argc, char *argv[]){
    int err;
    libusb_device **devs;// 定义一个 libusb_device 指针数组
    libusb_device *dev;// 定义一个 libusb_device 指针
    int num_devices;// 设备 数量
    int interface_num = 0;// 接口号
    int interfcace_count = 0;// 接口 数量

    struct libusb_config_descriptor *config_desc;// 配置描述符 结构体
    // struct libusb_interface *interface;// 接口 结构体
    // struct libusb_interface_descriptor *interface_desc;// 接口描述符 结构体


    struct libusb_device_handle *dev_handle = NULL;// libusb 设备句柄

    int endpoint = 0;// 端点号
    int endpoint_interrupt_in = 0;// 中断输入端点 地址


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
    for (int i = 0; i < num_devices; i++)
    {
        // dev = devs[i];
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
            struct libusb_interface_descriptor *interface_desc = &config_desc->interface[interfcace_index].altsetting[0];// 获取接口描述符
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

                        dev = devs[i];// 记录设备

                        break;// 找到就跳出循环

                    }
                }
                




            }

        }
        

        // 释放配置描述符
        libusb_free_config_descriptor(config_desc);


    }
    










    // 使用 libusb 打开设备

    err = libusb_open(dev, &dev_handle);
    if(err){
        fprintf(stderr, "failed to open usb mouse\n");
        exit(1);
    }
    fprintf(stdout, "libusb_open ok\n");

    // 释放设备列表
    libusb_free_device_list(devs, 1);// 释放设备列表，第二个参数为1表示释放设备列表中的设备描述符

    // claim interface  认领接管接口
    libusb_set_auto_detach_kernel_driver(dev_handle, 1);// 自动分离内核驱动
    err = libusb_claim_interface(dev_handle, interface_num);// 认领接口

    if (err)
    {
        fprintf(stderr, "failed to libusb_claim_interface\n");
        exit(1);
    }
    fprintf(stdout, "libusb_claim_interface ok\n");// 认领成功

    // 发起传输 获取 鼠标数据
    // libusb_interrupt_transfer 中断传输数据
    unsigned char buffer[16]; // 缓冲区 用于存储读取到的数据
    int transferred; // 实际传输的字节数
    while (1)
    {
        // 发起中断传输
        err = libusb_interrupt_transfer(dev_handle, endpoint_interrupt_in, buffer, sizeof(buffer), &transferred, 5000);// 5000 毫秒 超时
        if(err >=0){
            // 传输成功
            // 处理数据

            // 对于鼠标可以得到 4 个字节的  数据
            printf("Received data: ");
            for (int i = 0; i < transferred; i++)
            {
                printf("%02x ", buffer[i]);
            }
            printf("\n");


        }
        else if(err == LIBUSB_ERROR_TIMEOUT){
            fprintf(stderr, "libusb_interrupt_transfer timout\n");
        }
        else {

            fprintf(stderr, "failed to libusb_interrupt_transfer\n");
            // exit(1);
            // break;

        }

    }
    


    // 关闭
    libusb_release_interface(dev_handle, interface_num);// 释放接口
    libusb_close(dev_handle);// 关闭设备句柄
    libusb_exit(NULL);// 退出 libusb
    return 0;
}

void app_usb_mouse_main(int argc, char *argv[]){
    app_usb_mouse_init(argc, argv);
}