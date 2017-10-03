#include "readline.h"

// *************************************
// See the header file for documentation
char* readline(char* buff, ssize_t size, int fd) {

  //BUG something bad occurs after reaching 32 chars I believe

  /* check if a new block needs to be read */
  /* check if at buffer max (size) */

  static __thread int index = 0;
  static __thread char block[BLOCK_SIZE+1];
  static __thread int firstRead = -1;
  firstRead++;
  int block_num = 1;

  if(firstRead < 1) {
    block_num = read_block(fd, block);
    /* file read error */
    if(block_num < 0) return 0;
  }

  printf("read # %d\n", firstRead+1);

  int i = 0;
  /* while !EOF */
  while (block_num != 0) {
    while (i < size) {

      /* block size exceeded, a new block is needed */
      if(index > block_num){
        printf("%s\n", "Block Size Exceeded" );
        break;
      }

      /* a new line is encountered, return buffer */
      printf("looking at block: %c\n", block[index]);
      if(block[index] == '\n') {
        printf("%s\n", "new line found");
        buff[i++] = block[index++];
        buff[i] = 0;
        return buff;
      }

      /* reached max size */
      if(i > size-1) {
        buff[i] = block[index];
        buff[i++] = 0;
        return buff;
        printf("%s\n", "max size reached");
      }

      /* assign char val from block to buff */
      buff[i++] = block[index++];
    }
    
    block_num = read_block(fd, buff);
    printf("block_num: %d\n", block_num );
  }
  printf("%s\n", "zero return");
  return 0;
}
