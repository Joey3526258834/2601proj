#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/of_gpio.h>
#include <linux/of.h>
#include <asm/uaccess.h>
#include <asm/delay.h>

static int gpio_dht11 = 0;
extern void msleep(unsigned int msecs);

static void dht11_reset(void)
{
    gpio_direction_output(gpio_dht11, 1);
    gpio_set_value(gpio_dht11, 0);
    msleep(20);
    gpio_set_value(gpio_dht11, 1);
    
    return;
}

static int recv_dht11_respone(void)
{
    int timeout = 0;

    gpio_direction_input(gpio_dht11);
    
    timeout = 0;
    while (gpio_get_value(gpio_dht11) && timeout <= 10) {
        udelay(10);
        timeout++;
    }
    if (timeout > 10)
        return -1;

    while (!gpio_get_value(gpio_dht11));
    while (gpio_get_value(gpio_dht11));

    return 0;
}

static unsigned char recv_dht11_byte(void)
{   
    int cnt = 0;
    unsigned char value = 0;
    int n = 0;

    gpio_direction_input(gpio_dht11);

    for (n = 7; n >= 0; n--) {
        while (!gpio_get_value(gpio_dht11));
        
        cnt = 0;
        while (gpio_get_value(gpio_dht11)) {
            udelay(10);
            cnt++;
        }

        if (cnt > 5) {
            value |= (0x1 << n);
        } 
    }
    
    return value;
}

static void recv_dht11_data(unsigned char *pdata)
{
    pdata[0] = recv_dht11_byte();
    pdata[1] = recv_dht11_byte();
    pdata[2] = recv_dht11_byte();
    pdata[3] = recv_dht11_byte();
    pdata[4] = recv_dht11_byte();

    return;
}

static ssize_t dht11_read(struct file *fp, char __user *puser, size_t n, loff_t *off)
{   
    int ret = 0;
    long nret = 0;
    unsigned char data[5] = {0};

    dht11_reset();
    ret = recv_dht11_respone();
    if (ret) 
        return ret; 
    recv_dht11_data(data);
    gpio_direction_output(gpio_dht11, 1);

    if (data[0] + data[1] + data[2] + data[3] != data[4])
        return -2;

    nret = copy_to_user(puser, data, 4);
    if (nret) {
        pr_info("copy_to_user failed\n");
        return -3;
    }

    return 4;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = dht11_read,
};

static struct miscdevice misc_dht11 = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "misc_dht11",
    .fops = &fops,
};

static int dht11_probe(struct platform_device *pdev)
{
    int ret = 0;
    struct device_node *pdht11node = NULL;
    
    ret = misc_register(&misc_dht11);
    if (ret) {
        pr_info("misc_register failed\n");
        return -1;
    }

    pdht11node = of_find_node_by_path("/putedht11");
    if (NULL == pdht11node) {
        pr_info("of_find_node_by_path failed\n");
        return -1;
    }

    gpio_dht11 = of_get_named_gpio(pdht11node, "gpio-dht11", 0);
    if (gpio_dht11 < 0) {
        pr_info("of_get_named_gpio failed\n");
        return -1;
    }

    ret = devm_gpio_request(misc_dht11.this_device, gpio_dht11, "pute-dht11");
    if (ret) {
        pr_info("devm_gpio_request failed\n");
        return -1;
    }

    gpio_direction_output(gpio_dht11, 1);

    return 0;
}

static int dht11_remove(struct platform_device *pdev)
{
    int ret = 0;
    
    ret = misc_deregister(&misc_dht11);
    if (ret) {
        pr_info("misc_deregister failed\n");
        return -1;
    }

    return 0;
}

static struct of_device_id dht11_of_match_table[] = {
    {.compatible = "pute,putedht11"},
    {},
};

static struct platform_device_id dht11_id_table[] = {
    {.name = "putedht11"},
    {},
};

static struct platform_driver dht11_driver = {
    .probe = dht11_probe,
    .remove = dht11_remove,
    .driver = {
        .name = "putedht11",
        .owner = THIS_MODULE,
        .of_match_table = dht11_of_match_table,
    },
    .id_table = dht11_id_table,
};

module_platform_driver(dht11_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("pute");
