#include "../include/demo_app.h"
#include "../include/adc.h"
struct adc_dev imx6ulladc;
pthread_mutex_t data_mutex = PTHREAD_MUTEX_INITIALIZER;



int get_data_dht11(Que_Data_t *pdata)
{
    printf("DEBUG get_data_dht11 start\n");
    if (pdata == NULL)
    {
        printf("错误：pdata 是空指针\n");
        return -1;
    }
    int fd;
    char buf[5] = {0};
    char buff[100] = {0};
    int ret;

    fd = open("/dev/misc_dht11", O_RDWR);
    if (fd < 0)
    {
        perror("open /dev/misc_dht11 failed");
        return -1;
    }

    sleep(2);
    ret = read(fd, buf, sizeof(buf));
    if (ret == 4)
    {
        sprintf(buff, "%d.%d \n", buf[0], buf[1]);
        pdata->hum = atof(buff);
        sprintf(buff, "%d.%d \n", buf[2], buf[3]);
        pdata->temp = atof(buff);
    }
    printf("DEBUG get_data_dht11 end\n");
    close(fd);
    return 0;
}

int get_data_bh1750(Que_Data_t *pdata)
{
    printf("DEBUG get_data_bh1750 start\n");
    if (pdata == NULL)
    {
        printf("错误：pdata 是空指针\n");
        return -1;
    }
    int fd = 0;
    short data = 0;

    fd = open("/dev/misc_bh1750", O_RDWR);
    if (-1 == fd)
    {
        perror("fail to open bh1750");
        return -1;
    }

    read(fd, &data, sizeof(data));
    pdata->light = (data / 1.2);
    printf("DEBUG get_data_bh1750 end\n");
    close(fd);

    return 0;
}

int sensor_mq135(Que_Data_t *pdata)
{
    int ret = 0;

    pthread_mutex_lock(&data_mutex);
    ret = adc_read_avg(&imx6ulladc, 100);
    if (ret == 0)
    { 
        /* 数据读取成功 */
        // printf("ADC 原始值：%d,电压值:%.3fV\r\n", imx6ulladc.raw, imx6ulladc.act);
        pdata->smoke = mq135_voltage_to_ppm(imx6ulladc.act);
    }
    else
    {
        // 读取失败时赋默认值-1
        pdata->smoke = -1;
        printf("DEBUG smoke read fail\n");
    }

    pthread_mutex_unlock(&data_mutex);
    //usleep(100000); /*100ms */
    
    return 0;
}
#if 0
int sensor_mq135(Que_Data_t *pdata)
{
    int ret = 0;
    printf("DEBUG sensor_mq135 start\n");
    pthread_mutex_lock(&data_mutex);
    ret = adc_read(&imx6ulladc);
    if (ret == 0)
    {
        /* 数据读取成功 */
        // printf("ADC 原始值：%d,电压值:%.3fV\r\n", imx6ulladc.raw, imx6ulladc.act);

        pdata->smoke = imx6ulladc.act;
    }
    pthread_mutex_unlock(&data_mutex);
    usleep(100000); /*100ms */
    printf("DEBUG sensor_mq135 end\n");

    return 0;
}
#endif
