CC=gcc
OBJECTS=stats.o tools.o search.o priority.o main.o

search-cli: $(OBJECTS)
	$(CC) -o search-cli $(OBJECTS)

stats.o: graph.h search.h tools.h
	$(CC) -c stats.c

main.o: tools.h
	$(CC) -c main.c

tools.o: stats.h search.h
	$(CC) -c tools.c

priority.o: 
	$(CC) -c priority.c

search.o: graph.h stats.h priority.h tools.h
	$(CC) -masm=intel -c search.c

clean:
	rm -rf $(OBJECTS) search-cli
