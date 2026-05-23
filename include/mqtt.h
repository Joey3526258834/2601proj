#ifndef HEAD_H
#define HEAD_H


#include </home/linux/nfs/rootfs/mqtt/paho.mqtt.c-master/src/MQTTAsync.h>
#include </home/linux/nfs/rootfs/mqtt/paho.mqtt.c-master/src/MQTTClient.h>

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

//#define OLD_ADDRESS     "tcp://218.201.45.7:1883"
#define NEW_ADDRESS     "tcp://183.230.40.96:1883"
#define DEV_NAME  "device01"		   // "first_device"
#define CLIENTID DEV_NAME
#define PRODUCT_ID "2O6h46n1vc"   //"3XoV8495r6"
#define PASSWD "version=2018-10-31&res=products%2F2O6h46n1vc%2Fdevices%2Fdevice01&et=8937255523&method=md5&sign=kgf%2Bk7aMvTEKRFqpZs5%2FzA%3D%3D"



#define QOS         0
#define TIMEOUT     10000L
//#define __cplusplus

extern void mqtt_deinit();
extern int mqtt_send(char * key, float value);
extern int mqtt_init();
#endif // HEAD_H
