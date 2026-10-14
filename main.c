#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#define N 1023;

char *read_file_to_vector(char *fileName) {
  FILE *fptr = fopen(fileName, "r");
  if (!fptr) {
    perror("fopen");
    return NULL;
  }
  fseek(fptr, 0, SEEK_END);
  long size = ftell(fptr);
  rewind(fptr);
  char *buffer = malloc(size + 1);
  if (!buffer) {
    fclose(fptr);
    return NULL;
  }
  size_t n = fread(buffer, 1, size, fptr);
  buffer[n] = '\0';
  fclose(fptr);
  return buffer;
}

size_t parse_to_ints(char *buffer, int8_t *out) {
  size_t count = 0;
  const char *p = buffer;
  char *end;
  while (count < 1023) {
    long val = strtol(p, &end, 10);
    if (end == p)
      break;
    if (val > INT8_MAX || val < INT8_MIN) {
      printf("value %ld out of range of int8_t", val);
      break;
    }
    out[count++] = (int8_t)val;
    p = end;
  }
  return count;
}

typedef bool chipset[1023];
static inline void chipset_xor(chipset dst, const chipset set1,
                               const chipset set2) {
  for (int i = 0; i < 1023; i++) {
    dst[i] = set1[i] != set2[i];
  }
}

typedef uint16_t bitset10;
static inline bitset10 new_bs() { return 0x03FF; }
static inline bitset10 bs_shl(bitset10 b, unsigned n) {
  return (b << n) & 0x03FF;
}
static inline bool bs_idx(bitset10 b, unsigned n) {
  return ((1u << n) & b) != 0;
}
static inline void bs_write_idx(bitset10 *b, unsigned idx, bool value) {
  if (value) {
    *b |= (bitset10)(1u << idx);
  } else {
    *b &= (bitset10) ~(1u << idx);
  }
}

// void print_chip(chipset set) {
//   for (int i = 0; i < 1023; i++) {
//     std::cout << set[i];
//   }
// }
//
void generate_chip_sequence(chipset out[24]) {
  int8_t register_values[24][2] = {
      {2, 6},  {3, 7}, {4, 8}, {5, 9}, {1, 9}, {2, 10}, {1, 8}, {2, 9},
      {3, 10}, {2, 3}, {3, 4}, {5, 6}, {6, 7}, {7, 8},  {8, 9}, {9, 10},
      {1, 4},  {2, 5}, {3, 6}, {4, 7}, {5, 8}, {6, 9},  {1, 3}, {4, 6}};

  bitset10 mother1 = new_bs();

  chipset m1_chipset;

  // mother 1 factor
  for (int i = 0; i < 1023; i++) {
    bool carry = bs_idx(mother1, 9);
    bool daddy = bs_idx(mother1, 2); // das was man xort
    bool child = carry ^ daddy;      // das was man hinten wieder reinschiebt
    mother1 = bs_shl(mother1, 1);
    bs_write_idx(&mother1, 0, child);
    m1_chipset[i] = carry;
  }
  // all the mother 2 factors
  for (int j = 0; j <= 23; j++) {
    chipset m2_chipset;

    bitset10 mother2 = new_bs();
    for (int i = 0; i < 1023; i++) {
      int lfactor = register_values[j][0];
      int rfactor = register_values[j][1];
      bool register_sum =
          bs_idx(mother2, lfactor - 1) ^ bs_idx(mother2, rfactor - 1);

      bool carry = bs_idx(mother2, 9);
      bool child = bs_idx(mother2, 1) ^ bs_idx(mother2, 2) ^
                   bs_idx(mother2, 5) ^ bs_idx(mother2, 7) ^
                   bs_idx(mother2, 8) ^ carry;
      mother2 = bs_shl(mother2, 1);
      bs_write_idx(&mother2, 0, child);
      m2_chipset[i] = (uint8_t)register_sum;
    }
    chipset final[1023];
    chipset_xor(out[j], m1_chipset, m2_chipset);
  }
}
//
void extract_set(const int8_t *message, chipset chip, int satellite) {

  bool doubled_chip[2046];
  memcpy(doubled_chip, chip, 1023 * sizeof(bool));
  memcpy(doubled_chip + 1023, chip, 1023 * sizeof(bool));

  for (int offset = 0; offset < 1023; ++offset) {
    int result = 0;
    for (int i = 0; i < 1023; i++) {
      bool chip_bit = doubled_chip[(i + offset)];
      result += chip_bit ? message[i] : -message[i];
    }
    if (abs(result / 1023) == 1) {
      int bit = result > 0 ? 1 : 0;
      printf("Satellite %d has sent bit %d  (delta = %d offset) \n", satellite,
             bit, offset);
      return;
    }
  }
}
void extract_set_no_memcpy(int8_t *message, chipset chip, int satellite) {

  bool doubled_chip[2046];
  for (int i = 0; i < 1023; ++i) {
    doubled_chip[i] = chip[i];
    doubled_chip[i + 1023] = chip[i];
  }

  for (int offset = 0; offset < 1023; ++offset) {
    int result = 0;
    for (int i = 0; i < 1023; i++) {
      bool chip_bit = doubled_chip[(i + offset)];
      result += chip_bit ? message[i] : -message[i];
    }
    if (abs(result / 1023) == 1) {
      int bit = result > 0 ? 1 : 0;
      printf("Satellite %d has sent bit %d  (delta = %d offset) \n", satellite,
             bit, offset);
      return;
    }
  }
}
void extract_set_with_mod(int8_t *message, chipset chip, int satellite) {

  for (int offset = 0; offset < 1023; ++offset) {
    int result = 0;
    for (int i = 0; i < 1023; i++) {
      bool chip_bit = chip[(i + offset) % 1023];
      result += chip_bit ? message[i] : -message[i];
    }
    if (abs(result / 1023) == 1) {
      int bit = result > 0 ? 1 : 0;
      printf("Satellite %d has sent bit %d  (delta = %d offset) \n", satellite,
             bit, offset);
      return;
    }
  }
}
void extract_set_with_cond(int8_t *message, chipset chip, int satellite) {

  for (int offset = 0; offset < 1023; ++offset) {
    int result = 0;
    for (int i = 0; i < 1023; i++) {
      if (offset >= 1023)
        offset -= 1023;
      bool chip_bit = chip[(i + offset)];
      result += chip_bit ? message[i] : -message[i];
    }
    if (abs(result / 1023) == 1) {
      int bit = result > 0 ? 1 : 0;
      printf("Satellite %d has sent bit %d  (delta = %d offset) \n", satellite,
             bit, offset);
      return;
    }
  }
}

int main(int argc, char *argv[]) {
  char *fileName = argv[1];
  char *buffer = read_file_to_vector(fileName);
  int8_t signal[1023];
  size_t amount = parse_to_ints(buffer, signal);
  if (amount != 1023) {
    printf("wrong amount %zd", amount);
    return 1;
  }
  chipset sets[24];
  generate_chip_sequence(sets);
  clock_t for (int i = 0; i <= 23; i++) { extract_set(signal, sets[i], i + 1); }
  for (int i = 0; i <= 23; i++) {
    extract_set_no_memcpy(signal, sets[i], i + 1);
  }
  for (int i = 0; i <= 23; i++) {
    extract_set_with_cond(signal, sets[i], i + 1);
  }
  for (int i = 0; i <= 23; i++) {
    extract_set_with_mod(signal, sets[i], i + 1);
  }
  return 0;
}
