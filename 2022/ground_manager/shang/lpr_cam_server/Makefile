all:
	gcc -c -fPIC convert.c cam.c 
	gcc -shared -o libcamera.so convert.o cam.o
run:
	gcc main.c -L. -lcamera -ljpeg

clean:
	rm *.o -rf
