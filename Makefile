OBJS =  Graph.o Reductions.o Alg.o Mis.o ControlUnit.o Util.o
SHAREDLIB = libmis.so
all: libmis.so

CC 	= g++
FLAGS	= -std=c++17 -O3 -c -fPIC

libmis.so: $(OBJS) $(HEADER)
	$(CC) -O3 $(OBJS) -shared -o $(SHAREDLIB)

Util.o: src/Util.cpp
	$(CC) $(FLAGS) src/Util.cpp

Graph.o: src/Graph.cpp
	$(CC) $(FLAGS) src/Graph.cpp

ControlUnit.o: src/ControlUnit.cpp
	$(CC) $(FLAGS) src/ControlUnit.cpp

Reductions.o: src/Reductions.cpp
	$(CC) $(FLAGS) src/Reductions.cpp

Alg.o: src/Alg.cpp
	$(CC) $(FLAGS) src/Alg.cpp

Mis.o: src/Mis.cpp
	$(CC) $(FLAGS) src/Mis.cpp

clean:
	rm -f mis $(OBJS) $(SHAREDLIB)

count:
	wc -l src/*.cpp src/*.hpp

install: libmis.so
	install -d build/lib
	intall -m 644 libmis.so build/lib/
	install -d build/include
	install -m 644 src/Jlwrapper.h build/include/

