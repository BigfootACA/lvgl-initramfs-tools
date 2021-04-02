#include"drivers.h"
#include<time.h>
#include<errno.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<limits.h>
#include<pthread.h>
#include<sys/time.h>
#include<drm_fourcc.h>
#include"lv_conf.h"
#include"utils.h"
#include"lib.h"
uint32_t w=-1,h=-1;
bool run=true;
void do_quit(){
	fbdev_exit();
	drm_exit();
}
int _exec(draw_func draw){
	lv_init();
	fprintf(stderr,"try to scan drm\n");
	if(drm_scan_init_register(DRM_FORMAT_ARGB8888)<0){
		fprintf(stderr,"failed to scan drm, try to scan fbdev\n");
		if(fbdev_scan_init_register()<0)return return_stderr_printf(-1,"failed to scan fbdev\n");
		else fbdev_get_sizes(&w,&h);
	}else drm_get_sizes(&w,&h,NULL);
	ts_scan_register();
	lv_obj_t*screen;
	if(!(screen=lv_scr_act()))return return_stderr_printf(-1,"failed to get screen\n");
	draw(screen);
	while(run){
		lv_tick_inc(3);
		lv_task_handler();
		usleep(3000);
	}
	do_quit();
	return 0;
}
uint32_t custom_tick_get(void){
	static uint64_t start_ms=0;
	if(start_ms==0){
		struct timeval tv_start;
		gettimeofday(&tv_start,NULL);
		start_ms=(tv_start.tv_sec*1000000+tv_start.tv_usec)/1000;
	}
	struct timeval tv_now;
	gettimeofday(&tv_now,NULL);
	return (uint64_t)((tv_now.tv_sec*1000000+tv_now.tv_usec)/1000)-start_ms;
}
