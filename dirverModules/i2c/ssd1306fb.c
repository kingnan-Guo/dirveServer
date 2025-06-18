#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/fb.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/device.h>

#define SSD1306_NAME "ssd1306fb"
#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64
#define SSD1306_ADDRESS 0x3c

struct ssd1306_data {
    struct i2c_client *client;
    struct fb_info *fb_info;
    u8 *buffer; // 帧缓冲区
};

// 发送 SSD1306 命令
static int ssd1306_write_cmd(struct i2c_client *client, u8 cmd) {
    u8 buf[2] = {0x00, cmd};
    int ret = i2c_master_send(client, buf, 2);
    if (ret != 2) {
        dev_err(&client->dev, "发送命令 0x%02x 失败: %d\n", cmd, ret);
        return ret < 0 ? ret : -EIO;
    }
    return 0;
}

// 分块发送数据
static int ssd1306_write_data(struct i2c_client *client, u8 *data, int len) {
    const int chunk_size = 32;
    int i, ret;
    u8 *buf = kmalloc(chunk_size + 1, GFP_KERNEL);
    if (!buf) {
        dev_err(&client->dev, "分配数据缓冲区失败\n");
        return -ENOMEM;
    }

    for (i = 0; i < len; i += chunk_size) {
        int size = min(chunk_size, len - i);
        buf[0] = 0x40;
        memcpy(buf + 1, data + i, size);
        ret = i2c_master_send(client, buf, size + 1);
        if (ret != size + 1) {
            dev_err(&client->dev, "发送数据块 %d-%d 失败: %d\n", i, i + size - 1, ret);
            kfree(buf);
            return ret < 0 ? ret : -EIO;
        }
        udelay(100); // 微秒级延迟
    }
    kfree(buf);
    return 0;
}

// 初始化 SSD1306 显示器
static int ssd1306_init_display(struct ssd1306_data *data) {
    struct i2c_client *client = data->client;
    const u8 init_seq[] = {
        0xAE,       // 显示关闭
        0xD5, 0x80, // 时钟分频
        0xA8, 0x3F, // 多路复用比（64-1）
        0xD3, 0x00, // 显示偏移
        0x40,       // 起始行
        0x8D, 0x14, // 电荷泵
        0x20, 0x00, // 水平寻址模式
        0xA1,       // 段重映射
        0xC8,       // COM 扫描方向
        0xDA, 0x12, // COM 引脚配置
        0x81, 0xCF, // 对比度
        0xD9, 0xF1, // 预充电周期
        0xDB, 0x40, // VCOMH
        0xA4,       // 整个显示打开
        0xA6,       // 正常显示
        0xAF,       // 显示打开
    };
    int i;

    for (i = 0; i < sizeof(init_seq); i++) {
        if (ssd1306_write_cmd(client, init_seq[i])) {
            dev_err(&client->dev, "初始化命令 0x%02x 失败\n", init_seq[i]);
            return -EIO;
        }
        udelay(100);
    }

    // 清空显示内存
    if (!data->buffer) {
        dev_err(&client->dev, "缓冲区未分配\n");
        return -EINVAL;
    }
    memset(data->buffer, 0, SSD1306_WIDTH * SSD1306_HEIGHT / 8);
    return ssd1306_write_data(client, data->buffer, SSD1306_WIDTH * SSD1306_HEIGHT / 8);
}

// 更新显示内容
static int ssd1306_update_display(struct ssd1306_data *data) {
    struct i2c_client *client = data->client;
    int ret;

    ret = ssd1306_write_cmd(client, 0x21);
    if (ret) return ret;
    ret = ssd1306_write_cmd(client, 0x00);
    if (ret) return ret;
    ret = ssd1306_write_cmd(client, 0x7F);
    if (ret) return ret;

    ret = ssd1306_write_cmd(client, 0x22);
    if (ret) return ret;
    ret = ssd1306_write_cmd(client, 0x00);
    if (ret) return ret;
    ret = ssd1306_write_cmd(client, 0x07);
    if (ret) return ret;

    return ssd1306_write_data(client, data->buffer, SSD1306_WIDTH * SSD1306_HEIGHT / 8);
}

// 帧缓冲区写操作
static ssize_t ssd1306_fb_write(struct fb_info *info, const char __user *buf,
                                size_t count, loff_t *ppos) {
    struct ssd1306_data *data = info->par;
    unsigned long total_size = info->fix.smem_len;
    unsigned long offset = *ppos;
    int ret;

    // 放宽偏移检查
    if (offset >= total_size)
        offset = 0; // 重置到缓冲区开头
    if (count > total_size)
        count = total_size;

    if (copy_from_user(data->buffer + offset, buf, count))
        return -EFAULT;

    ret = ssd1306_update_display(data);
    if (ret)
        return ret;

    *ppos = offset + count;
    return count;
}

static struct fb_ops ssd1306_fb_ops = {
    .owner = THIS_MODULE,
    .fb_write = ssd1306_fb_write,
    .fb_fillrect = cfb_fillrect,
    .fb_copyarea = cfb_copyarea,
    .fb_imageblit = cfb_imageblit,
};

// 设置帧缓冲区
static int ssd1306_setup_fb(struct ssd1306_data *data) {
    struct fb_info *fb_info;
    int ret;

    // 分配缓冲区
    data->buffer = devm_kzalloc(&data->client->dev, SSD1306_WIDTH * SSD1306_HEIGHT / 8, GFP_KERNEL);
    if (!data->buffer) {
        dev_err(&data->client->dev, "分配缓冲区失败\n");
        return -ENOMEM;
    }

    fb_info = framebuffer_alloc(0, &data->client->dev);
    if (!fb_info) {
        dev_err(&data->client->dev, "分配帧缓冲区失败\n");
        return -ENOMEM;
    }

    data->fb_info = fb_info;
    fb_info->par = data;
    fb_info->fbops = &ssd1306_fb_ops;

    fb_info->var.xres = SSD1306_WIDTH;
    fb_info->var.yres = SSD1306_HEIGHT;
    fb_info->var.xres_virtual = SSD1306_WIDTH;
    fb_info->var.yres_virtual = SSD1306_HEIGHT;
    fb_info->var.bits_per_pixel = 1;
    fb_info->var.grayscale = 1;
    fb_info->fix.smem_len = SSD1306_WIDTH * SSD1306_HEIGHT / 8;
    fb_info->fix.line_length = SSD1306_WIDTH / 8;
    strscpy(fb_info->fix.id, SSD1306_NAME, sizeof(fb_info->fix.id));
    fb_info->screen_base = (char __iomem *)data->buffer;

    ret = register_framebuffer(fb_info);
    if (ret) {
        dev_err(&data->client->dev, "注册帧缓冲区失败：%d\n", ret);
        framebuffer_release(fb_info);
        return ret;
    }
    return 0;
}

static int ssd1306_probe(struct i2c_client *client) {
    struct ssd1306_data *data;
    int ret;

    dev_info(&client->dev, "探测 I2C 地址 0x%02x\n", client->addr);

    if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
        dev_err(&client->dev, "I2C 功能不支持\n");
        return -ENODEV;
    }

    data = devm_kzalloc(&client->dev, sizeof(*data), GFP_KERNEL);
    if (!data) {
        dev_err(&client->dev, "分配内存失败\n");
        return -ENOMEM;
    }

    data->client = client;
    i2c_set_clientdata(client, data);

    // 设置帧缓冲区（先分配缓冲区）
    ret = ssd1306_setup_fb(data);
    if (ret) {
        dev_err(&client->dev, "设置帧缓冲区失败：%d\n", ret);
        return ret;
    }

    // 初始化显示器
    ret = ssd1306_init_display(data);
    if (ret) {
        dev_err(&client->dev, "初始化 SSD1306 失败：%d\n", ret);
        unregister_framebuffer(data->fb_info);
        framebuffer_release(data->fb_info);
        return ret;
    }

    dev_info(&client->dev, "SSD1306 OLED 初始化成功，地址 0x%02x\n", client->addr);
    return 0;
}

static void ssd1306_remove(struct i2c_client *client) {
    struct ssd1306_data *data = i2c_get_clientdata(client);
    if (data->fb_info) {
        unregister_framebuffer(data->fb_info);
        framebuffer_release(data->fb_info);
    }
}

static const struct of_device_id ssd1306_of_match[] = {
    { .compatible = "kingnan,ssd1306" },
    {},
};
MODULE_DEVICE_TABLE(of, ssd1306_of_match);

static const struct i2c_device_id ssd1306_id[] = {
    { "ssd1306", 0 },
    {},
};
MODULE_DEVICE_TABLE(i2c, ssd1306_id);

static struct i2c_driver ssd1306_driver = {
    .driver = {
        .name = SSD1306_NAME,
        .of_match_table = ssd1306_of_match,
    },
    .probe = ssd1306_probe,
    .remove = ssd1306_remove,
    .id_table = ssd1306_id,
};

module_i2c_driver(ssd1306_driver);

MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("SSD1306 OLED Framebuffer Driver");
MODULE_LICENSE("GPL");