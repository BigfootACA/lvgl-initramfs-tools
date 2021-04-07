ifndef NO_DEFAULT
CC      =$(CROSS_COMPILE)gcc
AR      =$(CROSS_COMPILE)ar
PKGCONF =$(CROSS_COMPILE)pkgconf
STRIP   =$(CROSS_COMPILE)strip
endif
TOP     =$(PWD)
LIBDRM_CFLAGS ?= $(shell $(PKGCONF) --cflags libdrm)
JSON_C_CFLAGS ?= $(shell $(PKGCONF) --cflags json-c)
LIBDRM_LIBS ?= $(shell $(PKGCONF) --libs libdrm)
JSON_C_LIBS ?= $(shell $(PKGCONF) --libs json-c)
_CFLAGS  = -Ilvgl -Wall -Wextra -Werror -Wno-unused-parameter
CFLAGS  += -g -O3 $(LIBDRM_CFLAGS) $(JSON_C_CFLAGS)
LDFLAGS += -g
LIBS    += -lm -lpthread $(LIBDRM_LIBS) $(JSON_C_LIBS)
export CROSS_COMPILE CC AR PKGCONF STRIP CFLAGS LDFLAGS LIBS TOP
include Makefile.template
all: menu usb
lvgl-build/liblvgl.so: Makefile.lvgl FORCE
	@mkdir -p lvgl-build
	@$(MAKE) -C lvgl-build -f ../$< all
src/src.a: src/Makefile FORCE
	@$(MAKE) -C src all
fonts/liblvgl_font.so: fonts/Makefile FORCE
	@$(MAKE) -C fonts liblvgl_font.so
dialog/menu.o: dialog/menu.c lvgl/lvgl.h src/lv_conf.h src/drivers.h
menu_debug: dialog/menu.o src/src.a lvgl-build/liblvgl.so fonts/liblvgl_font.so
	@echo "  CCLD    $@"
	@$(CC) -o $@ dialog/menu.o src/src.a -llvgl -llvgl_font -Llvgl-build -Lfonts $(LDFLAGS) $(LIBS)
usb_debug: dialog/usb.o src/src.a lvgl-build/liblvgl.so fonts/liblvgl_font.so
	@echo "  CCLD    $@"
	@$(CC) -o $@ dialog/usb.o src/src.a -llvgl -llvgl_font -Llvgl-build -Lfonts $(LDFLAGS) $(LIBS)
menu: menu_debug
	@echo "  STRIP   $@"
	@$(STRIP) $^ -o $@
	@ls -lh $@
usb: usb_debug
	@echo "  STRIP   $@"
	@$(STRIP) $^ -o $@
	@ls -lh $@
clean-bin:
	@rm -f menu{_debug,}
clean-lvgl:
	@rm -rf lvgl-build
	@-cd lvgl;git clean -xdfq
clean-src: src/Makefile
	@find src -name '*.o' -or -name '*.a'|xargs rm -f
clean-dialog: src/Makefile
	@find dialog -name '*.o' -or -name '*.a'|xargs rm -f
clean-fonts: fonts/Makefile
	@find fonts -name '*.o' -or -name '*.a'|xargs rm -f
clean: clean-lvgl clean-src clean-fonts clean-bin clean-dialog
FORCE:
.PHONY: test clean all FORCE clean-lvgl clean-src clean-fonts clean-bin clean-dialog
