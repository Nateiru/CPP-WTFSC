#include "env_posix.h"

int main() {
  int fd = open("leveldb.txt", O_TRUNC | O_WRONLY | O_CREAT, 0644);
  Tiny::PosixWritableFile dm(fd);
  dm.Append(Tiny::Slice("leveldb!!!"));
  dm.Sync();
  dm.Close();
}