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
lvgl-build/.built: Makefile.lvgl FORCE
	@mkdir -p lvgl-build
	@$(MAKE) -C lvgl-build -f ../$< all
	@touch $@
src/.built: src/Makefile FORCE
	@$(MAKE) -C src all
	@touch $@
fonts/.built: fonts/Makefile FORCE
	@$(MAKE) -C fonts all
	@touch $@
icons/.built: icons/Makefile FORCE
	@$(MAKE) -C icons all
	@touch $@
lvgl-build/liblvgl.so lvgl-build/liblvgl.a: lvgl-build/.built
	@true
fonts/liblvgl_font.so fonts/liblvgl_font.a: fonts/.built
	@true
icons/liblvgl_icon.so icons/liblvgl_icon.a: icons/.built
	@true
src/liblvgl_tool.so src/liblvgl_tool.a: src/.built
	@true
dialog/menu.o: dialog/menu.c lvgl/lvgl.h src/lv_conf.h src/drivers.h
menu_debug: dialog/menu.o src/liblvgl_tool.so lvgl-build/liblvgl.so fonts/liblvgl_font.so icons/liblvgl_icon.so
	@echo "  CCLD    $@"
	@$(CC) -o $@ dialog/menu.o -llvgl_tool -llvgl -llvgl_font -llvgl_icon -Lsrc -Llvgl-build -Lfonts -Licons $(LDFLAGS) $(LIBS)
menu_static: dialog/menu.o src/liblvgl_tool.a lvgl-build/liblvgl.a fonts/liblvgl_font.a icons/liblvgl_icon.a
	@echo "  CCLD    $@"
	@$(CC) -s -static -o $@ dialog/menu.o -llvgl_tool -llvgl -llvgl_font -llvgl_icon -Lsrc -Llvgl-build -Lfonts -Licons $(LDFLAGS) $(LIBS)
	@ls -lh $@
usb_debug: dialog/usb.o src/liblvgl_tool.so lvgl-build/liblvgl.so fonts/liblvgl_font.so icons/liblvgl_icon.so
	@echo "  CCLD    $@"
	@$(CC) -o $@ dialog/usb.o -llvgl_tool -llvgl -llvgl_font -llvgl_icon -Lsrc -Llvgl-build -Lfonts -Licons $(LDFLAGS) $(LIBS)
usb_static: dialog/usb.o src/liblvgl_tool.a lvgl-build/liblvgl.a fonts/liblvgl_font.a icons/liblvgl_icon.a
	@echo "  CCLD    $@"
	@$(CC) -s -static -o $@ dialog/usb.o -llvgl_tool -llvgl -llvgl_font -llvgl_icon -Lsrc -Llvgl-build -Lfonts -Licons $(LDFLAGS) $(LIBS)
	@ls -lh $@
menu: menu_debug
	@echo "  STRIP   $@"
	@$(STRIP) $^ -o $@
	@ls -lh $@
usb: usb_debug
	@echo "  STRIP   $@"
	@$(STRIP) $^ -o $@
	@ls -lh $@
clean-bin:
	@rm -f {menu,usb}{_debug,}
clean-lvgl:
	@rm -rf lvgl-build
	@-cd lvgl;git clean -xdfq
clean-src: src/Makefile
	@find src -name '*.o' -or -name '*.a' -or -name '*.so'|xargs rm -f
clean-dialog: src/Makefile
	@find dialog -name '*.o' -or -name '*.a' -or -name '*.so'|xargs rm -f
clean-fonts: fonts/Makefile
	@find fonts -name '*.o' -or -name '*.a' -or -name '*.so'|xargs rm -f
clean-icons: icons/Makefile
	@find icons -name '*.o' -or -name '*.a' -or -name '*.so'|xargs rm -f
clean: clean-lvgl clean-src clean-fonts clean-bin clean-dialog clean-icons
FORCE:
.PHONY: test clean all FORCE clean-lvgl clean-src clean-fonts clean-bin clean-dialog clean-icons
