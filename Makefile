all: b r
b:
	clear && cmake --build build
r:
	DISPLAY=:0 gdb -q --batch \
   -ex "set print thread-events off" \
   -ex "run" -ex "bt" --args \
	 ./build/capture-cam
