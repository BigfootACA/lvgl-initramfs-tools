include ../Makefile.template
_CFLAGS  = -I../lvgl -I../src -I. -DLV_CONF_INCLUDE_SIMPLE
all: liblvgl_icon.so
liblvgl_icon.so: usb_icon.o locale_icon.o
usb_icon.o: usb_icon.c ../lvgl/lvgl.h
locale_icon.o: usb_icon.c ../lvgl/lvgl.h
.PHONY: all