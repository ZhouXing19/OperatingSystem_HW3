#include"udp.h"
#include"mfs.h"
#include<stdio.h>

#define CMD_SIZE (4096) //要改， 但是我没想好是多少
#define BUFFER_SIZE (4096)
char buffer[BUFFER_SIZE];

/*
MFS_Init() takes a host name and port number and uses those to find the server 
exporting the file system.*/

int sd; // fd which is a soccket
int client_port_num = 20000;
struct sockaddr_in addr, addr2; //用于发消息 & 收消息的socket

int MFS_Init(char *hostname, int port){
  sd = UDP_Open(client_port_num);
  assert(sd > -1);
  int rc = UDP_FillSockAddr(&addr, hostname, port);//contact server at specified port
  assert(rc == 0);
  return 0;
}
//MFS_Lookup() takes the parent inode number 
//(which should be the inode number of a directory) 
//and looks up the entry name in it. The inode number
//of name is returned. 
//Success: return inode number of name; 
//failure: return -1. 
//Failure modes: invalid pinum, name does not exist in pinum.
int MFS_Lookup(int pinum, char *name){
  char cmd[CMD_SIZE];
  char para1[sizeof(pinum)+1];
  sprintf(para1, "%d", pinum);

  strcat(cmd, "Lookup");
  strcat(cmd, "##");
  strcat(cmd, para1);
  strcat(cmd, "##");
  strcat(cmd, name);
  strcat(cmd, "##");

  int rc = UDP_Write(sd, &addr, cmd, CMD_SIZE); //write message to server@specified-port
  // printf("CLIENT:: sent LOOKUP command (%d), pinum (%d), path name (%s)\n", rc, pinum, name);

  if (rc > 0) {
	    int rc = UDP_Read(sd, &addr2, buffer, BUFFER_SIZE); //read message from ...
	    // printf("CLIENT:: read %d bytes (message: '%s')\n", rc, buffer);
    }

  int result = atoi(buffer);
  return result;
}

//MFS_Stat() returns some information about the file specified 
//by inum. Upon success, return 0, otherwise -1. 
//The exact info returned is defined by MFS_Stat_t. 
//Failure modes: inum does not exist.
int MFS_Stat(int inum, MFS_Stat_t *m){
    // 返回值放在m中吗？
    char cmd[CMD_SIZE];
    char para1[sizeof(inum)+1];
    sprintf(para1, "%d", inum);

    strcat(cmd, "Stat");
    strcat(cmd, "##");
    strcat(cmd, para1);
    strcat(cmd, "##");

    int rc = UDP_Write(sd, &addr, cmd, CMD_SIZE);
    if (rc > 0) {
	    int rc = UDP_Read(sd, &addr2, buffer, BUFFER_SIZE); //read message from ...
    }
    // 定义返回值格式， 放进m中
    // int type;   // MFS_DIRECTORY or MFS_REGULAR
    // int size;   // bytes
    // int blocks; // number of blocks allocated to file
    return 0;
}

int MFS_Write(int inum, char *buffer, int block){
  char cmd[CMD_SIZE];
  char para1[sizeof(inum)+1];
  sprintf(para1, "%d", inum);
  char para3[sizeof(block)+1];
  sprintf(para3, "%d", inum);

  strcat(cmd, "Write");
  strcat(cmd, "##");
  strcat(cmd, para1);
  strcat(cmd, "##");
  strcat(cmd, buffer);
  strcat(cmd, "##");
  strcat(cmd, para3);
  strcat(cmd, "##");

  int rc = UDP_Write(sd, &addr, cmd, CMD_SIZE);
  if (rc > 0) {
	    int rc = UDP_Read(sd, &addr2, buffer, BUFFER_SIZE); //read message from ...
  }
  return atoi(buffer);
} 
//MFS_Write() writes a block of size 4096 bytes at the block 
//offset specified by block . 
//Returns 0 on success, -1 on failure. 
//Failure modes: invalid inum, invalid block, 
//not a regular file (you can't write to directories).

int MFS_Read(int inum, char *buffer, int block){
  char cmd[CMD_SIZE];
  char para1[sizeof(inum)+1];
  sprintf(para1, "%d", inum);
  char para3[sizeof(block)+1];
  sprintf(para3, "%d", block);
  
  strcat(cmd, "Read");
  strcat(cmd, "##");
  strcat(cmd, para1);
  strcat(cmd, "##");
  strcat(cmd, para3);
  strcat(cmd, "##");
  
  int rc = UDP_Write(sd, &addr, cmd, CMD_SIZE);
  if (rc > 0) {
	    int rc = UDP_Read(sd, &addr2, buffer, BUFFER_SIZE); //read message from ...
  }
  return atoi(buffer);
  // return 格式： 1/0 ## buffer
} 
//MFS_Read() reads a block specified by block into the buffer 
//from file specified by inum . 
//The routine should work for either a file or directory; 
//directories should return data in the format specified 
//by MFS_DirEnt_t. Success: 0, failure: -1. 
//Failure modes: invalid inum, invalid block.

int MFS_Creat(int pinum, int type, char *name){
  char cmd[CMD_SIZE];
  char para1[sizeof(pinum)+1];
  sprintf(para1, "%d", pinum);
  char para2[sizeof(type)+1];
  sprintf(para2, "%d", type);

  strcat(cmd, "Creat");
  strcat(cmd, "##");
  strcat(cmd, para1);
  strcat(cmd, "##");
  strcat(cmd, para2);
  strcat(cmd, "##");
  strcat(cmd, name);
  strcat(cmd, "##");
  
  int rc = UDP_Write(sd, &addr, cmd, CMD_SIZE);
  if (rc > 0) {
	    int rc = UDP_Read(sd, &addr2, buffer, BUFFER_SIZE); //read message from ...
  }
  return atoi(buffer);
} 

//MFS_Creat() makes a file ( type == MFS_REGULAR_FILE) 
//or directory ( type == MFS_DIRECTORY) in the parent directory 
//specified by pinum of name name . 
//Returns 0 on success, -1 on failure. 
//Failure modes: pinum does not exist. 
//If name already exists, return success (think about why).

int MFS_Unlink(int pinum, char *name){
  char cmd[CMD_SIZE];
  char para1[sizeof(pinum)+1];
  sprintf(para1, "%d", pinum);

  strcat(cmd, "Unlink");
  strcat(cmd, "##");
  strcat(cmd, para1);
  strcat(cmd, "##");
  strcat(cmd, name);
  strcat(cmd, "##");

  int rc = UDP_Write(sd, &addr, cmd, CMD_SIZE);
  if (rc > 0) {
	    int rc = UDP_Read(sd, &addr2, buffer, BUFFER_SIZE); //read message from ...
  }
  return atoi(buffer);
} 
//MFS_Unlink() removes the file or directory name 
//from the directory specified by pinum . 
//0 on success, -1 on failure. 
//Failure modes: pinum does not exist, 
//pinum does not represent a directory, 
//the to-be-unlinked directory is NOT empty. 
//Note that the name not existing is NOT a failure by our definition 
//(think about why this might be).
