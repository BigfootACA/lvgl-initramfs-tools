include ../Makefile.template
_CFLAGS  = -I ../lvgl -I . -Wno-int-to-pointer-cast -Wall -Wextra -Werror -Wno-unused-parameter -Wno-pointer-sign -Wno-sign-compare -fPIC
OBJS=\
	fbdev.o \
	drm.o \
	ts.o \
	utils.o \
	lib.o \
	translate.o
all: liblvgl_tool.so liblvgl_tool.a
drm.o: drm.c drivers.h ../lvgl/lvgl.h lv_conf.h utils.h
fbdev.o: fbdev.c drivers.h ../lvgl/lvgl.h lv_conf.h utils.h
lib.o: lib.c ../lvgl/lvgl.h lv_conf.h drivers.h
ts.o: ts.c drivers.h ../lvgl/lvgl.h lv_conf.h utils.h
translate.o: json.h
utils.o: utils.h
liblvgl_tool.so: $(OBJS)
liblvgl_tool.a: $(OBJS)
.PHONY: all