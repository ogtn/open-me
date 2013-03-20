#  ome: Open Minimalistic Engine
#
#  Copyright: (c) 2012-2013
#  Olivier Guittonneau <OpenMEngine@gmail.com>
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the Do What The Fuck You Want To
#  Public License, Version 2, as published by Sam Hocevar. See
#  http://sam.zoy.org/projects/COPYING.WTFPL for more details.

# I hate makefiles, this is why that one is so crappy

CC=LC_ALL=en_US.UTF-8 colorgcc -Wunsafe-loop-optimizations
#CC=gcc
CC=clang -Wno-typedef-redefinition -Wparentheses

# hardcore mode :)
# http://blogs.gnome.org/otte/2008/12/22/warning-options/
# http://mces.blogspot.fr/2008/12/year-end-cleaning-ie-on-warning-options.html
# http://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
CWARN=-Wall -Wextra -fno-common -Wdeclaration-after-statement 
-Wformat=2 -Winit-self -Winline -Wpacked -Wp,-D_FORTIFY_SOURCE=2 \
-Wpointer-arith -Wlarger-than-65500 -Wmissing-declarations \
-Wmissing-format-attribute -Wmissing-noreturn -Wmissing-prototypes \
-Wnested-externs -Wold-style-definition -Wredundant-decls \
-Wsign-compare -Wstrict-aliasing=2 -Wstrict-prototypes -Wswitch-enum \
-Wundef -Wunreachable-code -Wwrite-strings -Werror=implicit-function-declaration\
#-ansi #-pedantic #-pedantic-errors

# easy version
#CWARN=-Wall -Wextra

# debug by default
ifdef RELEASE
CFLAGS=$(CWARN) -O3
OUTDIR=../Release
NOCHECK=1
else
CFLAGS=$(CWARN) -g -DDEBUG
OUTDIR=../Debug
endif

# private access checking activated by default
ifndef NOCHECK
CFLAGS+=-DOME_CHECK_PRIVACY
endif

#switch between ogl or ogles2
ifdef EMBEDDED
CFLAGS+=-DOME_USE_GLES
endif

LDFLAGS=

OPENME_INCLUDE=../OpenMe/include
UTHASH_INCLUDE=../uthash

all: $(OUTDIR)/messyExample $(OUTDIR)/test_string $(OUTDIR)/test_interp $(OUTDIR)/test_plugin
	@echo "\033[1;32m======[build complete]=======\033[0m"

runMessyExample: $(OUTDIR)/messyExample $(OUTDIR)/libOpenMe.so
	@cd $(OUTDIR); LD_LIBRARY_PATH=. ./messyExample

runMemCheck: $(OUTDIR)/messyExample $(OUTDIR)/libOpenMe.so
	@cd $(OUTDIR); LD_LIBRARY_PATH=. valgrind --tool=memcheck --leak-check=yes --track-origins=yes ./messyExample

$(OUTDIR)/test_string: ../Examples/test_string.c $(OUTDIR)/libOpenMe.so
	@echo "\033[1;32m===[building test_string]====\033[0m"
	@$(CC) $(CFLAGS) -o $(OUTDIR)/test_string ../Examples/test_string.c -lOpenMe -I$(UTHASH_INCLUDE) -I$(OPENME_INCLUDE) -L$(OUTDIR) -lglfw -lIL
#@echo "\033[1;32m======[build complete]=======\033[0m"

$(OUTDIR)/test_interp: ../Examples/test_interp.c $(OUTDIR)/libOpenMe.so
	@echo "\033[1;32m===[building test interp]====\033[0m"
	@$(CC) $(CFLAGS) -o $(OUTDIR)/test_interp ../Examples/test_interp.c -lOpenMe -I$(UTHASH_INCLUDE) -I$(OPENME_INCLUDE) -L$(OUTDIR) -lglfw -lIL
#@echo "\033[1;32m======[build complete]=======\033[0m"

$(OUTDIR)/test_plugin: ../Examples/test_plugin.c $(OUTDIR)/libOpenMe.so $(OUTDIR)/plugin.so
	@echo "\033[1;32m===[building test plugin]====\033[0m"
	@$(CC) $(CFLAGS) -o $(OUTDIR)/test_plugin ../Examples/test_plugin.c -lOpenMe -I$(UTHASH_INCLUDE) -I$(OPENME_INCLUDE) -L$(OUTDIR) -lglfw -lIL

$(OUTDIR)/plugin.so: ../Examples/plugin.c ../OpenMe/include/pluginInterface.h
	@echo "\033[1;32m======[building plugin]======\033[0m"
	@$(CC) $(CFLAGS) -o $(OUTDIR)/plugin.so -fPIC -shared -Wl,-soname,plugin.so ../Examples/plugin.c -I$(OPENME_INCLUDE)

#$(OUTDIR)/test_file: ../Examples/test_file.c $(OUTDIR)/libOpenMe.so
#	$(CC) $(CFLAGS) -o $(OUTDIR)/test_file ../Examples/test_file.c -lOpenMe -I$(UTHASH_INCLUDE) -I$(OPENME_INCLUDE) -L$(OUTDIR) -lglfw -lIL

runTests: $(OUTDIR)/test_string $(OUTDIR)/test_interp $(OUTDIR)/test_plugin #$(OUTDIR)/test_file
	@echo "\033[1;32m=======[running tests]=======\033[0m"
	@cd $(OUTDIR);  echo -n "Testing strings:..." && LD_LIBRARY_PATH=. ./test_string && echo "\033[1;32mOK\033[0m"
	@cd $(OUTDIR);  echo -n "Testing plugins:..." && LD_LIBRARY_PATH=. ./test_plugin && echo "\033[1;32mOK\033[0m"
	@cd $(OUTDIR);  echo -n "Testing interpreter:..." && LD_LIBRARY_PATH=. ./test_interp && echo "\033[1;32mOK\033[0m"
#@cd $(OUTDIR);  echo -n "Testing files:..." && LD_LIBRARY_PATH=. ./test_string && echo "\033[1;32mOK\033[0m"

showtodos:
	@grep -rin --color --include *.[hc] TODO ..
	@echo "\n\033[1;31mThere are `grep -ri --include *.[hc] TODO .. | wc -l` TODOs in the source code: still a lot of work!\033[0m"

showlines:
	@find ../OpenMe/ -name "*.[hc]" | xargs wc -l | sort -n

showfunctions: $(OUTDIR)/libOpenMe.so
	@objdump -tT $(OUTDIR)/libOpenMe.so | grep ome[A-Z][[:alnum:]]* | sed 's/\(.*\)\(ome[A-Z][[:alnum:]]*\)\(.*\)/\2/' | sort -u | cat -n

showglcalls:
	@grep -rnE --include "*.[hc]" --color=none "gl[A-Z][[:alnum:]]*\(" .. | sed 's/.*\(gl[A-Z][[:alnum:]]*\).*/\1/' | sort -u | cat -n

$(OUTDIR)/messyExample: ../Examples/messyExample.c $(OUTDIR)/libOpenMe.so
	@echo "\033[1;32m===[building messyExample]===\033[0m"
	@$(CC) $(CFLAGS) -o $(OUTDIR)/messyExample ../Examples/messyExample.c -lOpenMe -I$(UTHASH_INCLUDE) -I$(OPENME_INCLUDE) -L$(OUTDIR) -lglfw -lIL
#@echo "\033[1;32m======[build complete]=======\033[0m"

$(OUTDIR)/libOpenMe.so: ../OpenMe/include/*.h ../OpenMe/src/*.c
	@echo "\033[1;32m====[building libOpenMe]=====\033[0m"
	@$(CC) $(CFLAGS) -o $(OUTDIR)/libOpenMe.so -fPIC -shared -Wl,-soname,libOpenMe.so ../OpenMe/src/*.c -lm -lGLEW -I$(UTHASH_INCLUDE) -I$(OPENME_INCLUDE)
#@echo "\033[1;32m======[build complete]=======\033[0m"

clean:
	@rm -rf ../Debug/*.so ../Debug/messyExample ../Debug/test_*
	@rm -rf ../Release/*.so ../Release/messyExample ../Release/test_*
	@echo "\033[1;32mClean complete\033[0m"

.PHONY: all clean runTests showlines showtodos showglcalls runMessyExample runMemCheck

# @echo "\033[1;32mThis text is green\033[0m"
# @echo "\033[1;31mThat one is red\033[0m"
# interesting for Visual Studio's compiler compatibiliy: -Wdeclaration-after-statement -Werror=deqwqclaration-after-statement
