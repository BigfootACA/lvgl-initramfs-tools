#define _GNU_SOURCE
#include"drivers.h"
#include<stdio.h>
#include<stddef.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include<fcntl.h>
#include<errno.h>
#include<linux/input.h>
#include<unistd.h>
#include<pthread.h>
#include<stdbool.h>
#define LOGTAG "ts"
#include"utils.h"
pthread_t tsp=0;
static bool left_button_down=false;
static int16_t last_x=0,last_y=0,last_x_tmp=0,last_y_tmp=0;
static int flags=0;
static void*ts_handler(void*args){
	int fd;
	fd_set rds;
	struct input_absinfo abs={};
	if(!args)return NULL;
	if((fd=open((char*)args,O_RDONLY))<0)return (void*)return_perror(0,"open device '%s'",(char*)args);
 	ioctl(fd,EVIOCGABS(ABS_X),abs);
	ioctl(fd,EVIOCGABS(ABS_Y),abs);
	ioctl(fd,EVIOCGABS(ABS_PRESSURE),abs);
	while(1){
		FD_ZERO(&rds);
		FD_SET(fd,&rds);
		if(select(fd+1,&rds,NULL,NULL,NULL)<0)return (void*)return_perror(0,"select");
		else if(FD_ISSET(fd,&rds)){
			ssize_t s=sizeof(struct input_event);
			struct input_event event;
			if(read(fd,&event,s)!=s)continue;
			switch(event.type){
				case EV_SYN:if(event.code==SYN_REPORT){
					if((flags&0x03)==0x03){
						flags=0x00;
						last_x=last_x_tmp;
						last_y=last_y_tmp;
						if(!left_button_down)left_button_down=true;
					}else left_button_down=false;
				}break;
				case EV_REL:switch(event.code){
					case REL_X:last_x_tmp+=event.value;flags|=0x01;break;
					case REL_Y:last_y_tmp+=event.value;flags|=0x02;break;
				}break;
				case EV_ABS:switch(event.code){
					case ABS_X:case ABS_MT_POSITION_X:last_x_tmp=event.value;flags|=0x01;break;
					case ABS_Y:case ABS_MT_POSITION_Y:last_y_tmp=event.value;flags|=0x02;break;
				}break;
			}
		}
	}
	close(fd);
}
void ts_init(char*dev){
	char*x=strdup(dev);
	if(!x)return;
	fprintf(stderr,"starting touchscreen thread with device '%s'\n",x);
	if(tsp!=0)fprintf(stderr,"touchscreen thread already running\n");
	else if(pthread_create(&tsp,NULL,ts_handler,(void*)x)!=0)fprintf(stderr,"create thread failed\n");
	else pthread_setname_np(tsp,"TouchScreen Thread");
	free(x);
}
bool ts_read(struct _lv_indev_drv_t*indev_drv,lv_indev_data_t*data){
	data->point.x=last_x;
	data->point.y=last_y;
	data->state=left_button_down?LV_INDEV_STATE_PR:LV_INDEV_STATE_REL;
	return false;
}
int ts_scan_init(void){
	fprintf(stderr,"probing touchscreen devices\n");
	char path[32]={0},name[256]={0};
	int fd=-1,i;
	for(i=0;i<32;i++){
		unsigned char mask[EV_MAX/8+1];
		memset(path,0,32);
		memset(name,0,256);
		snprintf(path,31,"/dev/input/event%d",i);
		if((fd=open(path,O_RDONLY))<0){
			if(errno!=ENOENT)fprintf(stderr,"failed to open %s\n",path);
			continue;
		}
		if(ioctl(fd,EVIOCGNAME(255),name)<0){
			fprintf(stderr,"failed to event%d ioctl EIOCGNAME\n",i);
			strcpy(name,"unknown");
		}
		if(ioctl(fd,EVIOCGBIT(0,sizeof(mask)),mask)<0)fprintf(stderr,"failed to event%d ioctl EVIOCGBIT\n",i);
		else for(int j=0;j<EV_MAX;j++)if((mask[j/8]&(1<<(j%8)))&&j==EV_ABS){
			fprintf(stderr,"event%d name: '%s'\n",i,name);
			fprintf(stderr,"set event%d as default touchscreen\n",i);
			goto found;
		}
		close(fd);
	}
	fprintf(stderr,"no touchscreen found\n");
	return -1;
	found:ts_init(path);return 0;
}
static void _register(void){
	lv_indev_drv_t indev_drv;
	lv_indev_drv_init(&indev_drv);
	indev_drv.type=LV_INDEV_TYPE_POINTER;
	indev_drv.read_cb=ts_read;
	lv_indev_drv_register(&indev_drv);
}
void ts_register(char*dev){ts_init(dev);_register();}
void ts_scan_register(void){ts_scan_init();_register();}
