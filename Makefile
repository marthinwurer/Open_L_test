EXECUTABLES = cl-demo print-devices mytest

CL_LIBS = -lOpenCL
GL_LIBS = -lGLEW -lglfw3 -lGL -lX11 -lXxf86vm -lpthread -lXrandr -lXinerama -lXcursor -lXi -ldl -lm
CFLAGS = -g -ggdb 

all: $(EXECUTABLES)

ifdef OPENCL_INC
  CL_CFLAGS = -I$(OPENCL_INC)
endif

ifdef OPENCL_LIB
  CL_LDFLAGS = -L$(OPENCL_LIB)
endif

print-devices: print-devices.c cl-helper.c
	gcc $(CL_CFLAGS) $(CL_LDFLAGS) -std=gnu99 -o$@ $^ -lrt -lOpenCL

cl-demo: cl-demo.c cl-helper.c
	gcc $(CL_CFLAGS) $(CL_LDFLAGS) -std=gnu99 -o$@ $^ -lrt -lOpenCL
	
mytest: mytest.c my_cl_helper.c DiamondSquare.c map2d.c utilities.c xoroshiro128plus.c interpolation.c eventqueue.c gl_setup.c
	gcc $(CL_CFLAGS) $(CL_LDFLAGS) -flto -std=gnu99 -g -ggdb -o$@ $^ -lm -lrt -lOpenCL -lSDL2 $(GL_LIBS)

clean:
	rm -f $(EXECUTABLES) *.o
