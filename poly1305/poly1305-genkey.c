#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
  unsigned char r[16];
  int i;
  FILE *file;

  // Seed the random number generator for demonstration purposes.
  // For cryptographic purposes, you would use a cryptographically secure RNG.
  srand(time(NULL));

  // Generate each byte of the secret key according to the specified rules.
  for (i = 0; i < 16; i++)
  {
    if (i == 3 || i == 7 || i == 11 || i == 15)
    {
      // Clear top four bits (values between 0 and 15)
      r[i] = rand() & 0x0F;
    }
    else if (i == 4 || i == 8 || i == 12)
    {
      // Clear bottom two bits (values are multiples of 4)
      r[i] = rand() & 0xFC;
    }
    else
    {
      // No restrictions
      r[i] = rand() & 0xFF;
    }
  }

  // Open file for writing
  file = fopen("r", "wb");
  if (file == NULL)
  {
    perror("Failed to open file");
    return 1;
  }

  // Write the generated key to the file
  fwrite(r, 1, sizeof(r), file);
  fclose(file);

  printf("Generated secret key has been written to file 'r'.\n");

  return 0;
}
