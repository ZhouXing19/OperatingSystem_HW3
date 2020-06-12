#include <stdio.h>
#include"udp.h"
#include"mfs.h"

#define BUFFER_SIZE (4096)
char buffer[BUFFER_SIZE];

int main(int argc, char *argv[]){
    if(argc<4)
    {
      printf("Usage: client server-name server-port client-port\n");
      exit(1);
    }
    // int sd = UDP_Open(atoi(argv[3])); //communicate through specified port 
    // assert(sd > -1);

    // struct sockaddr_in addr, addr2;
    // int rc = UDP_FillSockAddr(&addr, argv[1], atoi(argv[2])); //contact server at specified port
    // assert(rc == 0);

    // 测试区 测试区 测试区 测试区 测试区 测试区 测试区 测试区 测试区 测试区
    //int MFS_Init(char *hostname, int port)
    MFS_Init("localhost", 10000);
    // MFS_Creat(int pinum, int type, char *name)
    char * name = "myTestDir";
    int test = MFS_Creat( 1, 0, name);

    // char message[BUFFER_SIZE];
    // sprintf(message, "hello##2##fileName"); //" write ## para1 ## para2"
    // rc = UDP_Write(sd, &addr, message, BUFFER_SIZE); //write message to server@specified-port
    // printf("CLIENT:: sent message (%d)\n", rc);

    // if (rc > 0) {
	  //   int rc = UDP_Read(sd, &addr2, buffer, BUFFER_SIZE); //read message from ...
	  //   printf("CLIENT:: read %d bytes (message: '%s')\n", rc, buffer);
    // }

    return 0;
}

