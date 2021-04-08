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
#include"../src/json.h"
LV_IMG_DECLARE(locale_icon);
struct item_s{
	unsigned int id;
	char title[64],action[32];
	lv_obj_t*object;
	bool selected;
	int top;
};
typedef struct item_s item_t;
struct config_s{
	char title[64],confirm[32];
	item_t*items[32];
};
typedef struct config_s config_t;
static config_t config;
static lv_obj_t*ok=NULL,*lang=NULL;
static bool is_state(lv_obj_t*obj,lv_state_t state){
	return lv_obj_get_state(obj,LV_BTN_PART_MAIN)&state;
}
const char**main_argv;
static void checked_btn(lv_obj_t*obj,lv_event_t event){
	if(event!=LV_EVENT_VALUE_CHANGED)return;
	item_t*t=NULL;
	for(size_t x=0;x<32&&(t=config.items[x]);x++){
		if(obj==t->object){
			t->selected=true;
			if(is_state(ok,LV_STATE_DISABLED)){
				fprintf(stderr,"now can confirm\n");
				lv_obj_clear_state(ok,LV_STATE_DISABLED);
			}
			lv_obj_add_state(t->object,LV_STATE_CHECKED);
		}else if(t->selected){
			t->selected=false;
			if(is_state(t->object,LV_STATE_CHECKED))
				lv_obj_clear_state(t->object,LV_STATE_CHECKED);
		}
	}
}
static void ok_click(lv_obj_t*obj,lv_event_t event){
	if(obj!=ok||event!=LV_EVENT_CLICKED)return;
	if(lv_obj_get_state(ok,LV_BTN_PART_MAIN)&LV_STATE_DISABLED)return;
	item_t*t=NULL;
	for(size_t x=0;x<32&&(t=config.items[x]);x++){
		if(!t->selected)continue;
		printf("%s\n",t->action);
		run=false;
		break;
	}
}
static lv_obj_t*add_button(lv_obj_t*frame,int y,char*v,lv_style_t*style){
	lv_obj_t*btn=lv_btn_create(frame,NULL);
	lv_obj_set_event_cb(btn,checked_btn);
	lv_obj_set_size(btn,DIS_X(95),DIS_Y(15));
	lv_obj_add_style(btn,LV_OBJ_PART_MAIN,style);
	lv_obj_align(btn,NULL,LV_ALIGN_IN_TOP_MID,0,y);
	lv_btn_set_checkable(btn,true);
	lv_obj_t*lbl=lv_label_create(btn,NULL);
	lv_label_set_text(lbl,v);
	return btn;
}
static void _draw(lv_obj_t*screen){

	// menu title
	lv_obj_t*title=lv_label_create(screen,NULL);
	lv_label_set_text(title,config.title);
	lv_obj_align(title,NULL,LV_ALIGN_IN_TOP_MID,0,DIS_Y(10));

	// menu button box
	lv_obj_t*page=lv_page_create(screen, NULL);
	lv_obj_set_size(page,w,DIS_Y(150));
	lv_obj_align(page,NULL,LV_ALIGN_CENTER,0,DIS_Y(-10));

	// button style
	static lv_style_t style;
	lv_style_init(&style);
	lv_style_set_radius(&style,LV_STATE_DEFAULT,5);

	// buttons
	item_t*t=NULL;
	for(size_t x=0;x<32&&(t=config.items[x]);x++)
		t->object=add_button(page,DIS_Y(t->top),t->title,&style);

	// confirm button
	ok=lv_btn_create(screen,NULL);
	lv_obj_set_event_cb(ok,ok_click);
	lv_obj_set_size(ok,DIS_X(40),DIS_Y(18));
	lv_obj_align(ok,NULL,LV_ALIGN_IN_BOTTOM_MID,0,DIS_Y(-20));
	lv_obj_add_state(ok,LV_STATE_DISABLED|LV_STATE_CHECKED);
	lv_label_set_text(lv_label_create(ok,NULL),config.confirm);

	// language button
	lang=lv_imgbtn_create(screen,NULL);
	lv_obj_set_event_cb(lang,i81n_click);
	lv_obj_set_size(lang,locale_icon.header.w,locale_icon.header.h);
	lv_obj_align(lang,NULL,LV_ALIGN_IN_BOTTOM_RIGHT,DIS_X(-4),DIS_Y(-4));
	lv_imgbtn_set_src(lang,LV_BTN_STATE_RELEASED,&locale_icon);
	lv_imgbtn_set_src(lang,LV_BTN_STATE_PRESSED,&locale_icon);
	lv_obj_add_style(lang,LV_OBJ_PART_MAIN,&style);

	// copyright
	lv_obj_t*copyright=lv_label_create(screen,NULL);
	lv_label_set_text(copyright,"Author: BigfootACA");
	lv_obj_add_style(copyright,LV_LABEL_PART_MAIN,NULL);
	lv_obj_align(copyright,NULL,LV_ALIGN_IN_BOTTOM_MID,0,DIS_Y(-10));

	init_translate_selector(screen);
}
#define JSON_MUST_TYPE(jo,type,str)\
	if(json_object_get_type(jo)!=type){\
		fprintf(stderr,str);\
		return -2;\
	}
#define JSON_TO_STRING(obj,str)\
	if(!(str=(char*)json_object_get_string(obj))){\
		fprintf(stderr,"parse string failed\n");\
		return -2;\
	}
#define JSON_PARSE_STRING(jo,jb,key,path,val)\
	if((jb=json_object_object_get(jo,key))){\
		JSON_MUST_TYPE(jb,json_type_string,path " must be string\n");\
		JSON_TO_STRING(jb,s);\
		translate(s,val,sizeof(val));\
	}
static json_object*sys_config=NULL;
static int parse_config(){
	char*s;
	json_object*jb=NULL;
	get_translate_object(sys_config);
	JSON_PARSE_STRING(sys_config,jb,"title",">title",config.title)
	JSON_PARSE_STRING(sys_config,jb,"confirm",">confirm",config.confirm)
	if((jb=json_object_object_get(sys_config,"items"))){
		json_object*jr,*jx;
		JSON_MUST_TYPE(jb,json_type_array,">items must be array");
		item_t*v;
		int top=0;
		for(size_t d=0,x=0;d<json_object_array_length(jb)&&d<32;d++){
			if(!(jr=json_object_array_get_idx(jb,d)))continue;
			if(!(v=malloc(sizeof(item_t))))continue;
			memset(v,0,sizeof(item_t));
			JSON_PARSE_STRING(jr,jx,"title",">items[*]>title",v->title)
			JSON_PARSE_STRING(jr,jx,"action",">items[*]>action",v->action)
			v->top=top;
			top+=20;
			config.items[x++]=v;
		}
	}
	return 0;
}
static int load_config(char*cfg){
	if(!(sys_config=json_object_from_file(cfg))){
		fprintf(stderr,"read %s failed: %s",cfg,json_util_get_last_err());
		return -1;
	}
	if(parse_config()<0){
		fprintf(stderr,"parsing config %s failed\n",cfg);
		return -2;
	}
	return 0;
}
static void init_config(){
	memset(config.title,0,sizeof(config.title));
	memset(config.confirm,0,sizeof(config.confirm));
	strcpy(config.title,"Menu");
	strcpy(config.confirm,"OK");
	memset(&(config.items),0,sizeof(config.items));
}
int main(int argc,char**argv){
	if(argc!=2||!argv[1]){
		fprintf(stderr,"Usage: guimenu <CONFIG>\n");
		return 1;
	}
	main_argv=(const char**)argv;
	init_config();
	if(load_config(argv[1])<0){
		fprintf(stderr,"failed to load config %s\n",argv[1]);
		return -1;
	}
	return _exec(_draw);
}
