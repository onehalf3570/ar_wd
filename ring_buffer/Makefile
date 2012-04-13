all: rtest

ring_buffer.o: ring_buffer.c

rtest.o: rtest.c

rtest: rtest.o ring_buffer.o

clean:
	rm -rf *.o rtest
