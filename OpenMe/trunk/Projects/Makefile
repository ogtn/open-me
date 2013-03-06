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

CC=LC_ALL=en_US.UTF-8 colorgcc
#CC=gcc

# hardcore mode :)
# http://blogs.gnome.org/otte/2008/12/22/warning-options/
# http://mces.blogspot.fr/2008/12/year-end-cleaning-ie-on-warning-options.html
# http://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
CWARN=-Wall -Wextra -fno-common -Wdeclaration-after-statement \
-Wformat=2 -Winit-self -Winline -Wpacked -Wp,-D_FORTIFY_SOURCE=2 \
-Wpointer-arith -Wlarger-than-65500 -Wmissing-declarations \
-Wmissing-format-attribute -Wmissing-noreturn -Wmissing-prototypes \
-Wnested-externs -Wold-style-definition -Wredundant-decls \
-Wsign-compare -Wstrict-aliasing=2 -Wstrict-prototypes -Wswitch-enum \
-Wundef -Wunreachable-code -Wunsafe-loop-optimizations \
-Wwrite-strings #-pedantic-errors #-ansi -pedantic

# easy version
#CWARN=-Wall -Wextra

# release by default
ifdef DEBUG
CFLAGS=$(CWARN) -g -DDEBUG
OUTDIR=../Debug
else
CFLAGS=$(CWARN) -O3
OUTDIR=../Release
endif

# private access checking activated by default
ifndef NOCHECK
CFLAGS+=-DOME_CHECK_PRIVACY
endif

LDFLAGS=

OPENME_INCLUDE=../OpenMe/include
UTHASH_INCLUDE=../uthash

all: $(OUTDIR)/messyExample $(OUTDIR)/test_string $(OUTDIR)/test_interp
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

#$(OUTDIR)/test_file: ../Examples/test_file.c $(OUTDIR)/libOpenMe.so
#	$(CC) $(CFLAGS) -o $(OUTDIR)/test_file ../Examples/test_file.c -lOpenMe -I$(UTHASH_INCLUDE) -I$(OPENME_INCLUDE) -L$(OUTDIR) -lglfw -lIL

runTests: $(OUTDIR)/test_string $(OUTDIR)/test_interp #$(OUTDIR)/test_file
	@cd $(OUTDIR);  echo -n "Testing strings:..." && LD_LIBRARY_PATH=. ./test_string && echo "\033[1;32mOK\033[0m"
	@cd $(OUTDIR);  echo -n "Testing interpreter:..." && LD_LIBRARY_PATH=. ./test_interp && echo "\033[1;32mOK\033[0m"
#@cd $(OUTDIR);  echo -n "Testing files:..." && LD_LIBRARY_PATH=. ./test_string && echo "\033[1;32mOK\033[0m"

showTODOs:
	@grep -rin --color --include *.[hc] TODO ..
	@echo "\n\033[1;31mThere are `grep -ri --include *.[hc] TODO .. | wc -l` TODOs in the source code: still a lot of work!\033[0m"

$(OUTDIR)/messyExample: ../Examples/messyExample.c $(OUTDIR)/libOpenMe.so
	@echo "\033[1;32m===[building messyExample]===\033[0m"
	@$(CC) $(CFLAGS) -o $(OUTDIR)/messyExample ../Examples/messyExample.c -lOpenMe -I$(UTHASH_INCLUDE) -I$(OPENME_INCLUDE) -L$(OUTDIR) -lglfw -lIL
#@echo "\033[1;32m======[build complete]=======\033[0m"

$(OUTDIR)/libOpenMe.so: ../OpenMe/include/*.h ../OpenMe/src/*.c
	@echo "\033[1;32m====[building libOpenMe]=====\033[0m"
	@$(CC) $(CFLAGS) -o $(OUTDIR)/libOpenMe.so -fPIC -shared -Wl,-soname,libOpenMe.so ../OpenMe/src/*.c -lm -lGLEW -I$(UTHASH_INCLUDE) -I$(OPENME_INCLUDE)
#@echo "\033[1;32m======[build complete]=======\033[0m"

clean:
	@rm -rf ../Debug/libOpenMe.so ../Debug/messyExample ../Debug/test_*
	@rm -rf ../Release/libOpenMe.so ../Release/messyExample ../Release/test_*
	@echo "\033[1;32mClean complete\033[0m"

.PHONY: all clean

# @echo "\033[1;32mThis text is green\033[0m"
# @echo "\033[1;31mThat one is red\033[0m"
# interesting for Visual Studio's compiler compatibiliy: -Wdeclaration-after-statement -Werror=deqwqclaration-after-statement
