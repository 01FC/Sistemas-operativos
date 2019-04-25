CC=gcc
all: leibniz_serial leibniz_hilos leibniz_clone leibniz_fork

leibniz_serial: leibniz_serial.c
	$(CC) -o leibniz_serial leibniz_serial.c -lm

leibniz_hilos: leibniz_hilos.c
	$(CC) -o leibniz_hilos leibniz_hilos.c -lm -lpthread

leibniz_clone: leibniz_clone.c
	$(CC) -o leibniz_clone leibniz_clone.c -lm -lpthread

leibniz_fork: leibniz_fork.c
	$(CC) -o leibniz_fork leibniz_fork.c -lm

clean:
	rm leibniz_serial leibniz_hilos leibniz_clone leibniz_fork
