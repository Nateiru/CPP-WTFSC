#include <iostream>   // std::cout
#include "disk_vector.h"
#include "disk_buffer_vector.h"

int main() {

  #define MAX 10
  {
    Tiny::DiskVector<int> disk_vec("diskvector.txt");
    for (int i = 0; i < MAX; i++) {
      disk_vec.put(i, i);
    }

    for (int i = 0; i < MAX; i++) {
      std::cout << disk_vec.get(i) << " \n"[i == MAX - 1];
    }
    std::cout << disk_vec.size() << std::endl;
  }

  {
    Tiny::DiskManager disk_manager("diskbuffervector.txt");
    Tiny::BufferPool buffer_pool(8, &disk_manager);
    Tiny::DiskBufferVector<int> disk_buffer_vec(&buffer_pool);
    for (int i = 0; i < MAX; i++) {
      disk_buffer_vec.put(i, i);
    }

    for (int i = 0; i < MAX; i++) {
      std::cout << disk_buffer_vec.get(i) << " \n"[i == MAX - 1];
    }
  }

  return 0;

}