#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb.h>// 重点
#include <linux/input.h>
#include <linux/hid.h>


static int usb_mouse_as_key_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
    // 1 记录设备信息 intf 要发起 信息传输的时候 需要用到
    // 2 分配 / 设置 / 注册 input devicve
    // 2.1 input 中 能产生 哪类事件
    // 2.2 input 中 能产生 这类 按键 里的那些事件 ： 例如 ： L/S/Enter
    // 2.3 设置 input 的open函数 
    // 2.4 open 函数 里面  提交URB 提交 URB
    // 2.5 URB 回调函数， 解析数据，上报 输入事件 input_event， 就是解析 左键右键 哪个被按下或松开
    return 0;
}



static void usb_mouse_as_key_disconnect(struct usb_interface *intf)
{
    
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
}

static struct usb_driver usb_mouse_as_key_driver = {
    .name = "usb_mouse_as_key",
    .probe = usb_mouse_as_key_probe,
    .disconnect = usb_mouse_as_key_disconnect,
    .id_table = usb_mouse_as_key_id_table,
}

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

moudle_init(usb_mouse_as_key__init);
module_exit(usb_mouse_as_key__init_exit);

MODULE_LICENSE("GPL");
