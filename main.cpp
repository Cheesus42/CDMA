#include <array>
#include <bit>
#include <bitset>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

template <std::size_t N>
std::bitset<N> rotr(const std::bitset<N> &b, std::size_t k) {
  k %= N;
  return (b >> k) | (b << (N - k));
}

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

    set10 mother2{"1111111111"};
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
    chipset final = m1_chipset ^ m2_chipset;
    full_chipset[j] = final;
  }

  return full_chipset;
}

void extract_set(std::vector<int> message, chipset chip, int satellite) {

  for (int offset = 0; offset < 1023; ++offset) {
    int result = 0;
    chipset rot_set = rotr(chip, offset);
    for (int i = 0; i < 1023; i++) {
      result += rot_set[i] == 1 ? message[i] : -message[i];
    }
    if (std::abs(result / 1023) == 1) {
      int bit = result > 0 ? 1 : 0;
      std::cout << "Satellite " << satellite << " has sent bit " << bit
                << "  (delta = " << offset << ")" << std::endl;
      return;
    }
  }
}

int main(int argc, char *argv[]) {
  char *fileName = argv[1];
  std::vector<int> signal = read_file_to_vector(fileName);

  const auto start{std::chrono::steady_clock::now()};
  std::array<chipset, 24> satellite_sequences = generate_chip_sequence();
  const auto middle{std::chrono::steady_clock::now()};
  for (int i = 0; i < satellite_sequences.size();
       i++) { // prints generated chip_sequences
    // std::cout << "id " << i+1 << ": " << satellite_sequences[i] << '\n';
    extract_set(signal, satellite_sequences[i], i + 1);
  }
  const auto finish{std::chrono::steady_clock::now()};

  const std::chrono::duration<double> total_seconds{finish - start};
  const std::chrono::duration<double> generator_seconds{middle - start};
  const std::chrono::duration<double> extract_seconds{finish - middle};

  std::cout << "total elapsed Time: " << total_seconds.count() << '\n';
  std::cout << "Gold code Generator Time: " << generator_seconds.count()
            << '\n';
  std::cout << "code extraction Time: " << extract_seconds.count() << '\n';

  return 0;
}
