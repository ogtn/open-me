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
	
showTODOs:
	@grep -rin --include *.[hc] TODO .. | sed -n 's%.*/\([^/]*:[^:]*:.*\)%\1%p'
	@echo "There are `grep -ri --include *.[hc] TODO .. | wc -l` TODOs in the source code: still a lot of work!"

../Debug/messyExample: ../Examples/messyExample.c ../Debug/libOpenMe.so
	gcc -Wall -Wextra -o ../Debug/messyExample ../Examples/messyExample.c -lOpenMe -I../OpenMe/include/ -L../Debug -lglfw -lIL

../Debug/libOpenMe.so: ../OpenMe/include/*.h ../OpenMe/src/*.c
	gcc -Wall -Wextra -o ../Debug/libOpenMe.so -fPIC -shared -Wl,-soname,libOpenMe.so ../OpenMe/src/*.c -lm -lGLEW -I../OpenMe/include/

clean:
	@rm -rf ../Debug/libOpenMe.so ../Debug/messyExample
	@echo "Clean complete"
