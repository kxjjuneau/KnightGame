all: hw6
hw6: main.cpp Log.o Knight.o Rabbit.o safequeue.o
	g++ -std=c++11 -lpthread main.cpp Log.o Knight.o Rabbit.o safequeue.o -o hw6
safequeue.o: safequeue.cpp safequeue.h
	g++ -std=c++11 -lpthread -c safequeue.cpp -o safequeue.o
Log.o: Log.cpp Log.h
	g++ -std=c++11 -c Log.cpp -o Log.o
Knight.o: Knight.cpp Knight.h
	g++ -std=c++11 -c Knight.cpp -o Knight.o
Rabbit.o: Rabbit.cpp Rabbit.h
	g++ -std=c++11 -c Rabbit.cpp -o Rabbit.o

.PHONY: clean

clean:
	rm *.o
	rm hw6
	
