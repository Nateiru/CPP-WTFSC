
#include <sys/epoll.h>

void register_stdin(int epoll_fd) {
    // 将 stdin 导入 epoll
    epoll_event ev;
    ev.events = EPOLLIN;


}

int main() {
    const int epoll_fd = epoll_create1(0);


}