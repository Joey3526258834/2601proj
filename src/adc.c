#include "stdio.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "sys/ioctl.h"
#include "fcntl.h"
#include "stdlib.h"
#include "string.h"
#include <poll.h>
#include <sys/select.h>
#include <sys/time.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include "adc.h"

struct adc_dev imx6ulladc;
 
/*
* @description : 读取指定文件内容
* @param – filename : 要读取的文件路径
* @param - str : 读取到的文件字符串
* @return : 0 成功;其他 失败
*/
int file_data_read(char *filename, char *str)
{
    int ret = 0;
    FILE *data_stream;
    //printf("DEBUG file_data_read\n");

    data_stream = fopen(filename, "r"); /* 只读打开 */
    if(data_stream == NULL)
    {
        printf("can't open file %s\r\n", filename);
        return -1;
    }

    ret = fscanf(data_stream, "%s", str);
    if(!ret) 
    {
        printf("file read error!\r\n");
    } 
    else if(ret == EOF) 
    {
    /* 读到文件末尾的话将文件指针重新调整到文件头 */
        fseek(data_stream, 0, SEEK_SET); 
    }
    //printf("DEBUG file_data_read end\n");
    fclose(data_stream); /* 关闭文件 */ 
    return 0;
}
 
/*
* @description : 获取 ADC 数据
* @param - dev : 设备结构体
* @return : 0 成功;其他 失败
*/
int adc_read(struct adc_dev *dev)
{
    int ret = 0;
    char str[50];

    printf("DEBUG adc_read\n");

    SENSOR_FLOAT_DATA_GET(ret, IN_VOLTAGE_SCALE, str, scale);
    SENSOR_INT_DATA_GET(ret, IN_VOLTAGE_RAW, str, raw);
 
    /* 转换得到实际电压值 mV */
    dev->act = (dev->scale * dev->raw)/1000.f;

    //printf("DEBUF 实际电压值%dmV\n", dev->act);
    return ret;
}

void sort(unsigned short *a,int len)
{
    int i = 0;
    int j = 0;
    for(i = 0; i < len-1; ++i)
    {
        for(j = i+1; j < len;++j)
        {
            
            if(a[i]>a[j])
            {
                int t = a[i];
                a[i] =a[j];
                a[j] = t;
            }
        }
    }
}

/*
* @description : MQ135电压转CO2浓度（PPM）
* @param - voltage : ADC读取的实际电压值（V）
* @return : 气体浓度（PPM，整数）
*/
float mq135_voltage_to_ppm(float voltage)
{
    // 防止电压为0导致除零错误
    if(voltage < 0.01f) return 0.0f;

    // 计算传感器当前电阻Rs
    float rs = (5.0f * MQ135_RL / voltage) - MQ135_RL;
    
    // 计算Rs/R0比值
    float ratio = rs / MQ135_RO_CLEAN;
    
    // MQ135官方CO2拟合公式：PPM = 400 * (Rs/R0)^(-1.179)
    float ppm = MQ135_PPM_BASE * pow(ratio, -1.179f);
    
    // 限制合理范围（0~10000PPM）
    if(ppm < 0.0f) ppm = 0.0f;
    if(ppm > 10000.0f) ppm = 10000.0f;
    
    return ppm;
}

/*
* @description : 多次采样ADC并取去极值平均（提升稳定性）
* @param - dev : 设备结构体
* @param - sample_cnt : 采样次数（建议10~200次）
* @return : 0 成功;其他 失败
*/
int adc_read_avg(struct adc_dev *dev, int sample_cnt)
{
    if(sample_cnt < 10 || sample_cnt > 200) sample_cnt = 100; // 限制合理采样范围
    unsigned short raw_buf[200] = {0};
    float scale = 0.0f;
    char str[50];
    int ret, i;

    // 先读取scale（ADC刻度，只需读1次）
    ret = file_data_read(file_path[IN_VOLTAGE_SCALE], str);
    if(ret != 0)
    {
        printf("read adc scale failed! ret=%d\r\n", ret);
        return ret;
    }
    scale = atof(str);

    // 连续采样raw值
    for(i = 0; i < sample_cnt; i++)
    {
        ret = file_data_read(file_path[IN_VOLTAGE_RAW], str);
        if(ret != 0) 
        {
            printf("read adc raw failed! idx=%d, ret=%d\r\n", i, ret);
            return ret;
        }
        raw_buf[i] = atoi(str);
        usleep(1000); // 每次采样间隔1ms，减少噪声
    }

    // 排序后去掉前后10%的极值
    sort(raw_buf, sample_cnt);
    int start = sample_cnt * 0.1;
    int end = sample_cnt * 0.9;
    unsigned int raw_sum = 0;
    for(i = start; i < end; i++)
    {
        raw_sum += raw_buf[i];
    }
    int raw_avg = raw_sum / (end - start);

    // 计算最终电压值
    dev->raw = raw_avg;
    dev->scale = scale;
    dev->act = (scale * raw_avg) / 1000.0f;

    return 0;
}