CC   = gcc -Wno-nullability-completeness
OPTS = -Wall

all: server client myServer

# this generates the target executables
server: server.o udp.o
	$(CC) -o server server.o udp.o 

client: client.o udp.o
	$(CC) -o client client.o udp.o 

myServer: myServer.o udp.o
	$(CC) -o myServer myServer.o udp.o 

# this is a generic rule for .o files 
%.o: %.c 
	$(CC) $(OPTS) -c $< -o $@

clean:
	rm -f server.o udp.o client.o myServer.o server client myServer