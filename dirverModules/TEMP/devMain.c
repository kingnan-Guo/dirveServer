#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>


#define DEVICE_NAME "my_test_device"
// static int major; // 设备主号
static unsigned char my_buffer[1024]; // 内核空间缓冲区

static struct class *my_test_device_class;

static struct cdev my_test_device_cdev;
static dev_t dev;

static int my_test_device_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}

static ssize_t my_test_device_read(struct file *file, char __user *buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
    // 读取数据到用户空间
    // int copy_to_user(void __user *dst, const void *src, unsigned size)
    // unsigned long len = len > 100 ? 100 : len;
    // copy_to_user(buffer, my_buffer, len);

    // 检查返回值
    if (copy_to_user(buffer, my_buffer, len)) {
        printk(KERN_WARNING "Failed to copy data to user space.\n");
        return -EFAULT;  // 返回错误码
    }


    return len;  // 返回读取的字节数
}

static ssize_t my_test_device_write(struct file *file, const char __user *buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);

    if (len > sizeof(my_buffer)) {
        len = sizeof(my_buffer);  // 限制写入长度
    }

    // 从用户空间读取数据到内核空间
    // copy_from_user(my_buffer, buffer, len);
    // 检查返回值
    if (copy_from_user(my_buffer, buffer, len)) {
        printk(KERN_WARNING "Failed to copy data from user space.\n");
        return -EFAULT;  // 返回错误码
    }


    return len;  // 返回写入的数据字节数
}

static int my_test_device_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}

static struct file_operations my_test_device_fops = {
    .owner = THIS_MODULE,
    .read = my_test_device_read,
    .write = my_test_device_write,
    .open = my_test_device_open,
    .release = my_test_device_release
};

// 模块加载时执行的函数
static int __init my_test_device_init(void) {
    int ret;
    // dev_t dev;

    /**
     * 1. 分配设备号
     * 2. 注册设备号
     * 3. 创建设备类
     * 4. 创建设备节点
     * 5. 注册设备驱动
     * 
    */

   /**
    * 申请一个 主次设备号的空间
    * alloc_chrdev_region
    * params:
    *      1. dev_t *: 设备号
    *      2. unsigned int: 从哪个主设备号开始分配
    *      3. unsigned int: 分配多少个设备号
    *      4. const char *: 设备名称
    * 
    *  返回值: 0 成功，负数失败
    * 
   */

    ret = alloc_chrdev_region(&dev, 0, 2, DEVICE_NAME);
    if(ret < 0){
        printk(KERN_ALERT "alloc_chrdev_region failed \n");
        return -EINVAL;
    }
    // 通过 MAJOR 宏从 devt 中提取出主设备号
    // major = MAJOR(devt);
    // 通过 MINOR 宏从 devt 中提取出次设备号
    // minor = MINOR(devt);



    //初始化 cdev ，让 cdev 跟 file_operations 关联起来
    // void cdev_init(struct cdev *, const struct file_operations *);
    cdev_init(&my_test_device_cdev, &my_test_device_fops);

    // 添加 cdev 到内核
    // int cdev_add(struct cdev *, dev_t, unsigned);
    // unsigned 占用几个 设备号
    ret = cdev_add(&my_test_device_cdev, dev, 2);

    if(ret < 0){
        printk(KERN_ALERT "cdev_add failed \n");
		return -EINVAL;
    }

    // class_create
    // 创建设备类
    // struct class *class_create(struct module *owner, const char *name);
    my_test_device_class = class_create( "my_test_device_class");
    if(IS_ERR(my_test_device_class)){
        // pr_err("failed to allocate class\n");
        printk("failed to allocate class\n");
        return PTR_ERR(my_test_device_class);
    }

    /**
     * 创建设备节点
     *  params:
     *      1. class: 设备类
     *      2. dev_t: 设备号
     *      3. const char *: 设备节点名称
     *      4. dev_t: 设备号            MKDEV(major, 0) 是个整数
     *      5. const char *: 设备节点名称
     *    *      6. void *: 私有数据
     * 
     *  会去 创建一个 设备节点 /dev/my_test_device
     */
    device_create(my_test_device_class, NULL, dev, NULL, DEVICE_NAME);






    
    // major = register_chrdev(0, DEVICE_NAME, &my_test_device_fops);
    // if (major < 0) {
    //     printk(KERN_ALERT "Failed to register character device.\n");
    //     return major;
    // }

    // // 创建设备类
    // // 为 THIS_MODULE  模块创建一个类
    // my_test_device_class = class_create(THIS_MODULE, "my_test_device_class");
    // if(IS_ERR(my_test_device_class)){
    //     // pr_err("failed to allocate class\n");
    //     printk("failed to allocate class\n");
    //     return PTR_ERR(my_test_device_class);
    // }
    // /**
    //  * 创建设备节点
    //  *  params:
    //  *      1. class: 设备类
    //  *      2. dev_t: 设备号
    //  *      3. const char *: 设备节点名称
    //  *      4. dev_t: 设备号            MKDEV(major, 0) 是个整数
    //  *      5. const char *: 设备节点名称
    //  *    *      6. void *: 私有数据
    //  * 
    //  *  会去 创建一个 设备节点 /dev/my_test_device
    //  */
    // device_create(my_test_device_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);


    // printk(KERN_INFO "my_test_device registered with major number %d.\n", major);



    return 0;
}

// 模块卸载时执行的函数
static void __exit my_test_device_exit(void) {

    // 打印 my_buffer
    printk(KERN_INFO "my_buffer: %s\n", my_buffer);

    // 销毁类下面的 设备节点
    // device_destroy(my_test_device_class, MKDEV(major, 0));
    device_destroy(my_test_device_class, dev);
    // 销毁类
    class_destroy(my_test_device_class);

    
    // unregister_chrdev(major, DEVICE_NAME);
    // 删除 cdev
    cdev_del(&my_test_device_cdev);
    // 释放设备号 释放 malloc 的区域
    unregister_chrdev_region(dev, 2);



    printk(KERN_INFO "my_test_device unregistered.\n");
}

// 定义模块的加载和卸载函数
module_init(my_test_device_init);
module_exit(my_test_device_exit);

MODULE_LICENSE("GPL");