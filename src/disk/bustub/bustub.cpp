#include <iostream>

#include "disk_manager.h"
#include "disk_manager_memory.h"

static char buf[4096];
int main() {

  Tiny::DiskManager dm("bustub.txt");
  char data[]= "zhuziyi!!!";
  dm.WriteBlock(0, data);
  dm.ReadBlock(0, buf);
  std::cout << buf << std::endl;

  Tiny::DiskManagerUnlimitedMemory dmum;
  dmum.WriteBlock(0, data);
  dmum.ReadBlock(0, buf);
  std::cout << buf << std::endl;
}