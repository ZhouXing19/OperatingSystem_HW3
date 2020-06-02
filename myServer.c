#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h> 
#include<errno.h> 
#include"udp.h"

#define BIT_MAP_SIZE (4096)
#define BIT_SIZE (1)
#define INODE_ARRAY_OFFSET (8193)
#define INODE_SIZE (60)
#define BLOCK_ARRAY_OFFSET (253953)
#define BLOCK_SIZE (4096)
#define MESSAGE_SIZE (4096) //TODO: change size
#define ARG_NUM (4)
#define ID_SIZE (4)
#define DIR_TABLE_ROW (16)
#define DIR_TABLE_ROW_LENGTH (256)
#define DIR_TABLE_ROW_NAME_LENGTH (252)
#define ENTRY_NUM (10)

int fd;

// Part 1
// 1-4096 inode bit map
// 4097-8193 block bit map

// Part 2
// every inode structure 60 byte
// inode id: 1-4096 -> 4 bytes(root inode #1)  -------------------------- 1-4
// type: 0 or 1 -> 4 bytes   -------------------------------------------- 5-8
// size: 0-40,000 -> 8 bytes -------------------------------------------- 9-16
// block field nums: 0 -10 -> 4 bytes ----------------------------------- 17-20
// 10 pointers(each is a block id 0-4095) -> 4x10 bytes ----------------- 21-60

// Part 3:
// every block 4096 bytes

// Directory file format design (4096 = 16row * 256bytes/row):
int clear_inode(int inode_id){
  /*
  1. inode_bit_map set to 0
  2. find the inode structure:
  3. */
  return 0;
}

int create_inode(int type, int size, int blk_num){
  // return inode_id
  /*
  1. search inode_bit_map to see if there is empty seat.
  2. Yes, set the seat to 1, preserve inode id. No, return -1.
  3. In the inode array, clear the original inode structure (clear_inode) --- TODO: check this
  4. Write new info the the inode structure
  5. return this inode_it.*/

  // 1. look through bit map
  lseek(fd, 1, SEEK_SET);
  int inode_id_ptr = 1;
  int inode_id = -1;
  while(inode_id_ptr<=BIT_MAP_SIZE){
    char *c = (char*) calloc(ID_SIZE, sizeof(char));
    if(atoi(c) == 0){
      inode_id = inode_id_ptr;
      break;
    }
    inode_id_ptr++;
  }
  if(inode_id == -1) return inode_id; // inode_bit_map used up
  //set bit map
  lseek(fd, inode_id, SEEK_SET);
  write(fd, "1", 1); // flip bit

  // move to the inode structure
  lseek(fd, INODE_ARRAY_OFFSET+(inode_id-1)*INODE_SIZE, SEEK_SET);
  // set inode id
  char str[ID_SIZE];
  sprintf(str, "%d", inode_id);
  write(fd, str, ID_SIZE);
  //set type
  sprintf(str, "%d", type);
  write(fd, str, ID_SIZE);
  //set size
  char strr[ID_SIZE*2];
  sprintf(str, "%d", size);
  write(fd, strr, ID_SIZE*2);
  //set block number
  sprintf(str, "%d", blk_num);
  write(fd, str, ID_SIZE);

  return inode_id;
}

int create_block(){
  lseek(fd, BIT_MAP_SIZE+1, SEEK_SET);
  int block_id_ptr = 1;
  int block_id = -1;
  while(block_id_ptr<=BIT_MAP_SIZE){
    char *c = (char*) calloc(ID_SIZE, sizeof(char));
    if(atoi(c) == 0){
      block_id = block_id_ptr;
      break;
    }
    block_id_ptr++;
  }
  if(block_id == -1) return block_id; // inode_bit_map used up
  //set bit map
  lseek(fd, BIT_MAP_SIZE + block_id, SEEK_SET);
  write(fd, "1", 1); // flip bit
  return block_id;
}

int LookupDirTable(int block_no, char * name){
  int current_block_base = BLOCK_ARRAY_OFFSET+(block_no-1)*BLOCK_SIZE;
  int row_ptr = 0;
  char *c = (char*) calloc(ID_SIZE, sizeof(char));
  int ret = read(fd, c, ID_SIZE);
  while(row_ptr<DIR_TABLE_ROW){
    lseek(fd, current_block_base+row_ptr*DIR_TABLE_ROW_LENGTH, SEEK_SET);
    ret = read(fd, c, DIR_TABLE_ROW_NAME_LENGTH);
    if(strcmp(name, c) == 0){
      read(fd, c, ID_SIZE);//TODO: check: 真的是顺着读的吗？
      return atoi(c);
    }
    if(strcmp("", c)==0){ //It's right!
      return -1;
    }
    row_ptr++;
  }
  return -1;
}

int Lookup(char* strs[ARG_NUM]){
  int pinum = atoi(strs[1]);
  char * name = strs[2];

  // TODO: return int or void->sendto()
  if(pinum < 1 || pinum > BIT_MAP_SIZE) return -1;

  int inode_offset = INODE_ARRAY_OFFSET+(pinum-1)*INODE_SIZE;
  // check type --------------------------------------------------------
  lseek(fd, inode_offset+5, SEEK_SET); 
  char *c = (char*) calloc(ID_SIZE, sizeof(char));
  int ret = read(fd, c, ID_SIZE); // read type TODO: if ret<0??
  if(atoi(c) != 0) return -1;

  // move to block fields number
  lseek(fd, inode_offset+17, SEEK_SET);
  c = (char*) calloc(ID_SIZE, sizeof(char));
  ret = read(fd, c, ID_SIZE);
  int block_num = atoi(c);

  // search each block
  int block_ptr = 0;
  int ptr_offset = inode_offset+21;
  while(block_ptr<block_num){
    lseek(fd, ptr_offset+block_ptr*ID_SIZE, SEEK_SET);
    c = (char*) calloc(ID_SIZE, sizeof(char));
    ret = read(fd, c, ID_SIZE);
    int block_no = atoi(c);
    int found = LookupDirTable(block_no, name);
    if(found!=-1) return found;
    block_ptr++;
  }
  return -1; //找遍该dir的所有block， 里面都没有child file/dir
}

int Creat(char* strs[ARG_NUM]){
  int pinum = atoi(strs[1]);
  int type = atoi(strs[2]);
  char * name = strs[3];

  // 1. check if parent node exist
  lseek(fd, pinum, SEEK_SET);
  char * c = (char*) calloc(BIT_SIZE, sizeof(char));
  read(fd, c, BIT_SIZE);
  if(atoi(c) == 0) return -1;
  free(c);

  // 2. check all blocks to see if name exists
  // 2-1 go to the pinode structure
  int inode_offset = INODE_ARRAY_OFFSET+ (pinum-1) * INODE_SIZE;
  // 2-2 check if it is a direcory
  lseek(fd, inode_offset+ID_SIZE, SEEK_SET);
  c = (char*) calloc(ID_SIZE, sizeof(char));
  read(fd, c, ID_SIZE);
  if(atoi(c) == 1) return -1; // inode is a file
  free(c);
  // 2-3 find number of entries in the dir
  c = (char*) calloc(ID_SIZE*2, sizeof(char));
  lseek(fd, ID_SIZE*2, SEEK_CUR);
  read(fd, c, ID_SIZE*2);
  int entryNo = atoi(c);
  free(c);
  //2-4 find number of blocks in the dir
  c = (char*) calloc(ID_SIZE, sizeof(char));
  lseek(fd, ID_SIZE, SEEK_CUR);
  read(fd, c, ID_SIZE);
  int block_num = atoi(c);
  free(c);
  //2-5 check duplicate name
  int block_ptr = 0;
  int ptr_offset = inode_offset+21;
  int block_no;
  while(block_ptr<block_num){
    lseek(fd, ptr_offset+block_ptr*ID_SIZE, SEEK_SET);
    c = (char*) calloc(ID_SIZE, sizeof(char));
    read(fd, c, ID_SIZE);
    block_no = atoi(c);
    int found = LookupDirTable(block_no, name);
    if(found != -1) return 0; // SUCCESS! find the name
    block_ptr++;
  }
  //可以有create new file/dir 
  int inode_id = create_inode(type, 0, 0);
  if(inode_id == -1) return -1; //bit-map is full

  //3-1 check if a new block needs to be assigned to the dir
  if(entryNo % DIR_TABLE_ROW != 0){ // inside the block
    int offset = BLOCK_ARRAY_OFFSET+(block_no-1)*BLOCK_SIZE + (entryNo % DIR_TABLE_ROW)*DIR_TABLE_ROW_LENGTH;
    lseek(fd, offset, SEEK_SET);
    write(fd, name, DIR_TABLE_ROW_NAME_LENGTH);
    char str[ID_SIZE];
    sprintf(str, "%d", inode_id);
    write(fd, str, ID_SIZE);
  }
  else{
    // 1. check dir block number <10
    // 2. create a new block
    // 3. change dir inode info: size, block_num, 10 pointers
    // 4. 在新的block里写下name, inode_id
    if(block_num == 10) return -1;
    int block_id = create_block();
    if(block_id == -1) return -1; // no more empty block

    lseek(fd, inode_offset+ID_SIZE*2, SEEK_SET);
    // set new size
    char dirSize[ID_SIZE*2];
    sprintf(dirSize, "%d", entryNo+1);
    write(fd, dirSize, ID_SIZE*2);
    // set new block num
    char dir_block_num[ID_SIZE];
    sprintf(dir_block_num, "%d", block_num+1);
    write(fd, dir_block_num, ID_SIZE);
    //add pointer+1 name
    lseek(fd, block_num*ID_SIZE, SEEK_CUR);
    write(fd, name, DIR_TABLE_ROW_NAME_LENGTH);
    //add pointer+1 id
    char dir_block_id[ID_SIZE];
    sprintf(dir_block_id, "%d", block_id);
    write(fd, dir_block_id, ID_SIZE);
  }
  return 0;
}

int Stat(char* strs[ARG_NUM]){
  return 0;
}

int check_inum(int inum){
  // -1: invalid; i: exist; 0: not exist
  if(inum > BIT_MAP_SIZE) return -1;
  lseek(fd, inum, SEEK_SET);
  char *c = (char*) calloc(1, sizeof(char));
  int res = read(fd, c, 1); 
  return atoi(c);
}

int check_block(int block_no){
  // -1: invalid; i: exist; 0: not exist
  if(block_no<=BIT_MAP_SIZE || block_no > 2*BIT_MAP_SIZE) return -1;
  lseek(fd, BIT_MAP_SIZE+block_no, SEEK_SET);
  char *c = (char*) calloc(1, sizeof(char));
  int res = read(fd, c, 1); 
  return atoi(c);
}

int Read(char* strs[ARG_NUM]){
  int inum = atoi(strs[1]);
  int block_no = atoi(strs[2]);
  if(check_inum(inum) != 1 || check_block(block_no) != 1) return -1;
  int inode_offset = INODE_ARRAY_OFFSET+ (inum-1) * INODE_SIZE;


  // 先找到inode type
  lseek(fd, inode_offset+ID_SIZE, SEEK_SET);
  char *t = (char*) calloc(ID_SIZE, sizeof(char));
  read(fd, t, ID_SIZE); 
  int type = atoi(t);
  free(t);

  //再找 block_no
  lseek(fd, inode_offset+17, SEEK_SET);
  char *c = (char*) calloc(ID_SIZE, sizeof(char));
  read(fd, c, ID_SIZE); 
  int block_num = atoi(c);
  free(c);


  for(int i=0; i<block_num; i++){
    read(fd, c, ID_SIZE); 
    if(atoi(c) == block_no){
      lseek(fd, BLOCK_ARRAY_OFFSET+block_no*BLOCK_SIZE, SEEK_SET);
      char * buffer = (char*) calloc(BLOCK_SIZE, sizeof(char));
      int res = read(fd, buffer, strlen(buffer));
      // shoud return 0##buffer
      char ans[BLOCK_SIZE+5];
      char flag[1];
      sprintf(flag, "%d", 0);
      strcat(ans, flag);
      strcat(ans, "##");
      strcat(ans, buffer);
      strcat(ans, "##");
      // TODO: how to send it back
    }
  }
  return -1; // invalid block_num
}

int Write(char* strs[ARG_NUM]){
  int inum = atoi(strs[1]);
  char * buffer = strs[2];
  int block_no = atoi(strs[3]);
  return 0;
}

int Unlink(char* strs[ARG_NUM]){
  return 0;
}


void parseMessage(char *message){
  char * strs[ARG_NUM];
  printf("Message is %s\n", message);
  int i = 0;
  char * token = strtok(message, "##");
  while(token != NULL){
    // printf("token1 is %s\n", token);
    strs[i] = malloc(strlen(token)+1);
    // printf("token2 is %s\n", token);
    strcpy(strs[i], token);
    // printf("token3 is %s\n", token);
    i++;
    token = strtok(NULL, "##");
  }
  // for(int j=0; j<i; j++){
  //   printf("%s\n", strs[j]);
  // }
  if(strcmp(strs[0], "Lookup")){
    Lookup(strs);
  }
  else if(strcmp(strs[0], "Stat")){
    Stat(strs);
  }
  else if(strcmp(strs[0], "Write")){
    Write(strs);
  }
  else if(strcmp(strs[0], "Read")){
    Read(strs);
  }
  else if(strcmp(strs[0], "Creat")){
    Creat(strs);
  }
  else if(strcmp(strs[0], "Unlink")){
    Unlink(strs);
  }
  else{
    // message format error, send back something?
  }
}


void initImage(int fd){
  //如果有root， ignore; else init
  lseek(fd, 0, SEEK_SET);
  char *c = (char*) calloc(1, sizeof(char));
  read(fd, c, 1); // read the 1st bit in inode_bit_map
  if(atoi(c)!=1){
    create_inode(0, 0, 0); // 默认 /root inode_id is 1
  }
}

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
  
  // int fd = open(argv[2], O_RDWR | O_CREAT);
  fd = open("image", O_RDWR | O_CREAT);
  
  if(fd == -1){
    perror("File Descriptor");
    return -1;
  }

  // initImage(fd);

  printf("Server is listening\n");

  // while(1){
  //   struct sockaddr_in s;
  //   char message[MESSAGE_SIZE];
  //   int rc = UDP_Read(sd, &s, message, MESSAGE_SIZE);
  //   if(rc > 0){
  //     parseMessage(message);
  //   }
  // }

  // 测试 write function 
  int res;
  char* buff = "Hello World!";
  printf("size of char pointer is %d\n", (int)strlen(buff));
  res = write(fd, buff, strlen(buff));
  printf("write result is %d\n", res);
  lseek(fd, 15, SEEK_SET);
  buff = "test distance!";
  res = write(fd, buff, strlen(buff));

  lseek(fd, 15, SEEK_SET);
  char *c = (char*) calloc(BLOCK_SIZE, sizeof(char));
  res = read(fd, c, 15+strlen(buff));
  printf("read result is %s\n", c);

  char *d = (char*) calloc(ID_SIZE, sizeof(char));
  int comp_result = strcmp("", d);
  printf("compare result is %d", comp_result);

}