output: build_network.o simlib.o main.o
	gcc build_network.o simlib.o main.o -o cpssim 

main.o: ./src/Configuration/main.c ./src/Configuration/build_network.h
	gcc -c ./src/Configuration/main.c

build_network.o: ./src/Configuration/build_network.h ./src/Configuration/build_network.c ./src/Sim/simlib.h
	gcc -c ./src/Configuration/build_network.c

simlib.o: ./src/Sim/simlib.c ./src/Sim/simlib.h
	gcc -c ./src/Sim/simlib.c

clean: 
	rm *.o cpssim

