#include <stdio.h>
#include <string.h>

typedef unsigned char BYTE;

int main(int argc, char *argv[])
{
  int x, numRead; // initialize loop control variables
  BYTE buffer[16]; // buffer to read 16 bytes at a time
  FILE *fp = fopen(argv[1], "rb"); // open file for reading
  fseek(fp, 0, SEEK_END); // seek to end of file
  int fileSize = ftell(fp); // get current file pointer
  fseek(fp, 0, SEEK_SET); // seek back to beginning of file
  printf("FileSize %d \n", fileSize); // print file size
  for (x = 0; x < fileSize; x += 16) // loop through file
  {
    numRead = fread(buffer, 1, 16, fp); // read 16 bytes
    if (numRead < 1) // if error or EOF, break
    {
      printf("error\n"); 
      break;
    }
    if (numRead < 16) // if less than 16 bytes read, pad with zeros
    {
      memset(&buffer[numRead], 0, 16 - numRead); 
    }
    printf("%.*s\n", numRead, buffer); // print 16 bytes
  }
  fclose(fp); // close file
  return (0);
}