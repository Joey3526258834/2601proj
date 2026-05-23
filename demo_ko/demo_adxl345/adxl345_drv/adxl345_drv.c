#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/device.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/interrupt.h>
#include <linux/irqreturn.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/poll.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/miscdevice.h>
#include <linux/spi/spi.h>

/* ADXL345 寄存器定义 */
#define ADXL345_DEVID          0x00  // 设备ID寄存器
#define ADXL345_DEVID_VAL      0xE5   // 预期设备ID值
#define ADXL345_DATA_FORMAT    0x31  // 数据格式寄存器
#define ADXL345_POWER_CTL      0x2D  // 电源控制寄存器
#define ADXL345_DATAX0         0x32  // X轴数据起始地址

static ssize_t adxl345_read(struct file *fp, char __user *puser, size_t n, loff_t *off);
static int adxl345_probe(struct spi_device *spi);
static int adxl345_remove(struct spi_device *spi);
static struct spi_device *adxl345_spi;

static struct of_device_id  adxl345_matchtable[] = {
    {.compatible = "pute,puteadxl345"},
    {},
};

static struct spi_device_id adxl345_table[] = {
    {.name = "puteadxl345"},
    {},
};

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = adxl345_read,
};

static struct miscdevice miscdevice = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "misc_adxl345",
    .fops = &fops,
};

static struct spi_driver adxl345_driver = {
    .driver = {
        .owner = THIS_MODULE,
        .name = "puteadxl345",
        .of_match_table = adxl345_matchtable,
    },
    .id_table = adxl345_table,
    .probe = adxl345_probe,
    .remove = adxl345_remove,
};

/* 读取三轴加速度的主函数 */
int adxl345_read_xyz(struct spi_device *spi, int *x, int *y, int *z)
{
    int ret;
    u8 tx_buf[7] = {0};  // 发送缓冲区：命令+6字节数据
    u8 rx_buf[7] = {0};  // 接收缓冲区

    struct spi_transfer tr = {
        .tx_buf = tx_buf,
        .rx_buf = rx_buf,
        .len = 7,        // 1字节命令 + 6字节数据
        .delay_usecs = 20,
    };
    struct spi_message msg;

    /* 构造读取命令 (0x80: 读标志 | 0x40: 多字节模式) */
    tx_buf[0] = ADXL345_DATAX0 | 0x80 | 0x40;  // 连续读取6字节数据

    spi_message_init(&msg);
    spi_message_add_tail(&tr, &msg);

    /* 执行SPI传输 */
    ret = spi_sync(spi, &msg);
    if (ret < 0) {
        pr_err("SPI transfer failed: %d\n", ret);
        return ret;
    }

    /* 解析原始数据 (小端模式: LSB在前) */
    *x = (s16)((rx_buf[2] << 8) | rx_buf[1]); 
    *y = (s16)((rx_buf[4] << 8) | rx_buf[3]);
    *z = (s16)((rx_buf[6] << 8) | rx_buf[5]);

    return 0;
}

/* 初始化ADXL345传感器 */
int adxl345_init(struct spi_device *spi)
{
    int ret;
    u8 tx[2], rx[2];

    /* 验证设备ID */
    tx[0] = ADXL345_DEVID | 0x80;  // 读命令
    ret = spi_write_then_read(spi, tx, 1, rx, 1);
    if (ret < 0 || rx[0] != ADXL345_DEVID_VAL) {
        pr_info("Device ID check failed (got 0x%02X, expected 0x%02X)\n", rx[0], ADXL345_DEVID_VAL);
        return -ENODEV;
    }

    /* 配置数据格式 (±16g, 全分辨率) */
    tx[0] = ADXL345_DATA_FORMAT;  // 寄存器地址
    tx[1] = 0x0B;                 // 0x0B = 0b1011 (全分辨率 + ±16g)[6](@ref)
    ret = spi_write_then_read(spi, tx, 2, NULL, 0);
    if (ret < 0) return ret;

    /* 启动测量模式 */
    tx[0] = ADXL345_POWER_CTL;
    tx[1] = 0x08;                 // 0x08 = 测量模式[6](@ref)
    return spi_write_then_read(spi, tx, 2, NULL, 0);
}

static ssize_t adxl345_read(struct file *fp, char __user *puser, size_t n, loff_t *off)
{
    int ret = 0;
    int buf[3] = {0};

    if (adxl345_init(adxl345_spi) != 0) {
        pr_info("ADXL345 initialization failed\n");
        return -EIO;
    }

    /* 读取加速度数据 */
    if (adxl345_read_xyz(adxl345_spi, &buf[0], &buf[1], &buf[2]) != 0) {
        pr_info("read adxl345 failed\n");
    }

    ret = copy_to_user(puser, buf, sizeof(buf));
    if (ret)
        pr_info("copy_to_user failed\n");

    return sizeof(buf);
}

static int adxl345_probe(struct spi_device *spi)
{
    int ret = 0;
    
    adxl345_spi = spi;
    ret = misc_register(&miscdevice);
    if (ret)
        pr_info("misc register failed\n");

    pr_info("adxl345_spi->bits_per_word = %d\n", adxl345_spi->bits_per_word);
    pr_info("adxl345_spi->mode = %#x\n", adxl345_spi->mode);

    pr_info("adxl345 probe ok\n");

    return 0;
}

static int adxl345_remove(struct spi_device *spi)
{
    int ret = 0;
    
    ret = misc_deregister(&miscdevice);
    if (ret)
        pr_info("misc deregister failed\n");
    pr_info("adxl345 remove ok\n");

    return 0;
}

static int __init adxl345_drv_init(void)
{
    int ret = 0;

    ret = spi_register_driver(&adxl345_driver);
    if (ret)
        pr_info("spi_register_driver failed");

    return 0;
}

static void __exit adxl345_drv_exit(void)
{
    spi_unregister_driver(&adxl345_driver);
    
    return;
}

module_init(adxl345_drv_init);
module_exit(adxl345_drv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("haiersen");