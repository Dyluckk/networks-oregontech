#include "readline.h"
#include <string.h>

// *************************************
// See the header file for documentation
char* readline(char* buff, ssize_t size, int fd) {

    static __thread char block[BLOCK_SIZE+1];
    static __thread int current_block_size = 0;
    static __thread int block_index = 0;
    static __thread int new_block_needed = 1;
    static __thread int EOF_OR_ERROR = 0;

    /* set up the buffer */
    int buffer_index = 0;
    memset (buff,'\0',size);

    while (!EOF_OR_ERROR) {

      if(new_block_needed) {

        current_block_size = read_block(fd, block);
        printf("%s\n%s\n%s\n\n", "GETTING A NEW BLOCK:\n============================", block, "============================");
        /* if EOF or file read error stop reading */
        if(current_block_size <= 0) {

          printf("%s\n", "EOF/ERROR REACHED" );

          /* set EOF_OR_ERROR flag */
          EOF_OR_ERROR = 1;
          /* return buffer to user if in the middle of a read */
          if(buff[0] != 0) {
            printf("BUFFER EXISTS: %s\n", buff);
            buff[buffer_index] = 0;
            return buff;
          }
          /* if no current buffer break */
          break;
        }

        /* reset index of block, and turn off new_block_needed flag */
        block_index = 0;
        new_block_needed = 0;
      }

      /* reached end of block, need to read a new one */
      if(block_index > current_block_size - 1) {
        printf("%s\n", "END OF BLOCK" );
        new_block_needed = 1;
      }
      /* reached new line */
      else if(block[block_index] == '\n') {
        printf("%s\n", "pushed: new line" );
        /* push null and the new line */
        buff[buffer_index++] = block[block_index++];
        buff[buffer_index] = 0;
        return buff;
      }
      /* exceededed consumer buffer, return  */
      else if(buffer_index > size - 1) {
        printf("%s\n", "BUFFER EXCEEDED");
        /* push null and return */
        buff[buffer_index] = 0;
        return buff;
      }
      /* push char into buffer */
      else {
        printf("pushed: %c\n", block[block_index]);
        buff[buffer_index++] = block[block_index++];
      }
    }
    /* EOF or Error occured in previous call */
    return 0;
}
