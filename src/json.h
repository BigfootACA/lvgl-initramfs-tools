#ifndef _JSON_H
#define _JSON_H
#include<json-c/json.h>
extern json_object*get_translate_object(json_object*parent);
extern char*translate(char*ident,char*dest,size_t dest_len);
extern void init_translate_selector(lv_obj_t*screen);
extern void add_language_button(lv_obj_t*screen);
extern void add_language_button_custom(lv_obj_t*screen,lv_img_dsc_t icon);
#endif
