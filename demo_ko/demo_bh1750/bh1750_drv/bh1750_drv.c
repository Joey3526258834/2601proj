#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/of_gpio.h>
#include <asm/uaccess.h>
#include <asm/gpio.h>
#include <linux/sysfs.h>
#include <linux/device.h>
#include <linux/mutex.h>
#include <asm/delay.h>
#include <linux/i2c.h>
#include <linux/miscdevice.h>

extern void msleep(unsigned int msecs);
static int bh1750_probe(struct i2c_client *pclient, const struct i2c_device_id *pdevid);
static ssize_t bh1750_read(struct file *fp, char __user *puser, size_t n, loff_t *off);
static int bh1750_remove(struct i2c_client *pclient);
static struct i2c_client *pbh1750client;

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = bh1750_read,
};

static struct miscdevice misc_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "misc_bh1750",
    .fops = &fops,
};

static struct i2c_device_id device_table[] = {
    {.name = "putebh1750"},
    {},
};

static struct of_device_id match_table[] = {
    {.compatible = "pute,putebh1750"},
    {},
};

static struct i2c_driver bh1750_driver = {
    .driver = {
        .name = "putebh1750",
        .owner = THIS_MODULE,
        .of_match_table = match_table,
    },
    .probe = bh1750_probe,
    .remove = bh1750_remove,
    .id_table = device_table,
};

static ssize_t bh1750_read(struct file *fp, char __user *puser, size_t n, loff_t *off)
{
    struct i2c_msg msg;
    int ret = 0;
    long nret = 0;
    unsigned char buf[2] = {0};
    short data = 0;

    //发送通电指令
    buf[0] = 0x01;
    msg.addr = pbh1750client->addr;
    msg.buf = buf;
    msg.flags = 0;
    msg.len = 1;

    ret = pbh1750client->adapter->algo->master_xfer(pbh1750client->adapter, &msg, 1);
    if (ret < 0)
        pr_info("pbh1750client->adapter->algo->master_xfer failed, ret = %d\n", ret);

    //发送高分辨率采集
    buf[0] = 0x10;
    msg.addr = pbh1750client->addr;
    msg.buf = buf;
    msg.flags = 0;
    msg.len = 1;

    ret = pbh1750client->adapter->algo->master_xfer(pbh1750client->adapter, &msg, 1);
    if (ret < 0)
        pr_info("pbh1750client->adapter->algo->master_xfer failed, ret = %d\n", ret);

    msleep(180);

    //接收采集结果
    msg.addr = pbh1750client->addr;
    msg.buf = buf;
    msg.flags |= I2C_M_RD;
    msg.len = 2;

    ret = pbh1750client->adapter->algo->master_xfer(pbh1750client->adapter, &msg, 1);
    if (ret < 0)
        pr_info("pbh1750client->adapter->algo->master_xfer failed, ret = %d\n", ret);

    data = (buf[0] << 8 | buf[1]);
    nret = copy_to_user(puser, &data, sizeof(data));
    if (nret)
        pr_info("copy_to_user failed\n");

    return sizeof(data);
}

static int bh1750_probe(struct i2c_client *pclient, const struct i2c_device_id *pdevid)
{
    int ret = 0;

    ret = misc_register(&misc_device);
    if (ret)
        pr_info("misc_register failed\n");
    pbh1750client = pclient;
    pr_info("probe success\n");

    return 0;
}

static int bh1750_remove(struct i2c_client *pclient)
{
    int ret = 0;

    ret = misc_deregister(&misc_device);
    if (ret)
        pr_info("misc_deregister failed\n");
    pr_info("remove success\n");

    return 0;
}

module_i2c_driver(bh1750_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("haiersen");