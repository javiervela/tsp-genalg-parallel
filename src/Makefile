TARGETS = main 

TSPLIB = ./TSPLIB/tsplib
GENETIC = ./Genetic/genetic
TSPLIB_H = ./TSPLIB/
GENETIC_H = ./Genetic/
#CC = g++
CC = mpic++
CFLAGS = -O3 -I$(GENETIC_H) -I$(TSPLIB_H)

all: $(TARGETS)

main: main.o tsplib.o genetic.o
	$(CC) -o $@ main.o $(TSPLIB).o $(GENETIC).o

main.o: main.cpp 
	$(CC) -c $(CFLAGS) -o main.o main.cpp
tsplib.o: $(TSPLIB).cpp $(TSPLIB).h
	$(CC) -c $(CFLAGS) -o $(TSPLIB).o $(TSPLIB).cpp 
genetic.o: $(GENETIC).cpp $(GENETIC).h
	$(CC) -c $(CFLAGS) -o $(GENETIC).o $(GENETIC).cpp -D LOG_LEVEL=1

clean:
	rm -f $(GENETIC).o $(TSPLIB).o $(TARGETS)
