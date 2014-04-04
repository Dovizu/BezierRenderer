RM = /bin/rm -f
BUILD = build
SRC = source
LIB = libraries

CIMG = -I$(LIB)/CImg/ -I/opt/X11/include -L/opt/X11/lib -lX11 -I/usr/local/bin/ -lpthread
EIGEN = -I$(LIB)/Eigen/

ifeq ($(shell sw_vers 2>/dev/null | grep Mac | awk '{ print $$2}'),Mac)
    CC = clang++ -std=c++11 -stdlib=libc++
	CFLAGS = -g -c -Wall -Wno-deprecated $(EIGEN)
	LFLAGS = -Wall
else

endif

PRODUCT = renderBezier
OBJS = $(BUILD)/renderBezier.o $(BUILD)/BEZParser.o

renderBezier: renderBezier.o BEZParser.o
	$(CC) $(LFLAGS) $(OBJS) -o $(PRODUCT)

renderBezier.o: $(SRC)/renderBezier.cpp
	$(CC) $(CFLAGS) $(SRC)/renderBezier.cpp -o $(BUILD)/renderBezier.o

BEZParser.o: $(SRC)/BEZParser.cpp
	$(CC) $(CFLAGS) $(SRC)/BEZParser.cpp -o $(BUILD)/BEZParser.o

clean:
	$(RM) *.o renderBezier build/renderBezier.o