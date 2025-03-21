#include <linux/module.h>

#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/kmod.h>
#include <linux/gfp.h>

#include <linux/gpio/consumer.h>
#include <linux/of.h>
#include <linux/platform_device.h>


#define _NAME "_diver"

// #define _NUM 2

static int major; // 设备主号

static struct class *_class;
static struct gpio_desc *_gpio;


static int _open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);


    // 干脆移除 gpiod_direction_output，因为在 probe 中已经设置过方向
    // gpiod_direction_output(_gpio, 0); // 这段要去掉的 原因 是 已经在初始化的 时候设置过方向，如果 ./main /dev/_dirver_0 on  运行时  会先调 open 然后 吧 输出设置成0 ，所以一直是 0

    // 第二种方式 (已经尝试 可以用)
    // 修改 _open 函数
    // 避免每次打开时重置 GPIO 状态：
    // 只设置方向，不强制设置值
    // gpiod_direction_output(_gpio, gpiod_get_value(_gpio)); // 保持当前值




    return 0;
}

static ssize_t _read(struct file *file, char __user *buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
    
    char level = '0';
    if(gpiod_get_value(_gpio)){
        level = '1';
    }
    if (copy_to_user(buffer, &level, 1)) {
        printk(KERN_ERR "Failed to copy GPIO status to user\n");
        return -EFAULT;
    }
    return 1;  // 返回读取的字节数
}

// 写入设备
// write(fd,  &val, sizeof(val));
static ssize_t _write(struct file *file, const char __user *buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);


    // int err;
    char status;
    int value;

    // err = copy_from_user(&status, buffer, 1);
    // printk(" status  %s", status);
    // // 根据次设备号 和 status 控制 XXX
    // gpiod_set_value(_gpio, status);
    if (copy_from_user(&status, buffer, 1)){
        return -EFAULT;
    }
        

    // 转换 ASCII 字符到数值
    if (status == '0') {
        value = 0;
    } else if (status == '1') {
        value = 1;
    } else {
        printk(KERN_ERR "Invalid input: %c\n", status);
        return -EINVAL;
    }

    gpiod_set_value(_gpio, value);

    return len;  // 返回写入的数据字节数
}

static int _release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}

static struct file_operations _fops = {
    .owner = THIS_MODULE,
    .read = _read,
    .write = _write,
    .open = _open,
    .release = _release
};






/// @brief 
/// @param pdev 
/// @return 
static int _chip_gpio_probe(struct platform_device *pdev){
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
    // int err;
    // 1、 设备树 中定义 有 led-gpio=<>
    _gpio = gpiod_get(&pdev->dev, "output_1", 0);
    if(IS_ERR(_gpio)){
        dev_err(&pdev->dev, "Failed to get GPIO for output_1 \n");
        return PTR_ERR(_gpio);
    }

    // 2、 注册  file_operations _fops
    major = register_chrdev(0, _NAME, &_fops);
    if (major < 0) {
        printk(KERN_ALERT "Failed to register character device.\n");
        return major;
    }

    _class = class_create("_diver_class");
    if(IS_ERR(_class)){
        printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

		unregister_chrdev(major, _NAME);
		gpiod_put(_gpio);

        printk("failed to allocate class\n");
        return PTR_ERR(_class);
    }



    char device_name_buf[30];
    int minor = 0;
    snprintf(device_name_buf, sizeof(device_name_buf), "%s_%d", _NAME, minor);
    device_create(_class, NULL, MKDEV(major, minor), NULL, device_name_buf);//创建 文件系统 的设备节点; 应用程序 通过文件系统的设备 节点 访问 硬件  
    // device_create(_class, NULL, MKDEV(major, 0), NULL, "100ask_led%d", 0);

    return 0;
}

/// @brief 清除驱动
/// @param pdev 
/// @return 
static int _chip_gpio_remove(struct platform_device *pdev){
    device_destroy(_class, MKDEV(major, 0)); // 销毁设备
    class_destroy(_class);// 销毁 class
    unregister_chrdev(major, _NAME);// 卸载驱动 注销字符设备
    // 清除 gpio
    gpiod_put(_gpio);
    return 0;
}



static const struct of_device_id _chip_gpio_of_match[] = {
    { .compatible = "my_outputs,my_drv" }// my_board_device,my_drv 这个值 在 dtb 上配好的 
};

static struct platform_driver _chip_gpio_dirver = {
    .probe = _chip_gpio_probe,
    .remove = _chip_gpio_remove,
    .driver = {
        .name = "my_outputs",// 名字 用来跟 platform_device 配对 如果配对成功
        .of_match_table = _chip_gpio_of_match
    },
};







// 模块加载时执行的函数 
static int __init device_init(void) {
    printk(KERN_INFO "========= %s %s %d  ========= \n", __FILE__, __FUNCTION__, __LINE__);

    // 入口函数里 注册 _chip_gpio_dirver 结构体
    int err;
    err = platform_driver_register(&_chip_gpio_dirver);// 注册 _chip_gpio_dirver 会调用  probe
    return err;
}

// 模块卸载时执行的函数
static void __exit device_exit(void) {
    platform_driver_unregister(&_chip_gpio_dirver);// 会调用 remove
    printk(KERN_INFO "Entering _exit\n");
}

// 定义模块的加载和卸载函数
module_init(device_init);
module_exit(device_exit);

MODULE_LICENSE("GPL");