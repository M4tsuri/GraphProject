search-cli: main.o stats.o
	cc -o search-cli main.o stats.o

main.o: stats.h
	cc -c main.c 

stats.o: stats.h graph.h
	cc -c stats.c

clean:

