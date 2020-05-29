#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h> 
#include<errno.h> 
#include"udp.h"

//// =========== 练习 file operation

int main(int argc, char *argv[]){
  //Your file server will store all of its data in an on-disk 
  //file which will be referred to as the file system image . 
  //This image contains the on-disk representation of your data 
  //structures; you should use these system calls to access it:
  // open(), read(), write(), lseek(), close(), fsync() .

  //server [portnum] [file-system-image]
  if(argc<3){
    printf("Usage: server server-portnum file-system-image\n");
    exit(1);
  }

  int portid = atoi(argv[1]);
  int sd = UDP_Open(portid);
  assert(sd > -1);

  printf("Server is listening\n");
  
  // int fd = open(argv[2], O_RDWR | O_CREAT);
  int fd = open("image", O_RDWR | O_CREAT);
  
  if(fd == -1){
    perror("File Descriptor");
    return -1;
  }

  //问题是怎么找到光标 -> lseek()


}