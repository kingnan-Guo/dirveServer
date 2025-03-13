#include "my_resources.h"
#include "my_op.h"

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

static struct my_resources * my_rsc;



static int board_init(int which) {
    printk(KERN_INFO "-board_init-  %s %s %d \n", __FILE__, __FUNCTION__, __LINE__);

	if (!my_rsc)
	{
		my_rsc = get_resources();
	}
    printk("init gpio: group %d, pin %d\n", GROUP(my_rsc->pin), PIN(my_rsc->pin));

    printk("board_init my_rsc->pin: %d\n", my_rsc->pin);
    // 这里写逻辑

    return 0;
}

static int board_ctl(int which, int status) {
    printk("set led %s: group %d, pin %d\n", status ? "on" : "off", GROUP(my_rsc->pin), PIN(my_rsc->pin));
    // 这里写逻辑
    // 打印 my_rsc
    printk("board_ctl my_rsc->pin: %d\n", my_rsc->pin);
    return 0;
}



static struct my_operations board_operations = {
    .num = 1,
    .init = board_init,
    .ctl = board_ctl,
};


struct my_operations * get_board_operations(void)
{
    return &board_operations;
}


