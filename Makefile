webc:
	mkdir -p build
	gcc -Wall -Wextra -g -o build/webc src/*.c
webc-release:
	mkdir -p build/release
	gcc -o build/release/webc src/*.c
	python3 onefiler.py ./include ./build/release/webc.h ./include/types.h
libwebc:
	mkdir -p build/release
	mkdir -p build/tmp
	gcc -c src/*.c
	mv *.o build/tmp/
	ar rcs build/release/libwebc.a build/tmp/*.o
	rm -r build/tmp
