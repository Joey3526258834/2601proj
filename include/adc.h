#ifndef __ADC_H__
#define __ADC_H__
 
//12位ADC

/* 字符串转数字，将浮点小数字符串转换为浮点数数值 */
#define SENSOR_FLOAT_DATA_GET(ret, index, str, member)\
ret = file_data_read(file_path[index], str);\
dev->member = atof(str);\
 
/* 字符串转数字，将整数字符串转换为整数数值 */
#define SENSOR_INT_DATA_GET(ret, index, str, member)\
ret = file_data_read(file_path[index], str);\
dev->member = atoi(str);\
 
static char *file_path[] = {
"/sys/bus/iio/devices/iio:device0/in_voltage_scale",
"/sys/bus/iio/devices/iio:device0/in_voltage1_raw",
};

#define MQ135_RL          10000.0f   // 负载电阻（10KΩ，模块上的贴片电阻）
#define MQ135_RO_CLEAN    76.63f     // 新鲜空气中的基准电阻（需自行校准）
#define MQ135_PPM_BASE    400.0f     // 新鲜空气CO2基准浓度（400PPM）

enum path_index {
IN_VOLTAGE_SCALE = 0,
IN_VOLTAGE_RAW,
};

struct adc_dev{
    int raw;
    float scale;
    float act;
};

extern int file_data_read(char *filename, char *str);
extern void sort(unsigned short *a,int len);
extern int adc_read(struct adc_dev *dev);
extern int adc_read_avg(struct adc_dev *dev, int sample_cnt);  // 多次采样平均
extern float mq135_voltage_to_ppm(float voltage);             // 电压转PPM浓度

#endif
