#include"udp.h"

// create a socket and bind it to a port on the current machine
// used to listen for incoming packets
int UDP_Open(int port){
    int fd;
    /*
    int socket(int domain, int type, int protocol);

    socket() creates an endpoint for communication and returns a file
       descriptor that refers to that endpoint.  The file descriptor
       returned by a successful call will be the lowest-numbered file
       descriptor not currently open for the process.
       
    The domain argument specifies a communication domain; this selects
       the protocol family which will be used for communication.  These
       families are defined in <sys/socket.h>. 
       
    AF_INET6     IPv6 Internet protocols 
    
    The socket has the indicated type, which specifies the communication
       semantics.
       
    SOCK_DGRAM      Supports datagrams (connectionless, unreliable
        messages of a fixed maximum length).
                       
    The protocol specifies a particular protocol to be used with the
       socket.  Normally only a single protocol exists to support a
       particular socket type within a given protocol family, in which case
       protocol can be specified as 0
    */
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
	    perror("socket");
	    return 0;
    }

    // set up the bind
    struct sockaddr_in myaddr;
    /*The bzero() function erases the data in the n bytes of the memory
    starting at the location pointed to by s, by writing zeros (bytes
    containing '\0') to that area.*/
    bzero(&myaddr, sizeof(myaddr));

    myaddr.sin_family      = AF_INET;
    myaddr.sin_port        = htons(port);//The htons() function converts the unsigned short integer hostshort from host byte order to network byte order.
    myaddr.sin_addr.s_addr = INADDR_ANY;

    /*After creation of the socket, bind function binds the socket to the address 
    and port number specified in addr(custom data structure). In the example 
    code, we bind the server to the localhost, hence we use INADDR_ANY to 
    specify the IP address.*/
    if (bind(fd, (struct sockaddr *) &myaddr, sizeof(myaddr)) == -1) {
	    perror("bind");
	    close(fd);
	    return -1;
    }

    // give back descriptor
    return fd;
}

// fill sockaddr_in struct with proper goodies
int UDP_FillSockAddr(struct sockaddr_in *addr, char *hostName, int port){
    bzero(addr, sizeof(struct sockaddr_in));
    if (hostName == NULL) {
	    return 0; // it's OK just to clear the address
    }
    
    addr->sin_family = AF_INET;          // host byte order
    addr->sin_port   = htons(port);      // short, network byte order

    struct in_addr *inAddr;
    struct hostent *hostEntry;
    if ((hostEntry = gethostbyname(hostName)) == NULL) {
	    perror("gethostbyname");
	    return -1;
    }
    inAddr = (struct in_addr *) hostEntry->h_addr;
    addr->sin_addr = *inAddr;

    // all is good
    return 0;
}

int UDP_Write(int fd, struct sockaddr_in *addr, char *buffer, int n){
    int addrLen = sizeof(struct sockaddr_in);
    int rc = sendto(fd, buffer, n, 0, (struct sockaddr *) addr, addrLen);
    return rc;
}
/*
The recvfrom() function shall receive a message from a connection-mode or 
connectionless-mode socket. It is normally used with connectionless-mode sockets
because it permits the application to retrieve the source address of received data.

The recvfrom() function takes the following arguments:

1. socket Specifies the socket file descriptor.
2. buffer Points to the buffer where the message should be stored.
3. length Specifies the length in bytes of the buffer pointed to by the buffer argument.
4. flags Specifies the type of message reception. Values of this argument are 
formed by logically OR'ing zero.
5. A null pointer, or points to a sockaddr structure in which the sending 
address is to be stored. The length and format of the address depend on the 
address family of the socket.
6. address_len Specifies the length of the sockaddr structure pointed to by the 
address argument.

The recvfrom() function shall return the length of the message written to the 
buffer pointed to by the buffer argument.*/
int UDP_Read(int fd, struct sockaddr_in *addr, char *buffer, int n){
    int len = sizeof(struct sockaddr_in); 
    int rc = recvfrom(fd, buffer, n, 0, (struct sockaddr *) addr, (socklen_t *) &len);
    // assert(len == sizeof(struct sockaddr_in)); 
    return rc;
}


int UDP_Close(int fd){
    return close(fd);
}
