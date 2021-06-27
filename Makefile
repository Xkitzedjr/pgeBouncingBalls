CC = g++
CFLAGS = -lX11 -lGL -lpthread -lpng -std=c++17
SRCDIR = src/

all: proggy

debug: CFLAGS += -DDEBUG
debug: proggy

proggy: main.o Sim.o PhysObj.o olcPixelGameEngine.o Transform.o
	$(CC) $(CFLAGS) -g main.o Sim.o PhysObj.o olcPixelGameEngine.o Transform.o -o proggy -lstdc++fs

main.o: $(SRCDIR)main.cpp
	$(CC) $(CFLAGS) -g -c $(SRCDIR)main.cpp -lstdc++fs

Sim.o: $(SRCDIR)Sim.cpp $(SRCDIR)Sim.h
	$(CC) $(CFLAGS) -g -c $(SRCDIR)Sim.cpp -lstdc++fs

PhysObj.o: $(SRCDIR)PhysObj.cpp $(SRCDIR)PhysObj.h
	$(CC) $(CFLAGS) -g -c $(SRCDIR)PhysObj.cpp -lstdc++fs

Transform.o: $(SRCDIR)Transform.cpp $(SRCDIR)Transform.h
	$(CC) $(CFLAGS) -g -c $(SRCDIR)Transform.cpp -lstdc++fs

olcPixelGameEngine.o: $(SRCDIR)olcPixelGameEngine.h $(SRCDIR)olcPixelGameEngine.cpp
	$(CC) $(CFLAGS) -c $(SRCDIR)olcPixelGameEngine.cpp -lstdc++fs

clean:
	rm proggy;\
	rm *.o
