#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

std::vector<int> readFileToVector(char *fileName) {
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

int main(int argc, char *argv[]) {
  char *fileName = argv[1];
  std::vector<int> signal = readFileToVector(fileName);

  for (int e : signal) {
    std::cout << e << std::endl;
  }
  return 0;
}
