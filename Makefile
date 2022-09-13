.default: all

all: Task1 Task2 Task3

clean:
	rm -f Task1 Task2 Task3 *.o

Task1: Task1.cpp
	g++ -Wall -Werror -std=c++14 -g -O -o Task1 $^

Task2: Task2.cpp
	g++ -Wall -Werror -std=c++14 -g -O -o Task2 $^

Task3: Task3.cpp
	g++ -Wall -Werror -std=c++14 -g -pthread -O -o Task3 $^