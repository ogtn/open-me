#  ome: Open Minimalistic Engine
#
#  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the Do What The Fuck You Want To
#  Public License, Version 2, as published by Sam Hocevar. See
#  http://sam.zoy.org/projects/COPYING.WTFPL for more details.

# I hate makefiles, this is why that one is so crappy

CC=LC_ALL=en_US.UTF-8 colorgcc
#CC=gcc

# hardcore mode, I save it for later :)
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
-Wwrite-strings #-ansi -pedantic

# easy version
#CWARN=-Wall -Wextra

CFLAGS=$(CWARN) -g -DDEBUG

LDFLAGS=

OPENME_INCLUDE=../OpenMe/include
UTHASH_INCLUDE=../uthash

all: ../Debug/messyExample

runMessyExample: ../Debug/messyExample ../Debug/libOpenMe.so
	@cd ../Debug; LD_LIBRARY_PATH=. ./messyExample

runMemCheck: ../Debug/messyExample ../Debug/libOpenMe.so
	@cd ../Debug; LD_LIBRARY_PATH=. valgrind --tool=memcheck --leak-check=yes ./messyExample

../Debug/test_string: ../Examples/test_string.c ../Debug/libOpenMe.so
	$(CC) $(CFLAGS) -o ../Debug/test_string ../Examples/test_string.c -lOpenMe -I$(UTHASH_INCLUDE) -I$(OPENME_INCLUDE) -L../Debug -lglfw -lIL

../Debug/test_file: ../Examples/test_file.c ../Debug/libOpenMe.so
	$(CC) $(CFLAGS) -o ../Debug/test_file ../Examples/test_file.c -lOpenMe -I$(UTHASH_INCLUDE) -I$(OPENME_INCLUDE) -L../Debug -lglfw -lIL

runTests: ../Debug/test_string #../Debug/test_file
	@cd ../Debug;  echo -n "Testing strings:..." && LD_LIBRARY_PATH=. ./test_string && echo "\033[1;32mOK\033[0m"
#@cd ../Debug;  echo -n "Testing files:..." && LD_LIBRARY_PATH=. ./test_string && echo "\033[1;32mOK\033[0m"

showTODOs:
	@grep -rin --color --include *.[hc] TODO ..
	@echo "\n\033[1;31m### There are `grep -ri --include *.[hc] TODO .. | wc -l` TODOs in the source code: still a lot of work! ###\033[0m"

../Debug/messyExample: ../Examples/messyExample.c ../Debug/libOpenMe.so
	$(CC) $(CFLAGS) -o ../Debug/messyExample ../Examples/messyExample.c -lOpenMe -I$(UTHASH_INCLUDE) -I$(OPENME_INCLUDE) -L../Debug -lglfw -lIL

../Debug/libOpenMe.so: ../OpenMe/include/*.h ../OpenMe/src/*.c
	$(CC) $(CFLAGS) -o ../Debug/libOpenMe.so -fPIC -shared -Wl,-soname,libOpenMe.so ../OpenMe/src/*.c -lm -lGLEW -I$(UTHASH_INCLUDE) -I$(OPENME_INCLUDE)

clean:
	@rm -rf ../Debug/libOpenMe.so ../Debug/messyExample ../Debug/test_*
	@echo "\033[1;32mClean complete\033[0m"

.PHONY: all clean

# @echo "\033[1;32mThis text is green\033[0m"
# @echo "\033[1;31mThat one is red\033[0m"
# interesting for Visual Studio's compiler compatibiliy: -Wdeclaration-after-statement -Werror=deqwqclaration-after-statement
