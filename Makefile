CC      =$(CROSS_COMPILE)gcc
AR      =$(CROSS_COMPILE)ar
PKGCONF =$(CROSS_COMPILE)pkgconf
STRIP   =$(CROSS_COMPILE)strip
TOP     =$(PWD)
_CFLAGS  = -Ilvgl -Wall -Wextra -Werror -Wno-unused-parameter
CFLAGS  += -g -O3
LDFLAGS += -g
LIBS    += -lm -lpthread -ldrm -ljson-c
export CROSS_COMPILE CC AR PKGCONF STRIP CFLAGS LDFLAGS LIBS TOP
include Makefile.template
all: menu
lvgl-built: Makefile.lvgl
	@mkdir -p lvgl-build
	@$(MAKE) -C lvgl-build -f ../$< all
src/src.a src/menu.o: src/Makefile FORCE
	@$(MAKE) -C src all
fonts/liblvgl_font.so: FORCE
	$(MAKE) -C fonts liblvgl_font.so
lvgl-build/liblvgl.so: lvgl-built FORCE
	@true
menu_debug: src/menu.o src/src.a lvgl-build/liblvgl.so fonts/liblvgl_font.so
	@echo "  CCLD    $@"
	@$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)
menu: menu_debug
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
clean-fonts: fonts/Makefile
	@find fonts -name '*.o' -or -name '*.a'|xargs rm -f
clean: clean-lvgl clean-src clean-fonts clean-bin
FORCE:
.PHONY: test clean all FORCE