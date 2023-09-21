#include <iostream>   // std::cout
#include "disk_vector.h"

int main() {

  Tiny::DiskVector<int> disk_vec("diskvector.txt");
  for (int i = 0; i < 4096; i++) {
    disk_vec.put(i, i);
  }

  for (int i = 0; i < 4096; i++) {
    std::cout << disk_vec.get(i) << std::endl;
  }
  std::cout << disk_vec.size() << std::endl;
  return 0;

}