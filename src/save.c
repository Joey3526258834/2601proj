#include"../include/save.h"

int num_day = 1;
time_t tmp_time;
sqlite3* db;

void init_save_module()
{
    
    int ret = 0;
    tmp_time = time(NULL);
    ret = sqlite3_open("./demo.db",&db);
    if(SQLITE_OK != ret)
    {
        fprintf(stderr,"open error:%s\n",sqlite3_errmsg(db));
        sqlite3_close(db);
        return ;
    }
    char tim_sec[100] = "create table if not exists timday(id INTEGER PRIMARY KEY AUTOINCREMENT,timesec INT);";
    ret = sqlite3_exec(db,tim_sec,NULL,NULL,NULL);
    sprintf(tim_sec,"insert into timday values(NULL,%ld);",tmp_time);
    sqlite3_exec(db,tim_sec,NULL,NULL,NULL);

}

int callback(void* arg,int cnt,char **values,char** name)
{
    tmp_time = atol(values[1]);
    return 0;
}

void isaday(time_t sec_num)
{
    int ret;
    sqlite3_exec(db,"select * from timday where id=1;",callback,NULL,NULL);
    time_t tmp;
    tmp = sec_num - tmp_time;
    if(tmp > 86400)
    {
      if((tmp / 86400) >= 8)
        {
            int i;
            for(i = 1;i < 8;++i)
            {
                char sql_dele[100] = {0};
                sprintf(sql_dele,"drop table if exists demo_day%d",i);
                ret = sqlite3_exec(db,sql_dele,NULL,NULL,NULL);
            }
        }
        num_day = (tmp / 86400) + num_day;
        if(num_day >= 8)
        {         
            num_day = 1;
        }
        char sql_dele[100] = {0};
        sprintf(sql_dele,"drop table if exists demo_day%d",num_day);
        ret = sqlite3_exec(db,sql_dele,NULL,NULL,NULL);  
        sqlite3_exec(db,"drop table timday;",NULL,NULL,NULL);
        init_save_module();
    }
    return ;
}

void deinit_save_module(void) 
{
    if (db != NULL) 
    {  
        sqlite3_close(db);
        db = NULL;
        printf("数据库连接已关闭\n");
    }
}

void save_sqlite3(Que_Data_t data)
{
    char tm_buf[512] = {0};
    time_t sec_num = time(NULL);

    struct tm tmp_tm;
    if (localtime_r(&sec_num, &tmp_tm) == NULL)
    {  
        perror("localtime_r failed");
        return ;
    }
    //data.tim = tmp_tm;
    
    int ret = 0;
    isaday(sec_num);
  
    char sql_cmd1[512]= {0};
    sprintf(sql_cmd1,"create table if not exists demo_day%d(id INT, temp REAL, hum REAL,light REAL,smoke INT,time TEXT,sendname TEXT,recvname TEXT);",num_day);//table name  info

    ret = sqlite3_exec(db,sql_cmd1,NULL,NULL,NULL);

    char tim_buf[512] = {0};
    sprintf(tim_buf,"%d-%d-%d %d:%d:%d",data.tim.tm_year + 1900,data.tim.tm_mon + 1,data.tim.tm_mday,data.tim.tm_hour,data.tim.tm_min,data.tim.tm_sec);
    //printf("time:%s\n",tim_buf);
    char sql_cmd[512]= {0};
    sprintf(sql_cmd,"insert into demo_day%d values(%d,%f,%f,%f,%f,'%s','%s','%s');",num_day,data.id,data.temp,data.hum,data.light,data.smoke,tim_buf,data.sendname,data.recvname);//table name  info
    char * errmsg;
    ret = sqlite3_exec(db,sql_cmd,NULL,NULL,&errmsg);
    if(SQLITE_OK != ret)
    {
        fprintf(stderr,"exec error:%s\n",errmsg);
        sqlite3_free(errmsg);
        sqlite3_close(db);
        return ;
    }
    return ;
}