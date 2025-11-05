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
#include <linux/pwm.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#define _NAME "my_pwm"
#define MAX_PWM_DEVICES 2
#define MINORMASK 1   // 2 个 PWM 通道
// #define _NUM 2

static int major; // 设备主号

static struct class *_class;



struct pwm_device *my_pwm_device;

struct my_pwm_device {
    struct pwm_device *pwm;
    struct device *dev;
    struct cdev cdev;// 字符设备
    dev_t dev_no;// 设备号
    struct class *class;
    struct device *device;
};

static dev_t _dev_no;
static struct my_pwm_device *pwm_devices[MAX_PWM_DEVICES];



static struct my_pwm_device *my_pwm_dev;

static int _open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
    unsigned int minor = iminor(inode);
    struct pwm_state pstate;
    struct my_pwm_device *my_pwd = pwm_devices[minor];

    // 
    file->private_data = my_pwd;// 保存 私有数据 指针

    // 
    pwm_get_state(my_pwd->pwm, &pstate);
    pstate.enabled = true;
    pwm_apply_might_sleep(my_pwd->pwm, &pstate);


    // // 配置 pwm
    // pwm_config(my_pwm_device, 500000, 1000000); // 占空比 50%
    // // 设置极性
    // pwm_set_polarity(my_pwm_device, PWM_POLARITY_NORMAL);
    // // 启动 pwm
    // pwm_enable(my_pwm_device);

    return 0;
}

static ssize_t _read(struct file *file, char __user *buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
    
    // char level = '0';

    return 1;  // 返回读取的字节数
}

// 写入设备
// write(fd,  &val, sizeof(val));
static ssize_t _write(struct file *file, const char __user *buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);

    struct my_pwm_device *my_pwd = file->private_data;

    struct pwm_state pstate;
    pwm_get_state(my_pwd->pwm, &pstate);


    char data_buffer[10] = {0};
    int err = copy_from_user(&data_buffer, buffer, len);// 获取用户空间数据

    data_buffer[len] = '\0'; // 确保字符串结束
    printk("data_buffer %s \n", data_buffer);

    int duty_percent;
    sscanf(data_buffer, "%d", &duty_percent); // 解析字符串为整数

    printk("duty_percent %d \n", duty_percent);
    // 根据次设备号 和 status 控制 XXX
    pstate.duty_cycle = duty_percent * pstate.period / 100; // 设置占空比
    pwm_apply_might_sleep(my_pwd->pwm, &pstate);

    // int err;
    // char status;
    // int value;

    // // err = copy_from_user(&status, buffer, 1);
    // // printk(" status  %s", status);
    // // // 根据次设备号 和 status 控制 XXX
    // if (copy_from_user(&status, buffer, 1)){
    //     return -EFAULT;
    // }

    // pwm_config(my_pwm_device, 500000+status*500000/45, 1000000); // 占空比 50%

    return len;  // 返回写入的数据字节数
}

static int _release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);


    // struct my_pwm_device *my_pwd = file->private_data;

    // struct pwm_state pstate;
    // pwm_get_state(my_pwd->pwm, &pstate);
    // pstate.enabled = false;
    // pwm_apply_might_sleep(my_pwd->pwm, &pstate);


    return 0;
}

static struct file_operations _fops = {
    .owner = THIS_MODULE,
    .read = _read,
    .write = _write,
    .open = _open,
    .release = _release
};





static atomic_t next_index = ATOMIC_INIT(0);  // For assigning minor
/// @brief 
/// @param pdev 
/// @return 
static int my_pwm_probe(struct platform_device *pdev){
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
    struct device_node *np = pdev->dev.of_node;// 获取 设备树 节点


    int err;
    struct device_node *child;
    int index = 0;


    // 1、 获取 pwm 设备

    // 分配实例
    // devm_kzalloc(&pdev->dev, sizeof(struct pwm_device), GFP_KERNEL);
    // pwm_get(&pdev->dev, "my_pwm");

    // for_each_child_of_node(np, child) {
    //     struct my_pwm_device *my_pwm_dev;
    //     struct pwm_state pstate; // 获取 pwm 通道 周期 极性
    //     // printk("child name: %s\n", child->name); // 打印子节点名字
    //     // // printk("child compatible: %s\n", child->compatible); // 打印子z z节点 compatible 属性
    //     // printk("child of_node_full_name : %s\n" , of_node_full_name(child));

    //     // my_pwm_dev = devm_kzalloc(&pdev->dev, sizeof(*my_pwm_dev), GFP_KERNEL);


    //     if (!of_device_is_compatible(child, "my_pwm,my_drv")) {
    //         printk(KERN_INFO "Skipping non-matching child: %s\n", child->name);
    //         continue;
    //     }

    //     printk(KERN_INFO "Processing child node: %s\n", of_node_full_name(child));

    //     my_pwm_dev = devm_kzalloc(&pdev->dev, sizeof(*my_pwm_dev), GFP_KERNEL);
    //     if (!my_pwm_dev) {
    //         dev_err(&pdev->dev, "Failed to allocate memory for PWM device\n");
    //         of_node_put(child);
    //         return -ENOMEM;
    //     }


    //     my_pwm_dev->dev = &pdev->dev;// 保存 设备 结构体 指针
    //     my_pwm_dev->pwm = devm_pwm_get(&pdev->dev, of_node_full_name(child));


    //     // huoqu  pwm  通道 周期 极性
    //     pwm_get_state(my_pwm_dev->pwm, &pstate);

    //     unsigned int channel = my_pwm_dev->pwm->hwpwm;
    //     // dev_info(&pdev->dev, "PWM Device: %s\n", of_node_full_name(child));
    //     // printk("pwm period: %llu\n", pstate.period);
    //     // printk("pwm polarity = PWM_POLARITY_NORMAL : %d\n", pstate.polarity);
    //     // printk("pwm duty_cycle: %llu\n", pstate.duty_cycle);
    //     // dev_info(dev, "  Enabled: %s\n", state.enabled ? "Yes" : "No");


    //     dev_info(&pdev->dev, "PWM Device: %s\n", of_node_full_name(child));
    //     dev_info(&pdev->dev, "  Controller: %s\n", my_pwm_dev->pwm->chip->dev->of_node->name);
    //     dev_info(&pdev->dev, "  Channel: %u\n", my_pwm_dev->pwm->hwpwm);
    //     dev_info(&pdev->dev, "  Period: %llu ns (%llu ms)\n", pstate.period, pstate.period / 1000000);
    //     dev_info(&pdev->dev, "  Polarity: %s\n", pstate.polarity == PWM_POLARITY_NORMAL ? "Normal" : "Inversed");

    //     pstate.duty_cycle = 0;// 默认占空不 为 0
    //     pstate.enabled = true;// 使能 pwm

    //     pwm_apply_might_sleep(my_pwm_dev->pwm, &pstate);
        

    //     // 注册字符设备
    //     // alloc_chrdev_region();
        

    // }

    

    const char *pwm_name;
    struct pwm_state pstate; // 获取 pwm 通道 周期 极性

 
    printk(KERN_INFO "%s %s %d: Probe called for device %s\n", __FILE__, __FUNCTION__, __LINE__, pdev->name);
    if (!np) {
        dev_err(&pdev->dev, "No device tree node found\n");
        return -ENODEV;
    }
    printk(KERN_INFO "Device tree node: %s\n", np->full_name);

    my_pwm_dev = devm_kzalloc(&pdev->dev, sizeof(*my_pwm_dev), GFP_KERNEL);
    if (!my_pwm_dev) {
        dev_err(&pdev->dev, "Failed to allocate memory for PWM device\n");
        return -ENOMEM;
    }

    my_pwm_dev->dev = &pdev->dev;
    err = of_property_read_string(np, "pwm-names", &pwm_name);
    if (err) {
        dev_err(&pdev->dev, "Failed to read pwm-names\n");
        return err;
    }

    my_pwm_dev->pwm = devm_pwm_get(&pdev->dev, pwm_name);
    if (IS_ERR(my_pwm_dev->pwm)) {
        dev_err(&pdev->dev, "Failed to get PWM for %s: %ld\n", np->full_name, PTR_ERR(my_pwm_dev->pwm));
        return PTR_ERR(my_pwm_dev->pwm);
    }

    // 关键修复：强制禁用 PWM 以重置状态，确保第二次加载时生效
    pwm_disable(my_pwm_dev->pwm);

    pwm_get_state(my_pwm_dev->pwm, &pstate);
    dev_info(&pdev->dev, "PWM Device: %s, Channel: %u, Period: %llu ns\n", np->full_name, my_pwm_dev->pwm->hwpwm, pstate.period);
    dev_info(&pdev->dev, "Polarity: %s, Enabled: %s\n", pstate.polarity == PWM_POLARITY_NORMAL ? "Normal" : "Inversed", pstate.enabled ? "Yes" : "No");

    // pstate.duty_cycle = 0;
    pstate.period = 20000000;         // ✅ 设置周期 20ms = 20,000,000ns
    pstate.duty_cycle = 5000000;//10000000;     // ✅ 设置占空比 50% = 10,000,000ns
    pstate.polarity = PWM_POLARITY_NORMAL;
    pstate.enabled = true;
    err = pwm_apply_might_sleep(my_pwm_dev->pwm, &pstate);
    if (err) {
        dev_err(&pdev->dev, "Failed to apply PWM state: %d\n", err);
        return err;
    }





    // //  开始注册 字符设备
    //  err = alloc_chrdev_region(&_dev_no, 0, 1, _NAME);// 分配设备号, 存放在 my_input_dev->dev_no 中 
    

    dev_info(&pdev->dev, "Allocated major number: %d\n", major);
     // 
    index = atomic_inc_return(&next_index) - 1;// 获取次设备号； atomic_inc_return 把 next_index 的值 加 1
    dev_info(&pdev->dev, "Assigned minor number: %d\n", index);
    my_pwm_dev->dev_no = MKDEV(major, index);
    dev_info(&pdev->dev, "Device number: %d\n", my_pwm_dev->dev_no);


    cdev_init(&my_pwm_dev->cdev, &_fops);// 初始化 cdev 结构体
    err = cdev_add(&my_pwm_dev->cdev, my_pwm_dev->dev_no, 1);// 添加 cdev 到内核


    // // 2、 注册  file_operations _fops
    // major = register_chrdev(0, _NAME, &_fops);
    // if (major < 0) {
    //     printk(KERN_ALERT "Failed to register character device.\n");
    //     return major;
    // }

    // _class = class_create("_diver_class");
    // if(IS_ERR(_class)){
    //     printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

	// 	unregister_chrdev(major, _NAME);

    //     printk("failed to allocate class\n");
    //     return PTR_ERR(_class);
    // }




    char device_name_buf[30];
    int minor = index;
    snprintf(device_name_buf, sizeof(device_name_buf), "%s_%d", _NAME, minor);
    // my_pwm_dev->device  = device_create(_class, NULL,  my_pwm_dev->dev_no, NULL, device_name_buf);//创建 文件系统 的设备节点; 应用程序 通过文件系统的设备 节点 访问 硬件  
    // device_create(_class, NULL, MKDEV(major, 0), NULL, "100ask_led%d", 0);

    my_pwm_dev->device = device_create(_class, &pdev->dev, my_pwm_dev->dev_no, NULL,  "%s_%d", _NAME, index);

    // // 获取 pwm 设备
    // my_pwm_device = devm_pwm_get(&pdev->dev, "my_pwm");


    platform_set_drvdata(pdev, my_pwm_dev);// 将 my_pwm_dev 存入 platform_device 的 drvdata 字段


    pwm_devices[minor] = my_pwm_dev;

    return 0;
}

/// @brief 清除驱动
/// @param pdev 
/// @return 
static int my_pwm_remove(struct platform_device *pdev){
    // device_destroy(_class, MKDEV(major, 0)); // 销毁设备
    // class_destroy(_class);// 销毁 class
    // unregister_chrdev(major, _NAME);// 卸载驱动 注销字符设备
    struct my_pwm_device *my_pwm_dev = platform_get_drvdata(pdev);
    int minor = MINOR(my_pwm_dev->dev_no);

    pwm_disable(my_pwm_dev->pwm);// 关键修复：强制禁用 PWM 以重置状态，确保第二次加载时生效
    dev_info(&pdev->dev, "my_pwm_remove Device number: %d\n", my_pwm_dev->dev_no);
    // if (!my_pwm_dev) return 0;  // 防御性检查
    device_destroy(_class, my_pwm_dev->dev_no);
    cdev_del(&my_pwm_dev->cdev);

    pwm_devices[minor] = NULL;

    return 0;
}



static const struct of_device_id my_pwm_of_match[] = {
    // { .compatible = "my_pwm,my_drv", },// my_board_device,my_drv 这个值 在 dtb 上配好的 ,
    { .compatible = "my_pwm,my_dev" },
    // { .compatible = "brcm,bcm2835-pwm" }, // 绑定到 PWM 控制器
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, my_pwm_of_match);

static struct platform_driver my_pwm_dirver = {
    .probe = my_pwm_probe,
    .remove = my_pwm_remove,
    .driver = {
        .name = "my_pwm",// 名字 用来跟 platform_device 配对 如果配对成功
        .of_match_table = my_pwm_of_match
    },
};







// 模块加载时执行的函数 
static int __init device_init(void) {
    printk(KERN_INFO "========= %s %s %d  ========= \n", __FILE__, __FUNCTION__, __LINE__);
    _class = class_create("my_pwm_diver_class");
    //  开始注册 字符设备
    int err = alloc_chrdev_region(&_dev_no, 0, MAX_PWM_DEVICES, _NAME);// 分配设备号, 存放在 my_input_dev->dev_no 中 
    
    major = MAJOR(_dev_no);// 提取 主设备号

    // 入口函数里 注册 my_pwm_dirver 结构体
    err = platform_driver_register(&my_pwm_dirver);// 注册 my_pwm_dirver 会调用  probe
    return err;
}

// 模块卸载时执行的函数
static void __exit device_exit(void) {



    // 2. 注销字符设备区域
    // unregister_chrdev_region(MKDEV(major, 0), MINORMASK + 1);
    unregister_chrdev_region(_dev_no, MAX_PWM_DEVICES);

    platform_driver_unregister(&my_pwm_dirver);// 会调用 remove

    if (_class) {
        class_destroy(_class);
        _class = NULL;
    }

    printk(KERN_INFO "Entering _exit\n");
}

// 定义模块的加载和卸载函数
module_init(device_init);
module_exit(device_exit);

MODULE_LICENSE("GPL");