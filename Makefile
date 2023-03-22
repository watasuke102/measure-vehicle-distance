all: b r
b:
	clear && cmake --build build
r:
	gdb -q --batch \
   -ex "set print thread-events off" \
   -ex "run" -ex "bt" --args \
	 ./build/capture-cam
