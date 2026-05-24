#define _GNU_SOURCE
#include<time.h>
#include "../source/lvgl/lvgl.h"
#include "../source/lvgl/demos/lv_demos.h"
#include "../source/lv_drivers/display/fbdev.h"
#include "../source/lv_drivers/indev/evdev.h"
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include "../source/ui/ui.h"
#include <stdio.h>
#include "../include/demo_app.h"
#include <unistd.h>
#include "../include/mailbox.h"
#include "../include/list.h"
#include "../include/save.h"
#include "../include/mqtt.h"

Link_t *pmail = NULL;
#define DISP_BUF_SIZE (128 * 1024)

void *get_data_th(void *arg)
{
	printf("DEBUG get_data_th\n");
	Que_Data_t data;

	while (1)
	{
		data.id = 1;
		get_data_dht11(&data);
		get_data_bh1750(&data);
		sensor_mq135(&data);
		time_t sec_num = time(NULL);

		printf("DEBUG get data complete\n");

    	//struct tm tmp_tm;
    	if (localtime_r(&sec_num, &(data.tim)) == NULL)
    	{  
        	perror("localtime_r failed");
        	return NULL;
    	}
		//data.tim = tmp_tm;
		//printf("time : %ld",data.tim.tm_year + 1900);
		send_msg(pmail,"get_data", data, "show_data");
		send_msg(pmail,"get_data", data, "save_data");
		send_msg(pmail,"get_data", data, "send_data");
		printf("DEBUG get_data_th end\n");
		sleep(1);
	}
	return NULL;
}
void *show_data_th(void *arg)
{
	printf("DEBUG show_data_th\n");
	Que_Data_t data;
	char sendname[32] = {0};
	
    lv_init();

    /*Linux frame buffer device init*/
    fbdev_init();

    /*A small buffer for LittlevGL to draw the screen's content*/
    static lv_color_t buf[DISP_BUF_SIZE];

    /*Initialize a descriptor for the buffer*/
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);

    /*Initialize and register a display driver*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf   = &disp_buf;
    disp_drv.flush_cb   = fbdev_flush;
    disp_drv.hor_res    = 800;
    disp_drv.ver_res    = 480;
    lv_disp_drv_register(&disp_drv);

    evdev_init();
    static lv_indev_drv_t indev_drv_1;
    lv_indev_drv_init(&indev_drv_1); /*Basic initialization*/
    indev_drv_1.type = LV_INDEV_TYPE_POINTER;

    /*This function will be called periodically (by the library) to get the mouse position and state*/
    indev_drv_1.read_cb = evdev_read;
    lv_indev_t *mouse_indev = lv_indev_drv_register(&indev_drv_1);


    /*Set a cursor for the mouse*/
    LV_IMG_DECLARE(mouse_cursor_icon)
    lv_obj_t * cursor_obj = lv_img_create(lv_scr_act()); /*Create an image object for the cursor */
    lv_img_set_src(cursor_obj, &mouse_cursor_icon);           /*Set the image source*/
    lv_indev_set_cursor(mouse_indev, cursor_obj);             /*Connect the image  object to the driver*/


    /*Create a Demo*/
    //lv_demo_widgets();
   

    /*Handle LitlevGL tasks (tickless mode)*/
    while(1) {
        ui_init(pmail);
        
		printf("show_data ok!\n");
		lv_timer_handler();
        usleep(5000);
    }
	
	return NULL;
}
void *save_data_th(void *arg)
{
	printf("DEBUG save_data_th\n");
	Que_Data_t data;
	char sendname[32] = {0};
	init_save_module();
	while (1)
	{
		recv_msg(pmail,&data);
		save_sqlite3(data);
		printf("save_data ok!\n");
	}
	deinit_save_module();
	return NULL;
}
void *send_data_th(void *arg)
{
	printf("DEBUG send_data_th\n");
	Que_Data_t data;
	char sendname[32] = {0};
	mqtt_init();
	while (1)
	{
		recv_msg(pmail,&data);
		mqtt_send("smoke", data.smoke);
		mqtt_send("temp", data.temp);
		mqtt_send("hum",data.hum); //rand()%100*0.1);
		mqtt_send("light", data.light);
		printf("DEBUG MQtt send :  %f %f %f %f\n", data.smoke,data.temp, data.hum, data.light);
	}
	mqtt_deinit();
	return NULL;
}


int main(int argc, const char *argv[])
{
	pmail = create_mailbox();
	if (NULL == pmail)
	{
		return -1;
	}
	printf("DEBUG create_mailbox success\n");

	register_thread_task(pmail, show_data_th, "show_data");
	register_thread_task(pmail, save_data_th, "save_data");
	register_thread_task(pmail, send_data_th, "send_data");
	register_thread_task(pmail, get_data_th, "get_data");

	destroy_mailbox(pmail);
	printf("DEBUG ----------main end\n");
	return 0;
}

uint32_t custom_tick_get(void)
{
    static uint64_t start_ms = 0;
    if(start_ms == 0) {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
    }

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t now_ms;
    now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

    uint32_t time_ms = now_ms - start_ms;
    return time_ms;
}