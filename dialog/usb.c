#include<time.h>
#include<errno.h>
#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>
#include<limits.h>
#include<pthread.h>
#include<sys/time.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include"../src/lv_conf.h"
#include"../src/drivers.h"
#include"../src/utils.h"
#include"../src/lib.h"
const char**main_argv;
LV_IMG_DECLARE(usb_icon);
static void _draw(lv_obj_t*screen){
	static lv_style_t bgcolor;
	lv_style_init(&bgcolor);
	lv_style_set_bg_color(&bgcolor,LV_STATE_DEFAULT,LV_COLOR_BLACK);
	lv_obj_add_style(screen,LV_OBJ_PART_MAIN,&bgcolor);

	// menu title
	lv_obj_t*title=lv_label_create(screen,NULL);
	lv_label_set_text(title,"USB mass storage");
	lv_obj_align(title,NULL,LV_ALIGN_IN_TOP_MID,0,DIS_Y(10));

	// USB picture
	lv_obj_t*usb=lv_img_create(screen,NULL);
	lv_img_set_src(usb,&usb_icon);
	lv_obj_align(usb,NULL,LV_ALIGN_CENTER,0,0);

	// copyright
	lv_obj_t*copyright=lv_label_create(screen,NULL);
	lv_label_set_text(copyright,"Author: BigfootACA");
	lv_obj_add_style(copyright,LV_LABEL_PART_MAIN,NULL);
	lv_obj_align(copyright,NULL,LV_ALIGN_IN_BOTTOM_MID,0,DIS_Y(-10));

	//init_translate_selector(screen);
}
int main(int argc,char**argv){
	main_argv=(const char**)argv;
	return _exec(_draw);
}
