#include "disk_manager.h"
#include <iostream>

static char buf[4096];
int main() {

  Tiny::DiskManager dm("zhuziyi.txt");
  char data[]= "zhuziyi!!!\n";
  dm.WriteBlock(0, data);
  dm.ReadBlock(0, buf);
  std::cout << buf << std::endl;
}