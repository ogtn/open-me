interp: interp.c
	@echo "\033[1;32m====[building interpreter]=====\033[0m"
	@LC_ALL=en_US.UTF-8 colorgcc interp.c -o interp -O3 \
	-Wall -Wextra -fno-common -Wdeclaration-after-statement \
	-Wformat=2 -Winit-self -Winline -Wpacked -Wp,-D_FORTIFY_SOURCE=2 -Wpointer-arith \
	-Wlarger-than-65500 -Wmissing-declarations -Wmissing-format-attribute -Wmissing-noreturn \
	-Wmissing-prototypes -Wnested-externs -Wold-style-definition -Wredundant-decls -Wsign-compare \
	-Wstrict-aliasing=2 -Wstrict-prototypes -Wswitch-enum -Wundef -Wunreachable-code \
	-Wunsafe-loop-optimizations -Wwrite-strings -pedantic-errors -ansi -pedantic 

clean:
	@rm -f interp
	@echo "\033[1;32m====[clean complete]=====\033[0m"
