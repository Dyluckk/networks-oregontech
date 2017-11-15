// Computer the MD5 sum of a file
//
// file_name: The null terminated name of the file to be summed
// md5_sum: A buffer large enough to receive the text MD5SUM
//
// Returns: zero on failure, non-zero on success.
//
int CalcFileMD5(char *file_name, char *md5_sum);
