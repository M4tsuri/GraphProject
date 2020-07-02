search-cli: main.o stats.o
	cc -o search-cli main.o stats.o

main.o:
	cc -c main.c

stats.o:
	cc -c stats.c

clean:

