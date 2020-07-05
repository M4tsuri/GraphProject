CC=gcc
OBJECTS=stats.o tools.o search.o priority.o main.o cli.o

search-cli: $(OBJECTS)
	$(CC) -o search-cli $(OBJECTS)

stats.o: 
	$(CC) -c stats.c

main.o:
	$(CC) -c main.c

tools.o:
	$(CC) -c tools.c

priority.o: 
	$(CC) -c priority.c

search.o:
	$(CC) -masm=intel -c search.c

cli.o:
	$(CC) -c cli.c

clean:
	rm -rf $(OBJECTS) search-cli
