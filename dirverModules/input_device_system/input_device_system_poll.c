#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/pm.h>
#include <linux/slab.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/gpio_keys.h>
#include <linux/workqueue.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/spinlock.h>
#include <linux/poll.h>
#include <linux/cdev.h>
#include <linux/time.h>
#include <linux/time64.h>
#include <dt-bindings/input/gpio-keys.h>
#include <linux/kobject.h>

#define DEVICE_NAME "my_input_device"
#define CLASS_NAME "my_input_device_class"
#define BUFFER_LEN 128

#define INPUT_IOC_MAGIC 'M'
#define IOCTL_RESET_COUNT _IOW(INPUT_IOC_MAGIC, 0x01, unsigned int)
#define IOCTL_GET_EVENT_PATH _IOR(INPUT_IOC_MAGIC, 0x05, char[256])

struct input_device_button_data {
    struct input_dev *input_device;
    struct gpio_desc *gpiod;
    int irq;
    unsigned int key_code;
    unsigned long press_count;
    unsigned long last_time;
};

struct input_device_data {
    struct input_dev *input_device;
    int n_buttons;
    struct input_device_button_data button_data[];
};

struct button_event {
    unsigned int button_idx;
    struct input_event input_ev;
    unsigned long press_count;
};

struct my_input_device {
    struct input_device_data *input_data;
    struct cdev cdev;
    dev_t dev_no;
    struct class *class;
    struct device *device;
    struct button_event event_buffer[BUFFER_LEN];
    spinlock_t buffer_lock;
    int read_index;
    int write_index;
    wait_queue_head_t wait_queue;
};

static struct my_input_device *my_input_dev;

/* 环形缓冲区操作 */
static int next_pos(int pos)
{
    return (pos + 1) % BUFFER_LEN;
}

static int is_buffer_empty(struct my_input_device *dev)
{
    return dev->read_index == dev->write_index;
}

static int is_buffer_full(struct my_input_device *dev)
{
    return dev->read_index == next_pos(dev->write_index);
}

static void push_event(struct my_input_device *dev, struct button_event *event)
{
    unsigned long flags;
    int was_empty;

    spin_lock_irqsave(&dev->buffer_lock, flags);
    was_empty = is_buffer_empty(dev);
    if (!is_buffer_full(dev)) {
        dev->event_buffer[dev->write_index] = *event;
        dev->write_index = next_pos(dev->write_index);
        if (was_empty)
            wake_up_interruptible(&dev->wait_queue);
    }
    spin_unlock_irqrestore(&dev->buffer_lock, flags);
}

static int get_event(struct my_input_device *dev, struct button_event *event)
{
    unsigned long flags;
    int ret = 0;

    spin_lock_irqsave(&dev->buffer_lock, flags);
    if (!is_buffer_empty(dev)) {
        *event = dev->event_buffer[dev->read_index];
        dev->read_index = next_pos(dev->read_index);
        ret = 1;
    }
    spin_unlock_irqrestore(&dev->buffer_lock, flags);
    return ret;
}

/* 中断处理函数 */
static irqreturn_t input_device_irq_handler(int irq, void *dev_id)
{
    struct input_device_button_data *button_data = dev_id;
    unsigned long now = jiffies;
    int state;
    struct button_event event;
    struct my_input_device *dev = my_input_dev;
    struct input_device_data *input_data = input_get_drvdata(button_data->input_device);
    struct timespec64 ts;

    if (time_before(now, button_data->last_time + msecs_to_jiffies(50)))
        return IRQ_HANDLED;
    button_data->last_time = now;

    state = gpiod_get_value(button_data->gpiod);
    if (state < 0) {
        dev_err(&button_data->input_device->dev, "无法读取 GPIO: %d\n", state);
        return IRQ_HANDLED;
    }

    if (state == 1)
        button_data->press_count++;

    event.button_idx = button_data - input_data->button_data;
    event.input_ev.type = EV_KEY;
    event.input_ev.code = button_data->key_code;
    event.input_ev.value = state;
    ktime_get_real_ts64(&ts);
    event.input_ev.time.tv_sec = ts.tv_sec;
    event.input_ev.time.tv_usec = ts.tv_nsec / 1000;
    event.press_count = button_data->press_count;
    push_event(dev, &event);

    input_event(button_data->input_device, EV_KEY, button_data->key_code, state);
    input_sync(button_data->input_device);
    dev_dbg(&button_data->input_device->dev, "中断 %d 触发, 按键码=%d, 状态=%d\n",
            irq, button_data->key_code, state);

    return IRQ_HANDLED;
}

/* 字符设备操作 */
static int my_input_open(struct inode *inode, struct file *file)
{
    file->private_data = my_input_dev;
    return 0;
}

static ssize_t my_input_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    struct my_input_device *dev = file->private_data;
    struct button_event event;
    char tmp[256];
    int len;

    if (file->f_flags & O_NONBLOCK) {
        if (!get_event(dev, &event))
            return -EAGAIN;
    } else {
        wait_event_interruptible(dev->wait_queue, !is_buffer_empty(dev));
        if (!get_event(dev, &event))
            return -EAGAIN;
    }

    len = snprintf(tmp, sizeof(tmp),
                   "按键 %u: 类型=%u, 代码=%u, 值=%d, 按下次数=%lu, 时间=%ld.%06ld\n",
                   event.button_idx, event.input_ev.type, event.input_ev.code,
                   event.input_ev.value, event.press_count,
                   (long)event.input_ev.time.tv_sec, event.input_ev.time.tv_usec);
    if (len > count)
        len = count;
    if (copy_to_user(buf, tmp, len))
        return -EFAULT;

    return len;
}

static ssize_t my_input_write(struct file *file, const char __user *buffer, size_t len, loff_t *offset)
{
    return -EINVAL;
}

static int my_input_release(struct inode *inode, struct file *file)
{
    return 0;
}

static unsigned int my_input_poll(struct file *file, struct poll_table_struct *wait)
{
    struct my_input_device *dev = file->private_data;
    poll_wait(file, &dev->wait_queue, wait);
    return is_buffer_empty(dev) ? 0 : POLLIN | POLLRDNORM;
}

static long my_input_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct my_input_device *dev = file->private_data;

    switch (cmd) {
    case IOCTL_RESET_COUNT:
        {
            unsigned int button_idx;
            if (copy_from_user(&button_idx, (void __user *)arg, sizeof(button_idx))) {
                dev_err(&dev->input_data->input_device->dev, "无法复制 button_idx\n");
                return -EFAULT;
            }
            if (button_idx >= dev->input_data->n_buttons) {
                dev_err(&dev->input_data->input_device->dev, "无效的 button_idx: %u\n", button_idx);
                return -EINVAL;
            }
            dev->input_data->button_data[button_idx].press_count = 0;
            return 0;
        }
    case IOCTL_GET_EVENT_PATH:
        {
            char event_path[256];
            snprintf(event_path, sizeof(event_path), "/dev/input/event%d",
                     MINOR(dev->input_data->input_device->dev.devt));
            if (copy_to_user((void __user *)arg, event_path, sizeof(event_path))) {
                dev_err(&dev->input_data->input_device->dev, "无法复制事件路径\n");
                return -EFAULT;
            }
            return 0;
        }
    default:
        dev_err(&dev->input_data->input_device->dev, "不支持的 ioctl 命令: 0x%x\n", cmd);
        return -ENOTTY;
    }
}

static const struct file_operations input_device_fops = {
    .owner = THIS_MODULE,
    .open = my_input_open,
    .read = my_input_read,
    .write = my_input_write,
    .release = my_input_release,
    .poll = my_input_poll,
    .unlocked_ioctl = my_input_ioctl,
};

/* 平台驱动 */
static int input_device_probe(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    struct input_device_data *input_data;
    struct device_node *node, *child;
    int err, i = 0;

    int nbuttons = device_get_child_node_count(dev);
    dev_info(dev, "按键数量: %d\n", nbuttons);

    input_data = devm_kzalloc(dev, struct_size(input_data, button_data, nbuttons), GFP_KERNEL);
    if (!input_data) {
        dev_err(dev, "无法分配 input_data\n");
        return -ENOMEM;
    }
    input_data->n_buttons = nbuttons;

    my_input_dev = devm_kzalloc(dev, sizeof(*my_input_dev), GFP_KERNEL);
    if (!my_input_dev) {
        dev_err(dev, "无法分配 my_input_dev\n");
        return -ENOMEM;
    }
    my_input_dev->input_data = input_data;

    spin_lock_init(&my_input_dev->buffer_lock);
    init_waitqueue_head(&my_input_dev->wait_queue);

    err = alloc_chrdev_region(&my_input_dev->dev_no, 0, 1, DEVICE_NAME);
    if (err < 0) {
        dev_err(dev, "无法分配设备号\n");
        return err;
    }

    cdev_init(&my_input_dev->cdev, &input_device_fops);
    my_input_dev->cdev.owner = THIS_MODULE;
    err = cdev_add(&my_input_dev->cdev, my_input_dev->dev_no, 1);
    if (err) {
        dev_err(dev, "无法添加 cdev: %d\n", err);
        unregister_chrdev_region(my_input_dev->dev_no, 1);
        return err;
    }

    my_input_dev->class = class_create(DEVICE_NAME);
    if (IS_ERR(my_input_dev->class)) {
        dev_err(dev, "无法创建类\n");
        cdev_del(&my_input_dev->cdev);
        unregister_chrdev_region(my_input_dev->dev_no, 1);
        return PTR_ERR(my_input_dev->class);
    }

    my_input_dev->device = device_create(my_input_dev->class, dev, my_input_dev->dev_no, NULL, DEVICE_NAME);
    if (IS_ERR(my_input_dev->device)) {
        dev_err(dev, "无法创建设备\n");
        class_destroy(my_input_dev->class);
        cdev_del(&my_input_dev->cdev);
        unregister_chrdev_region(my_input_dev->dev_no, 1);
        return PTR_ERR(my_input_dev->device);
    }

    input_data->input_device = devm_input_allocate_device(dev);
    if (!input_data->input_device) {
        dev_err(dev, "无法分配输入设备\n");
        return -ENOMEM;
    }

    input_data->input_device->name = "dirve-input-device";
    input_data->input_device->phys = "input_device";
    input_data->input_device->dev.parent = dev;
    input_data->input_device->id.bustype = BUS_HOST;
    input_data->input_device->id.vendor = 0x0001;
    input_data->input_device->id.product = 0x0001;
    input_data->input_device->id.version = 0x0100;

    input_set_drvdata(input_data->input_device, input_data);
    __set_bit(EV_KEY, input_data->input_device->evbit);

    node = dev->of_node;
    for_each_child_of_node(node, child) {
        input_data->button_data[i].input_device = input_data->input_device;

        input_data->button_data[i].gpiod = devm_fwnode_gpiod_get(dev, &child->fwnode, NULL, GPIOD_IN, "button");
        if (IS_ERR(input_data->button_data[i].gpiod)) {
            err = PTR_ERR(input_data->button_data[i].gpiod);
            dev_err(dev, "无法获取 %s 的 GPIO: %d\n", child->name, err);
            of_node_put(child);
            return err;
        }

        err = fwnode_property_read_u32(&child->fwnode, "linux,code", &input_data->button_data[i].key_code);
        if (err) {
            dev_err(dev, "%s 缺少 linux,code\n", child->name);
            of_node_put(child);
            return err;
        }

        __set_bit(input_data->button_data[i].key_code, input_data->input_device->keybit);

        input_data->button_data[i].irq = fwnode_irq_get(&child->fwnode, 0);
        if (input_data->button_data[i].irq < 0) {
            dev_err(dev, "无法获取 %s 的中断: %d\n", child->name, input_data->button_data[i].irq);
            of_node_put(child);
            return input_data->button_data[i].irq;
        }

        unsigned int trigger_type = irq_get_trigger_type(input_data->button_data[i].irq);
        if (!trigger_type)
            trigger_type = IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING;

        err = devm_request_irq(dev, input_data->button_data[i].irq, input_device_irq_handler,
                               trigger_type, child->name, &input_data->button_data[i]);
        if (err) {
            dev_err(dev, "无法请求 %s 的中断 %d: %d\n",
                    child->name, input_data->button_data[i].irq, err);
            of_node_put(child);
            return err;
        }

        u32 debounce_ms;
        if (fwnode_property_read_u32(&child->fwnode, "debounce-interval", &debounce_ms))
            debounce_ms = 50;
        err = gpiod_set_debounce(input_data->button_data[i].gpiod, debounce_ms * 1000);
        if (err)
            dev_warn(dev, "无法为 %s 设置去抖: %d\n", child->name, err);

        dev_info(dev, "添加按键 %s: GPIO %d, 中断 %d, 按键码 %d\n",
                 child->name, desc_to_gpio(input_data->button_data[i].gpiod),
                 input_data->button_data[i].irq, input_data->button_data[i].key_code);
        i++;
    }

    err = input_register_device(input_data->input_device);
    if (err) {
        dev_err(dev, "无法注册输入设备: %d\n", err);
        return err;
    }

    dev_info(dev, "输入设备注册为 %s\n", dev_name(&input_data->input_device->dev));

    platform_set_drvdata(pdev, my_input_dev);
    dev_info(dev, "输入设备探测成功\n");

    return 0;
}

static int input_device_remove(struct platform_device *pdev)
{
    struct my_input_device *my_input_dev = platform_get_drvdata(pdev);
    struct input_device_data *data = my_input_dev->input_data;

    input_unregister_device(data->input_device);
    device_destroy(my_input_dev->class, my_input_dev->dev_no);
    class_destroy(my_input_dev->class);
    cdev_del(&my_input_dev->cdev);
    unregister_chrdev_region(my_input_dev->dev_no, 1);

    dev_info(&pdev->dev, "已移除, 按键数量=%d\n", data->n_buttons);
    return 0;
}

static void input_device_shutdown(struct platform_device *pdev)
{
}

static const struct of_device_id input_device_of_match[] = {
    { .compatible = "dirve,input_device", },
    { },
};

static struct platform_driver input_device_driver = {
    .probe = input_device_probe,
    .remove = input_device_remove,
    .shutdown = input_device_shutdown,
    .driver = {
        .name = "input_device",
        .of_match_table = input_device_of_match,
    }
};

static int __init input_device_init(void)
{
    return platform_driver_register(&input_device_driver);
}

static void __exit input_device_exit(void)
{
    platform_driver_unregister(&input_device_driver);
}

module_init(input_device_init);
module_exit(input_device_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("多按键输入设备驱动");