// https://www.bilibili.com/video/BV1bh4y197WZ/?spm_id_from=333.1007.top_right_bar_window_custom_collection.content.click

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/timerfd.h>
#include <unistd.h>

int timer_fd = -1;

void handle_stdin() {
  constexpr size_t len = 8192;
  char buf[len];

  const ssize_t n = read(STDIN_FILENO, buf, len);
  if (n > 0) {
    write(STDOUT_FILENO, buf, n);
  } 
}

void handle_timer() {
  // 定时逻辑
  printf("2s Timer expired\n");

  // 重制定时器
  uint64_t buf[1];
  auto res = read(timer_fd, buf, 1);
  if (res < 0) {
    perror("read");
  }
}

void register_stdin(int epoll_fd) {
	// 将 stdin 导入 epoll
	epoll_event ev;
	ev.events = EPOLLIN;
  ev.data.ptr = reinterpret_cast<void *>(handle_stdin);
  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &ev);
}

timespec second_to_timespec(int second) {
  return timespec {
    .tv_sec = second,
    .tv_nsec = 0,
  };
}

void register_timer(int epoll_fd, int second) {
  // 构造 timerfd
  timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);

  itimerspec duration {
    .it_interval = second_to_timespec(second),
    .it_value = second_to_timespec(second),
  };

  timerfd_settime(timer_fd, 0, &duration, nullptr);
  
  // 将 timerfd 倒入 epoll
  epoll_event ev;
  ev.events = EPOLLIN;
  ev.data.ptr = reinterpret_cast<void *>(handle_timer);
  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, timer_fd, &ev);
}

int main() {
	const int epoll_fd = epoll_create1(0);

  // 1. 批量提交
  register_stdin(epoll_fd);      // 监听 stdin
  register_timer(epoll_fd, 2);// 监听 timer

  for (;;) {
    epoll_event evs[4];

    // 2. 主动询问 -1表示阻塞 一直等待直到有IO事件触发
    const int n = epoll_wait(epoll_fd, evs, 4, -1);

    if (n < 0) {
      // 错误处理
      perror("epoll_wait");
      continue;
    }

    // 3. 处理就绪的请求
    for (int i = 0; i < n; ++i) {
      auto *handler = reinterpret_cast<void (*)()>(evs[i].data.ptr);
      handler();
    }
  }
  return 0;
}