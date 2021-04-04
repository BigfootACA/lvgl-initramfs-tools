include ../Makefile.template
_CFLAGS  = -I ../lvgl -I . -Wno-int-to-pointer-cast -Wall -Wextra -Werror -Wno-unused-parameter -Wno-pointer-sign -Wno-sign-compare
all: src.a
drm.o: drm.c drivers.h ../lvgl/lvgl.h lv_conf.h utils.h
fbdev.o: fbdev.c drivers.h ../lvgl/lvgl.h lv_conf.h utils.h
lib.o: lib.c ../lvgl/lvgl.h lv_conf.h drivers.h
src.a: fbdev.o drm.o ts.o utils.o lib.o
ts.o: ts.c drivers.h ../lvgl/lvgl.h lv_conf.h utils.h
utils.o: utils.h
.PHONY: all