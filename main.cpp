#include <bitset>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
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

chipset generate_chip_sequence(int lfactor, int rfactor) {
  set10 mother1{"1111111111"};
  set10 mother2{"1111111111"};

  chipset mother_chipset;

  for (int i = 0; i < 1023; i++) {
    bool carry = mother1[9];
    bool daddy = mother1[2];    // das was man xort
    bool child = carry ^ daddy; // das was man hinten wieder reinschiebt
    mother1 >>= 1;
    mother1[0] = child;
    mother_chipset[i] = carry;
  }

  return 0;
}

int main(int argc, char *argv[]) {
  char *fileName = argv[1];
  std::vector<int> signal = read_file_to_vector(fileName);

  for (int e : signal) {
    std::cout << e << std::endl;
  }
  return 0;
}
