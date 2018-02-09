// Derived from code taken from Stack Overflow
//
#include <stdio.h>
#include <ctype.h>

#define PATH_LEN 256
#define MD5_LEN 32

// Computer the MD5 sum of a file
//
// file_name: The null terminated name of the file to be summed
// md5_sum: A buffer large enough to receive the text MD5SUM
//
// Returns: zero on failure, non-zero on success.
//
int CalcFileMD5(char *file_name, char *md5_sum)
{
    char cmd[2*PATH_LEN];
    sprintf(cmd, "md5sum %s 2>/dev/null", file_name);

    FILE *p = popen(cmd, "r");
    if (p == NULL) return 0;

    int ii, ch;
    for (ii = 0; ii < MD5_LEN && isxdigit(ch = fgetc(p)); ii++) {
        *md5_sum++ = ch;
    }

    *md5_sum = '\0';
    pclose(p);
    return ii == MD5_LEN;
}
