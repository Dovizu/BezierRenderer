RM = /bin/rm -f
BUILD = build
SRC = source
LIB = libraries
DEBUG = -DDEBUG

CIMG = -I$(LIB)/CImg/ -I/opt/X11/include -L/opt/X11/lib -lX11 -I/usr/local/bin/ -lpthread
EIGEN = -I$(LIB)/Eigen/
GLFW_COMP = -I$(LIB)/GLFW/include
GLFW_LINK = -L$(LIB)/GLFW/lib -lglfw3 -framework Cocoa -framework IOKit -framework CoreVideo
GLEW_COMP = -I$(LIB)/GLEW/include -DGLEW_STATIC
GLEW_LINK = -L$(LIB)/GLEW/lib -lGLEW

ifeq ($(shell sw_vers 2>/dev/null | grep Mac | awk '{ print $$2}'),Mac)
    CC = clang++ -std=c++11 -stdlib=libc++ -O3
	CFLAGS = -g -c -Wall -Wno-deprecated $(EIGEN) $(GLFW_COMP) $(GLEW_COMP) $(DEBUG)
	LFLAGS = -Wall $(GLFW_LINK) $(GLEW_LINK) -framework OpenGL -framework GLUT
else

endif

PRODUCT = renderBezier
OBJS = $(BUILD)/utilities.o $(BUILD)/renderBezier.o $(BUILD)/BEZParser.o $(BUILD)/UniformTessellation.o $(BUILD)/Tessellation.o $(BUILD)/AdaptiveTessellation.o

renderBezier: renderBezier.o BEZParser.o utilities.o UniformTessellation.o Tessellation.o AdaptiveTessellation.o
	$(CC) $(LFLAGS) $(OBJS) -o $(PRODUCT)

renderBezier.o: $(SRC)/renderBezier.cpp
	$(CC) $(CFLAGS) $(SRC)/renderBezier.cpp -o $(BUILD)/renderBezier.o

BEZParser.o: $(SRC)/BEZParser.cpp
	$(CC) $(CFLAGS) $(SRC)/BEZParser.cpp -o $(BUILD)/BEZParser.o

utilities.o: $(SRC)/utilities.cpp
	$(CC) $(CFLAGS) $(SRC)/utilities.cpp -o $(BUILD)/utilities.o
    
UniformTessellation.o: $(SRC)/UniformTessellation.cpp
	$(CC) $(CFLAGS) $(SRC)/UniformTessellation.cpp -o $(BUILD)/UniformTessellation.o

AdaptiveTessellation.o: $(SRC)/AdaptiveTessellation.cpp
	$(CC) $(CFLAGS) $(SRC)/AdaptiveTessellation.cpp -o $(BUILD)/AdaptiveTessellation.o
    
Tessellation.o: $(SRC)/Tessellation.cpp
	$(CC) $(CFLAGS) $(SRC)/Tessellation.cpp -o $(BUILD)/Tessellation.o

clean:
	$(RM) $(BUILD)/*.o $(PRODUCT)