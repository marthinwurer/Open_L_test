EXECUTABLES = cl-demo print-devices mytest

CL_LIBS = -lOpenCL
GL_LIBS = -lGLEW -lglfw3 -lGL -lX11 -lXxf86vm -lpthread -lXrandr -lXinerama -lXcursor -lXi -ldl -lm

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
	
mytest: mytest.c cl-helper.c DiamondSquare.c map2d.c utilities.c xoroshiro128plus.c interpolation.c
	gcc $(CL_CFLAGS) $(CL_LDFLAGS) -std=gnu99 -g -ggdb -o$@ $^ -lm -lrt -lOpenCL -lSDL2 $(GL_LIBS)

clean:
	rm -f $(EXECUTABLES) *.o
