#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<unistd.h>
#include<limits.h>
#include"drivers.h"
#include"json.h"
#include"lib.h"
LV_IMG_DECLARE(locale_icon);
static json_object*trans=NULL,*trans_all=NULL;
struct lang_item{
	bool selected;
	char name[128],key[16];
	lv_obj_t*button;
};
static struct{
	lv_obj_t*selector,*select_ok;
	struct lang_item*items[256];
}trans_gui;
static char*_translate(json_object*_trans,char*ident,char*def,char*dest,size_t dest_len){
	if(!ident||!dest)return NULL;
	memset(dest,0,dest_len);
	char*ret=ident;
	if(_trans&&ident[0]=='@'){
		json_object*obj=json_object_object_get(_trans,ident+1);
		if(obj&&json_object_get_type(obj)==json_type_string)
			ret=(char*)json_object_get_string(obj);
		else fprintf(stderr,"translate: identifier %s not found\n",ident);
	}
	return strncpy(dest,ret,dest_len);
}
char*translate(char*ident,char*dest,size_t dest_len){return _translate(trans,ident,ident,dest,dest_len);}
static json_object*get_lang_object(json_object*parent,char*lang,int depth){
	if(!parent||!lang)return NULL;
	if(depth++>5){
		fprintf(stderr,"too many translate aliases\n");
		return NULL;
	}
	json_object*obj=json_object_object_get(parent,lang);
	if(obj)switch(json_object_get_type(obj)){
		case json_type_object:return obj;
		case json_type_string:return get_lang_object(parent,(char*)json_object_get_string(obj),depth);
		default:fprintf(stderr,"unknown translate type for %s\n",lang);
	}
	if(strcmp(lang,"C")==0){
		fprintf(stderr,"no any translate found\n");
		return NULL;
	}
	return get_lang_object(parent,"C",depth);
}
static char*get_current_locale(char*buff,size_t buff_len){
	if(!buff)return NULL;
	char*e=getenv("LANG");
	if(!e)e=getenv("LANGUAGE");
	if(!e)e=getenv("LC_ALL");
	if(!e)e="C";
	return strncpy(buff,e,buff_len);
}
json_object*get_translate_object(json_object*parent){
	if(trans)return trans;
	if((trans_all=json_object_object_get(parent,"translate"))){
		if(json_object_get_type(trans_all)!=json_type_object){
			fprintf(stderr,"translate must be object");
			return NULL;
		}
		char lang[16]={0};
		get_current_locale(lang,sizeof(lang));
		trans=get_lang_object(trans_all,lang,0);
	}
	return trans;
}
static void change_lang(char*lang){
	fprintf(stderr,"translate: change language to %s\n",lang);
	char buf[PATH_MAX]={0};
	setenv("LANG",lang,1);
	if(readlink("/proc/self/exe",buf,PATH_MAX)>0){
		execv(buf,(char**)main_argv);
		return;
	}
}
static void lang_select(lv_obj_t*obj,lv_event_t event){
	if(event!=LV_EVENT_CLICKED)return;
	struct lang_item*x=NULL;
	if(obj==trans_gui.select_ok){
		for(int xi=0;(x=trans_gui.items[xi]);xi++)if(x->selected){
			change_lang(x->key);
			break;
		}
	}else{
		for(int xi=0;(x=trans_gui.items[xi]);xi++){
			lv_obj_clear_state(x->button,LV_STATE_CHECKED);
			x->selected=(obj==x->button);
		}
		lv_obj_add_state(obj,LV_STATE_CHECKED);
		lv_obj_clear_state(trans_gui.select_ok,LV_STATE_DISABLED);
	}
}
static void i18n_click(lv_obj_t*obj,lv_event_t event){
	if(event!=LV_EVENT_CLICKED)return;
	lv_obj_set_hidden(trans_gui.selector,false);
	lv_obj_set_hidden(trans_gui.select_ok,false);
}
void init_translate_selector(lv_obj_t*screen){
	if(!trans||!trans_all)return;
	static lv_style_t style;
	lv_style_init(&style);
	lv_style_set_radius(&style,LV_STATE_DEFAULT,0);

	// language select list
	trans_gui.selector=lv_list_create(screen,NULL);
	lv_obj_set_size(trans_gui.selector,w,h-DIS_Y(20));
	lv_obj_align(trans_gui.selector,NULL,LV_ALIGN_IN_TOP_MID,0,0);
	lv_obj_add_style(trans_gui.selector,LV_OBJ_PART_MAIN,&style);
	lv_obj_set_hidden(trans_gui.selector,true);
	int xi=0;
	json_object_object_foreach(trans_all,key,val){
		if(json_object_get_type(val)!=json_type_object)continue;
		json_object*obj=json_object_object_get(trans_all,key);
		if(!obj)continue;
		struct lang_item*x=malloc(sizeof(struct lang_item));
		if(!x)continue;
		memset(x,0,sizeof(struct lang_item));
		strncpy(x->key,key,sizeof(x->key)-1);
		_translate(obj,"@name",key,x->name,sizeof(x->name));
		x->button=lv_list_add_btn(trans_gui.selector,NULL,x->name);
		lv_obj_set_event_cb(x->button,lang_select);
		if(obj==trans){
			lv_list_focus_btn(trans_gui.selector,x->button);
			lv_obj_add_state(x->button,LV_STATE_CHECKED);
		}
		trans_gui.items[xi++]=x;
	}

	// language confirm button
	trans_gui.select_ok=lv_btn_create(screen,NULL);
	lv_obj_set_size(trans_gui.select_ok,w,DIS_Y(20));
	lv_obj_align(trans_gui.select_ok,NULL,LV_ALIGN_IN_BOTTOM_MID,0,0);
	lv_obj_add_style(trans_gui.select_ok,LV_OBJ_PART_MAIN,&style);
	lv_obj_add_state(trans_gui.select_ok,LV_STATE_DISABLED|LV_STATE_CHECKED);
	lv_label_set_text(lv_label_create(trans_gui.select_ok,NULL),"OK");
	lv_obj_set_hidden(trans_gui.select_ok,true);
	lv_obj_set_event_cb(trans_gui.select_ok,lang_select);
}
void add_language_button(lv_obj_t*screen){
	add_language_button_custom(screen,locale_icon);
}
void add_language_button_custom(lv_obj_t*screen,lv_img_dsc_t icon){
	lv_obj_t*lang_btn=lv_imgbtn_create(screen,NULL);
	lv_obj_set_event_cb(lang_btn,i18n_click);
	lv_obj_set_size(lang_btn,icon.header.w,icon.header.h);
	lv_obj_align(lang_btn,NULL,LV_ALIGN_IN_BOTTOM_RIGHT,DIS_X(-4),DIS_Y(-4));
	lv_imgbtn_set_src(lang_btn,LV_BTN_STATE_RELEASED,&icon);
	lv_imgbtn_set_src(lang_btn,LV_BTN_STATE_PRESSED,&icon);
}
