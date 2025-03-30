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
#include <asm/pgtable.h>
#include <linux/mm.h>
#include <linux/slab.h>

static int major;
#define DEVICE_NAME "my_mmap"

static struct class *my_mmap_class;


static char *kernel_buffer;// 在入口函数 分配
static int buffer_size = 1024 * 8;

#define MIN(a, b) (a < b ? a : b)

static int my_open(struct inode *node, struct file *file) {
    printk(KERN_INFO "%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}

static ssize_t my_read(struct file *file, char __user *buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

    int err = copy_to_user(buffer, kernel_buffer, MIN(buffer_size, len));
    // kernel_buffer
    printk(KERN_INFO "read %s\n", kernel_buffer);
    if(err){
        return -EFAULT;
    }
    return  MIN(buffer_size, len);
}

static ssize_t my_write(struct file *file, const char __user *buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    int err = copy_from_user(kernel_buffer, buffer, MIN(1024, len));
    // kernel_buffer
    printk(KERN_INFO "write %s\n", kernel_buffer);
    if(err){
        return -EFAULT;
    }
    return  MIN(1024, len);
}

static int my_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}

// my_mmap
static int my_mmap(struct file *file, struct vm_area_struct *vma) {
    
    // 1 获得 物理地址 P_addr； 使用 virt_to_phys 函数 虚拟地址 转换 物理地址
    unsigned long phy = virt_to_phys(kernel_buffer);// 把 kernel_buffer  转换为 物理地址



   
    /**
     *  /linux-rpi-6.6.y/arch/arm/include/asm/pgtable.h
     * #define pgprot_writecombine(prot) __pgprot_modify(prot, L_PTE_MT_MASK, L_PTE_MT_BUFFERABLE)
     * 
     *         /linux-rpi-6.6.y/arch/arm/include/asm/pgtable-2level.h
     *         #define L_PTE_MT_MASK		(_AT(pteval_t, 0x0f) << 2)
     *         #define L_PTE_MT_BUFFERABLE	(_AT(pteval_t, 0x01) << 2)	0001
     * 
     *      所以 传入 的值  是  0 1
     *      
     *      查找 手册  页属性  有多少种
     * 是否使用 cache       是否使用 buffer         说明
     * 0                   0                        Non-cached， Non-buffered (NCNB) 读、写都直达 外设硬件； 对应 #define pgprot_noncached(prot) 
     * 0                   1                        Non-cached， buffered (NCB) 读、写都直达 外设硬件， 写操作通过 buffer实现， CPU 不等到写操作完成，CPU 会马上执行下一条指令； 对应 #define pgprot_writecombine(prot)
     * 1                   0                        cached， write-through mode (WT) 写通，读 ：cahe hit 时 从 cache 读数据， cache miss 时 已经入一行数据到 cache，写： 通过 buffer 实现，CPU 不等待写操作完成，CPU 会马上执行下一条指令； 对应 #define pgprot_stronglyordered(prot)
     * 1                   1                        cached， write-back mode (WB) 写回，读：cache hit 时 从 cache 读数据， cache miss 时 已经入一行数据到 cache，写：通过 buffer 实现， cache hit 时 新数据不会到达硬件，CPU 不等待写操作完成，CPU 会马上执行下一条指令； 对应 #define pgprot_device(prot)
     * 
     * 
     * 
     * 
     *  
     */
    vma->vm_page_prot = pgprot_writecombine(vma->vm_page_prot);// 

     // 2、获得 虚拟地址 V_addr
    // vma 里面 是有 虚拟地址的
    /**
     * int remap_pfn_range(struct vm_area_struct *vma, unsigned long addr, unsigned long pfn, unsigned long size, pgprot_t prot)
     * int remap_pfn_range(struct vm_area_struct *, unsigned long addr, unsigned long pfn, unsigned long size, pgprot_t);
     * 参数
     *      struct vm_area_struct *vma,// 虚拟内存区域
     *      unsigned long addr,// 虚拟地址
     *      unsigned long pfn,// 物理地址 ： phy / PAGE_SIZE 是 phy / 4096 ，也是 phy >> PAGE_SHIFT， 因为 有的 内核会修改  PAGE_SIZE 页的大小
     *      unsigned long size,// 
     *      pgprot_t prot
     * 
     *  
     */
	if(remap_pfn_range(vma, vma->vm_start, phy >> PAGE_SHIFT, vma->vm_end - vma->vm_start, vma->vm_page_prot)){
        printk(KERN_ERR "Failed to remap_pfn_range\n");
        return -EAGAIN;
    }

    // 3、设置属性  使用 cache, buffer

    // 4、map 映射

    return 0;
}

static struct file_operations my_fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .read = my_read,
    .write = my_write,
    .release = my_release,
    .mmap = my_mmap
};

static int __init my_mmap_init(void) {
    printk(KERN_INFO "%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);


    // 为  kernel_buffer 分配 内存; GFP_KERNEL 是 内核态分配内存的标志
    kernel_buffer = kmalloc(buffer_size, GFP_KERNEL);

    // 在使用private 的时候  先 测试写入 数据 这个时候写入的是 old 老数据
    strcpy(kernel_buffer, "old Data");



    major = register_chrdev(0, DEVICE_NAME, &my_fops);
    if (major < 0) {
        printk(KERN_ERR "Failed to register chrdev\n");
        return major;
    }

    my_mmap_class = class_create("my_mmap_class");
    if (IS_ERR(my_mmap_class)) {
        printk(KERN_ERR "Failed to create class\n");
        unregister_chrdev(major, DEVICE_NAME);
        return PTR_ERR(my_mmap_class);
    }

    device_create(my_mmap_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    printk(KERN_INFO "my_mmap initialized\n");
    return 0;
}

static void __exit my_mmap_exit(void) {
    printk(KERN_INFO "%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

    
    device_destroy(my_mmap_class, MKDEV(major, 0));
    class_destroy(my_mmap_class);
    unregister_chrdev(major, DEVICE_NAME);
    kfree(kernel_buffer);// 释放内存
    printk(KERN_INFO "my_mmap unregistered.\n");

}

module_init(my_mmap_init);
module_exit(my_mmap_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("kingnan Guo");
MODULE_DESCRIPTION("my_mmap driver for Raspberry Pi");
