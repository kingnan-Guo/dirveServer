#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb.h>// 重点
#include <linux/input.h>
#include <linux/hid.h>



// 
struct usb_mouse_as_key_desc{
    struct usb_interface *intf;// usb 接口
    const struct usb_device_id *id; // usb 设备 id
    struct usb_device *dev; // usb 设备
    int pipe; // usb 传输管道
    int maxp; // 最大包大小
    int bInterval; // 间隔， 这个是 usb 传输的间隔
    void* data_buffer; // 数据缓冲区
    dma_addr_t data_dma; // 数据缓冲区的 dma 地址
    struct urb* urb; // usb 请求块
};

static void usb_mouse_as_key_irq(struct urb *urb)
{
    struct input_dev *dev = urb->context;// 在 usb_fill_int_urb 中 ， 传入了 input_dev dev ， 所以在上下文 中找到 dev
    // 从 dev 中 获取 usb_mouse_as_key_desc 结构体
    struct usb_mouse_as_key_desc* desc = input_get_drvdata(dev);// 从 input_dev 结构体中 获取 usb_mouse_as_key_desc 结构体

    // 获取 数据
    // 从 data_buffer 中 获取 数据
    signed char* data = desc->data_buffer;// 从 data_buffer 中 获取 数据


    switch (urb->status)// urb->status 是 usb 传输的状态; 
    {
        case 0:// 当前状态 为0 ， 表示传输成功
            break;
        case -ECONNRESET:// 当前状态 为 -ECONNRESET ， 表示传输被取消
        case -ENOENT:// 当前状态 为 -ENOENT ， 表示传输被取消
        case -ECONNABORTED:// 当前状态 为 -ECONNABORTED ， 表示传输被取消
        case -ESHUTDOWN:// 当前状态 为 -ESHUTDOWN ， 表示传输被取消
            return;

        default:
            goto resubmit;
            // usb_submit_urb(desc->urb, GFP_KERNEL);
    }

    // 解析 数据
    // 将 数据 转换为 鼠标事件
    // 将 鼠标事件 发送给 input subsystem
    // printk("get data: 0x%x 0x%x\n", data[0], data[1]);
	// input_report_key(dev, BTN_LEFT,   data[1] & 0x01);
	// input_report_key(dev, BTN_RIGHT,  data[1] & 0x02);
	// input_report_key(dev, BTN_MIDDLE, data[1] & 0x04);
	// input_report_key(dev, BTN_SIDE,   data[1] & 0x08);
	// input_report_key(dev, BTN_EXTRA,  data[1] & 0x10);

	// input_report_rel(dev, REL_X,     data[1]);
	// input_report_rel(dev, REL_Y,     data[2]);
	// input_report_rel(dev, REL_WHEEL, data[3]);

	// input_sync(dev);


    // input_report_key 将 鼠标事件 发送给 input subsystem
    // input_report_key(dev, BTN_LEFT,   data[0] & 0x01);// 左键

    input_report_key(dev, KEY_L, data[1] & 0x01);// 左键
    input_report_key(dev, KEY_R, data[1] & 0x02);// 右键
    input_report_key(dev, KEY_M, data[1] & 0x04);// 中键


    input_sync(dev);// 同步 input subsystem


resubmit:
    usb_submit_urb(desc->urb, GFP_KERNEL);


}





static int usb_mouse_as_key_open(struct input_dev *dev){
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    struct urb* urb;

    // 从 struct input_dev *dev 里面 获取 struct usb_mouse_as_key_desc 结构体
    struct usb_mouse_as_key_desc* desc = input_get_drvdata(dev);

    // 分配 URB / 填充 URB / 提交 urb
    // 1 分配 URB 
    urb = usb_alloc_urb(0, GFP_KERNEL);
    


    // sources/linux-rpi-6.6.y/include/linux/usb.h
    // static inline void usb_fill_int_urb(struct urb *urb, struct usb_device *dev,  unsigned int pipe, void *transfer_buffer, int buffer_length, usb_complete_t complete_fn, void *context, int interval)

    // 2 填充 URB
    usb_fill_int_urb(
        urb,
        desc->dev,// usb 设备
        desc->pipe,// 传输管道
        desc->data_buffer,// 数据缓冲区
        desc->maxp,// 最大包大小
        usb_mouse_as_key_irq,// URB 回调函数
        dev,// 回调函数的 参数 ; 为什么到了 usb_mouse_as_key_irq 函数里面， dev 储存在 urb 的上下文
        desc->bInterval// 传输间隔
    );
    // 告诉 urb 的 当前内存是 dma 内存
    urb->transfer_dma = desc->data_dma;// dma 地址
    urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;// 告诉 urb 的 当前内存是 dma 内存


    // 将 urb 记录在 desc 结构体中
    desc->urb = urb;

    // 3 提交 URB
    int err = usb_submit_urb(urb, GFP_KERNEL);// 提交 URB


    return err;
}



static void usb_mouse_as_key_close(struct input_dev *dev)
{
    struct usb_mouse_as_key_desc* desc = input_get_drvdata(dev);
    // 取消  / 释放 urb
    usb_kill_urb(desc->urb);

    usb_free_urb(desc->urb);
}

/**
probe 函数的 会在每一个匹配的设备的时候都会被调用
 */
static int usb_mouse_as_key_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

    struct input_dev *input_dev;
    struct usb_mouse_as_key_desc *desc;
    struct usb_device *dev = interface_to_usbdev(intf);// 获取 usb 设备
    struct usb_host_interface *interface;// 获取 usb 接口
    struct usb_endpoint_descriptor *endpoint;// 获取 usb 端点 描述符
    int pipe;// 传输管道
    int maxp;// 最大包大小
    int error;

    // 1 记录设备信息 intf 要发起 信息传输的时候 需要用到
    // 2 分配 / 设置 / 注册 input devicve
    // 2.1 input 中 能产生 哪类事件
    // 2.2 input 中 能产生 这类 按键 里的那些事件 ： 例如 ： L/S/Enter
    // 2.3 设置 input 的open函数 
    // 2.4 open 函数 里面  提交URB 提交 URB
    // 2.5 URB 回调函数， 解析数据，上报 输入事件 input_event， 就是解析 左键右键 哪个被按下或松开

    input_dev = devm_input_allocate_device(&intf->dev);// 分配 input_dev 设备
    desc = kmalloc(sizeof(struct usb_mouse_as_key_desc), GFP_KERNEL);// 分配 usb_mouse_as_key_desc 结构体 ; GFP_KERNEL 表示 分配内存的时候， 如果内存紧张， 可以睡眠等待

    interface = intf->cur_altsetting; // 获取 当前接口的 备用设置

    // 判断 端点数据 有 几个
    if(interface->desc.bNumEndpoints != 1){
        // 只支持 一个端点
        return -ENODEV;
    }

    endpoint = &interface->endpoint[0].desc; // 获取 端点描述符

    // 判断端点的描述符  usb_endpoint_is_int_in 是 判断 端点描述符 是否是 中断输入端点
    if(!usb_endpoint_is_int_in(endpoint)){// 不是 中断输入端点
        // 不是 中断输入端点
        return -ENODEV;

    }

    // 获取 管道
    pipe = usb_rcvintpipe(dev, endpoint->bEndpointAddress);

    // 获取 最大包大小
    maxp = usb_maxpacket(dev, pipe);

    
    desc->intf = intf; // 记录 usb 接口
    desc->id = id; // 记录 usb 设备 id 
    desc->dev = dev;// 记录 usb 设备
    desc->pipe = pipe; // 记录 传输管道
    desc->maxp = maxp; // 记录 最大包大小
    desc->bInterval = endpoint->bInterval; // 记录 传输间隔
    desc->data_buffer = usb_alloc_coherent(dev, maxp, GFP_ATOMIC, &desc->data_dma);// 分配 usb 数据缓冲区 dma 地址

    //  给 input_dev 设置 drvData 
    input_set_drvdata(input_dev, desc);// drvData 是 可以用来存储一些自定义的数据

    // 设置 input 上报的 event 类型
    __set_bit(EV_KEY, input_dev->evbit);// 设置 input 能产生 按键事件

    //  鼠标按键
    // __set_bit(BTN_MOUSE, input_dev->keybit);// 设置 input 能产生 鼠标事件
    __set_bit(BTN_LEFT, input_dev->keybit);// 按键事件 ：设置 input 能产生
    __set_bit(BTN_RIGHT, input_dev->keybit);// 按键事件 ：设置 input 能产生 鼠标右键事件
    __set_bit(BTN_MIDDLE, input_dev->keybit);// 按键事件： 设置 input 能产生 鼠标中键事件

    // 设置鼠标滚轮
    __set_bit(EV_REL, input_dev->evbit); // 支持 相对坐 标事件

    __set_bit(REL_X, input_dev->relbit);//相对位移下：  设置 input 能产生 鼠标 X 轴 事件
    __set_bit(REL_Y, input_dev->relbit);//相对位移下：  设置 input 能产生 鼠标 Y 轴 事件
    __set_bit(REL_WHEEL, input_dev->relbit);//相对位移下：  设置 input 能产生 鼠标 滚轮 事件



    __set_bit(KEY_L, input_dev->keybit);// 设置 input 能产生 L 按键事件
    __set_bit(KEY_S, input_dev->keybit);// 设置 input 能产生 S 按键事件
    __set_bit(KEY_ENTER, input_dev->keybit);// 设置 input 能产生 ENTER 按键事件


    input_dev->open = usb_mouse_as_key_open; // 设置 input 的 open 函数； 这里面 注册 urp
    input_dev->close = usb_mouse_as_key_close; // 设置 input 的 close 函


    // 注册 input_dev
    error = input_register_device(input_dev);

    usb_set_intfdata(intf, input_dev);// 设置 usb 接口的 drvData; dev

    return 0;
}



static void usb_mouse_as_key_disconnect(struct usb_interface *intf)
{
    //  从 usb interface 里面 获取 inpu_dev
    struct input_dev* input_dev = usb_get_intfdata(intf);
    // 从 input_dev 里面 获取 usb_mouse_as_key_desc 结构体
    struct usb_mouse_as_key_desc* desc = input_get_drvdata(input_dev);
    
    
    usb_free_coherent(desc->dev, desc->maxp, desc->data_buffer, desc->data_dma);// 释放 usb 数据缓冲区
    kfree(desc);// 释放 usb_mouse_as_key_desc 结构体


    input_unregister_device(input_dev);// 注销 input_dev
    usb_set_intfdata(intf, NULL);// 设置 usb 接口的 drvData 为 NULL

}


static struct usb_device_id usb_mouse_as_key_id_table[] ={
    // 
    {
        // USB_INTERFACE_INFO 是 里面比较 bInterfaceClass 、bInterfaceSubClass、bInterfaceProtocol 这三个是否为 传入的 
        // bInterfaceClass = USB_INTERFACE_CLASS_HID, // HID
        // bInterfaceSubClass = USB_INTERFACE_SUBCLASS_BOOT, // Boot Protocol
        // bInterfaceProtocol = USB_INTERFACE_PROTOCOL_MOUSE // Mouse
        USB_INTERFACE_INFO(
            USB_INTERFACE_CLASS_HID,// HID
            USB_INTERFACE_SUBCLASS_BOOT, // Boot Protocol
            USB_INTERFACE_PROTOCOL_MOUSE // Mouse
        ),
        .driver_info = (kernel_ulong_t)"it is a mouse",// driver_info 是 传入一些自定义信息，当 上面的 判断通过的时候 可以  再 probe 的时候 拿到这些信息去使用 
    },
    {}
};

static struct usb_driver usb_mouse_as_key_driver = {
    .name = "usb_mouse_as_key",
    .probe = usb_mouse_as_key_probe,
    .disconnect = usb_mouse_as_key_disconnect,
    .id_table = usb_mouse_as_key_id_table,
};

static int __init usb_mouse_as_key__init(void)
{
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    return usb_register(&usb_mouse_as_key_driver);
}


static void __exit usb_mouse_as_key__init_exit(void)
{ 
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    usb_deregister(&usb_mouse_as_key_driver);
}

module_init(usb_mouse_as_key__init);
module_exit(usb_mouse_as_key__init_exit);

MODULE_LICENSE("GPL");
