#  ome: Open Minimalistic Engine
#
#  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the Do What The Fuck You Want To
#  Public License, Version 2, as published by Sam Hocevar. See
#  http://sam.zoy.org/projects/COPYING.WTFPL for more details.

# I hate makefiles, this is why that one is so crappy

all: ../Debug/messyExample

runMessyExample: ../Debug/messyExample ../Debug/libOpenMe.so
	@cd ../Debug; LD_LIBRARY_PATH=. ./messyExample

../Debug/test_string: ../Examples/test_string.c ../Debug/libOpenMe.so
	gcc -Wall -Wextra -o ../Debug/test_string ../Examples/test_string.c -lOpenMe -I../OpenMe/include/ -L../Debug -lglfw -lIL

../Debug/test_file: ../Examples/test_file.c ../Debug/libOpenMe.so
	gcc -Wall -Wextra -o ../Debug/test_file ../Examples/test_file.c -lOpenMe -I../OpenMe/include/ -L../Debug -lglfw -lIL

runTests: ../Debug/test_string ../Debug/test_file
	@cd ../Debug;  echo -n "Testing strings:..." && LD_LIBRARY_PATH=. ./test_string && echo "\033[1;32mOK\033[0m"
	@cd ../Debug;  echo -n "Testing files:..." && LD_LIBRARY_PATH=. ./test_string && echo "\033[1;32mOK\033[0m"

showTODOs:
	@grep -rin --color --include *.[hc] TODO ..
	@echo "There are `grep -ri --include *.[hc] TODO .. | wc -l` TODOs in the source code: still a lot of work!"

../Debug/messyExample: ../Examples/messyExample.c ../Debug/libOpenMe.so
	gcc -Wall -Wextra -o ../Debug/messyExample ../Examples/messyExample.c -lOpenMe -I../OpenMe/include/ -L../Debug -lglfw -lIL

../Debug/libOpenMe.so: ../OpenMe/include/*.h ../OpenMe/src/*.c
	gcc -Wall -Wextra -o ../Debug/libOpenMe.so -fPIC -shared -Wl,-soname,libOpenMe.so ../OpenMe/src/*.c -lm -lGLEW -I../OpenMe/include/

clean:
	@rm -rf ../Debug/libOpenMe.so ../Debug/messyExample ../Debug/test_*
	@echo "\033[1;32mClean complete\033[0m"

# @echo "\033[1;32mThis text is green\033[0m"
# @echo "\033[1;31mThat one is red\033[0m"
# interesting for Visual Studio's compiler compatibiliy: -Wdeclaration-after-statement -Werror=declaration-after-statement
