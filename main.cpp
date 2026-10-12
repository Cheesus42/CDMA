#include <array>
#include <bitset>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

std::vector<int> read_file_to_vector(char *fileName) {
  std::ifstream FileReader(fileName);
  std::string stringStream;
  getline(FileReader, stringStream);
  std::istringstream iss(stringStream);
  std::vector<int> res;
  int n;
  while (iss >> n) {
    res.push_back(n);
  }
  return res;
}

typedef std::bitset<10> set10;
typedef std::bitset<1023> chipset;
typedef std::pair<int, int> sum_val;

void print_chip(chipset set) {
  for (int i = 0; i < 1023; i++) {
    std::cout << set[i];
  }
}

std::array<chipset, 24> generate_chip_sequence() {
  std::array<sum_val, 24> register_values{
      {{2, 6},  {3, 7}, {4, 8}, {5, 9}, {1, 9}, {2, 10}, {1, 8}, {2, 9},
       {3, 10}, {2, 3}, {3, 4}, {5, 6}, {6, 7}, {7, 8},  {8, 9}, {9, 10},
       {1, 4},  {2, 5}, {3, 6}, {4, 7}, {5, 8}, {6, 9},  {1, 3}, {4, 6}}};

  set10 mother1{"1111111111"};
  set10 mother2{"1111111111"};
  std::array<chipset, 24> full_chipset;

  chipset m1_chipset;

  // mother 1 factor
  for (int i = 0; i < 1023; i++) {
    bool carry = mother1[9];
    bool daddy = mother1[2];    // das was man xort
    bool child = carry ^ daddy; // das was man hinten wieder reinschiebt
    mother1 <<= 1;
    mother1[0] = child;
    m1_chipset[i] = carry;
  }
  // all the mother 2 factors
  for (int j = 0; j <= 23; j++) {
    chipset m2_chipset;
    for (int i = 0; i < 1023; i++) {
      int lfactor = register_values[j].first;
      int rfactor = register_values[j].second;
      bool register_sum = mother2[lfactor - 1] ^ mother2[rfactor - 1];

      bool carry = mother2[9];
      bool child = mother2[1] ^ mother2[2] ^ mother2[5] ^ mother2[7] ^
                   mother2[8] ^ carry;
      mother2 <<= 1;
      mother2[0] = child;
      m2_chipset[i] = register_sum;
    }
    for (int i = 0; i <= 23; i++) {
      chipset final = m1_chipset ^ m2_chipset;
      full_chipset[j] = final;
    }
  }

  return full_chipset;
}

int main(int argc, char *argv[]) {
  char *fileName = argv[1];
  std::vector<int> signal = read_file_to_vector(fileName);

  for (int e : signal) {
    std::cout << e << std::endl;
  }
  std::array<chipset, 24> chipsets = generate_chip_sequence();
  for (chipset e : chipsets) {
    print_chip(e);
    std::cout << '\n';
  }
  return 0;
}
