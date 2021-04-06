#ifndef LIB_H
#define LIB_H
#define DIS_X(x) (int)(w/100*x)
#define DIS_Y(y) (int)(h/200*y)
typedef void (*draw_func)(lv_obj_t*);
extern int _exec(draw_func draw);
extern void do_quit();
extern uint32_t custom_tick_get(void);
extern uint32_t w,h;
extern bool run;
extern const char**main_argv;
#endif
