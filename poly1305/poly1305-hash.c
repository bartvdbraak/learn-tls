#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <gmp.h>

#define BLOCK_SIZE 16

// Initialize and set a mpz_t from a byte array (little-endian)
void mpz_import_le(mpz_t rop, const uint8_t *bytes, size_t len)
{
  uint8_t *le_bytes = malloc(len * sizeof(uint8_t));
  for (size_t i = 0; i < len; i++)
  {
    le_bytes[i] = bytes[len - 1 - i];
  }
  mpz_import(rop, len, -1, sizeof(le_bytes[0]), 0, 0, le_bytes);
  free(le_bytes);
}

// Finalize and export mpz_t to a byte array (little-endian)
void mpz_export_le(uint8_t *bytes, size_t len, mpz_t op)
{
  uint8_t *tmp = malloc(len);
  memset(bytes, 0, len);
  mpz_export(tmp, NULL, -1, 1, 0, 0, op);
  for (size_t i = 0; i < len; i++)
  {
    bytes[i] = tmp[len - 1 - i];
  }
  free(tmp);
}

// Evaluate the Poly1305 hash
void poly1305_hash(uint8_t *hash, const uint8_t *key, const uint8_t *message, size_t message_len)
{
  mpz_t r, s, acc, c, p, temp, r_pow;
  mpz_inits(r, s, acc, c, p, temp, r_pow, NULL);
  mpz_set_ui(acc, 0);
  mpz_set_str(p, "340282366920938463463374607431768211457", 10); // 2^130 - 5

  // Initialize r and s from key (example, split 16-byte key appropriately)
  mpz_import_le(r, key, 16);
  mpz_import_le(s, key + 16, 16);

  // Process each block
  size_t num_blocks = (message_len + 15) / BLOCK_SIZE;
  mpz_set_ui(r_pow, 1);
  for (size_t i = 0; i < num_blocks; i++)
  {
    size_t block_len = (i < num_blocks - 1) ? BLOCK_SIZE : message_len % BLOCK_SIZE;
    mpz_import_le(c, message + i * BLOCK_SIZE, block_len);
    mpz_mul(temp, c, r_pow);  // temp = c * r^i
    mpz_add(acc, acc, temp);  // acc += temp
    mpz_mod(acc, acc, p);     // acc %= p
    mpz_mul(r_pow, r_pow, r); // r_pow *= r
    mpz_mod(r_pow, r_pow, p);
  }

  // Reduce mod 2^128 and add 's'
  mpz_t mod;
  mpz_init_set_str(mod, "340282366920938463463374607431768211456", 10); // 2^128
  mpz_mod(acc, acc, mod);
  mpz_add(acc, acc, s);
  mpz_mod(acc, acc, mod);

  // Convert result back to byte array
  mpz_export_le(hash, 16, acc);

  // Clear GMP variables
  mpz_clears(r, s, acc, c, p, temp, r_pow, mod, NULL);
}

int main(int argc, char **argv)
{
  if (argc < 3)
  {
    fprintf(stderr, "Usage: %s <keyfile> <messagefile>\n", argv[0]);
    return 1;
  }

  // Example use case: read key and message from files
  uint8_t key[32], hash[16];
  FILE *keyfile = fopen(argv[1], "rb");
  fread(key, 1, 32, keyfile);
  fclose(keyfile);

  FILE *msgfile = fopen(argv[2], "rb");
  fseek(msgfile, 0, SEEK_END);
  size_t msg_len = ftell(msgfile);
  fseek(msgfile, 0, SEEK_SET);
  uint8_t *message = malloc(msg_len);
  fread(message, 1, msg_len, msgfile);
  fclose(msgfile);

  poly1305_hash(hash, key, message, msg_len);
  printf("Poly1305 Hash: ");
  for (int i = 0; i < 16; i++)
  {
    printf("%02x", hash[i]);
  }
  printf("\n");

  free(message);
  return 0;
}
