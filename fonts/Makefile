include ../Makefile.template
_CFLAGS  = -I.. -I../src -DLV_CONF_INCLUDE_SIMPLE -DLV_FONT_FMT_TXT_LARGE
OBJS=\
	lv_font_cjk_48.o
all: liblvgl_font.so liblvgl_font.a
lv_font_cjk_48.o: lv_font_cjk_48.c
liblvgl_font.so: $(OBJS)
liblvgl_font.a: $(OBJS)
