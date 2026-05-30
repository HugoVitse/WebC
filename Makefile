webc:
	mkdir -p build
	gcc -Wall -Wextra -g -o build/webc src/*.c
webc-release:
	mkdir -p build/release
	gcc -o build/release/webc src/*.c
	python3 onefiler.py ./include ./build/release/webc.h ./include/types.h
libwebc:
	python3 onefiler.py ./include ./build/lib/webc.h ./include/types.h
	mkdir -p build/lib
	mkdir -p build/tmp
	gcc -c src/*.c
	mv *.o build/tmp/
	ar rcs build/lib/libwebc.a build/tmp/*.o
	rm -r build/tmp
	mkdir -p myApp/lib/lib
	mkdir -p myApp/lib/include
	cp build/lib/libwebc.a myApp/lib/lib/libwebc.a
	cp build/lib/webc.h myApp/lib/include/webc.h
	cp static/accueil.html myApp/static
	zip -r webc.zip myApp
