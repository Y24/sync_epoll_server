#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define IPADDRESS "127.0.0.1"
#define PORT 8787
#define MAXSIZE 1024
#define LISTENQ 5
#define FDSIZE 1000
#define EPOLLEVENTS 100

static int socket_bind(const char *ip, int port);

static void do_epoll(int listenfd);

static void handle_events(int epollfd, struct epoll_event *events, int num,
                          int listenfd, char *buf);

static void handle_accpet(int epollfd, int listenfd);

static void do_read(int epollfd, int fd, char *buf);

static void do_write(int epollfd, int fd, char *buf);

static void add_event(int epollfd, int fd, int state);

static void modify_event(int epollfd, int fd, int state);

static void delete_event(int epollfd, int fd, int state);

int main(int argc, char *argv[]) {
  int listenfd = socket_bind(IPADDRESS, PORT);
  listen(listenfd, LISTENQ);
  do_epoll(listenfd);
  return 0;
}

static int socket_bind(const char *ip, int port) {
  int listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd == -1) {
    perror("socket error:");
    exit(EXIT_FAILURE);
  }
  struct sockaddr_in servaddr;
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  inet_pton(AF_INET, ip, &servaddr.sin_addr);
  servaddr.sin_port = htons(port);
  if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
    perror("bind error: ");
    exit(EXIT_FAILURE);
  }
  return listenfd;
}

static void do_epoll(int listenfd) {
  struct epoll_event events[EPOLLEVENTS];
  char buf[MAXSIZE];
  memset(buf, 0, MAXSIZE);
  int epollfd = epoll_create(FDSIZE);
  add_event(epollfd, listenfd, EPOLLIN);
  while (true) {
    int ret = epoll_wait(epollfd, events, EPOLLEVENTS, -1);
    handle_events(epollfd, events, ret, listenfd, buf);
  }
  close(epollfd);
}

static void handle_events(int epollfd, struct epoll_event *events, int num,
                          int listenfd, char *buf) {
  for (int i = 0; i < num; i++) {
    int fd = events[i].data.fd;
    if ((fd == listenfd) && (events[i].events & EPOLLIN))
      handle_accpet(epollfd, listenfd);
    else if (events[i].events & EPOLLIN)
      do_read(epollfd, fd, buf);
    else if (events[i].events & EPOLLOUT)
      do_write(epollfd, fd, buf);
  }
}
static void handle_accpet(int epollfd, int listenfd) {
  struct sockaddr_in cliaddr;
  socklen_t cliaddrlen = sizeof(cliaddr);
  int clifd = accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddrlen);
  if (clifd == -1)
    perror("accpet error:");
  else {
    printf("accept a new client: %s:%d\n", inet_ntoa(cliaddr.sin_addr),
           cliaddr.sin_port);
    add_event(epollfd, clifd, EPOLLIN);
  }
}

static void do_read(int epollfd, int fd, char *buf) {
  if (int nread = read(fd, buf, MAXSIZE); nread == -1) {
    perror("read error:");
    close(fd);
    delete_event(epollfd, fd, EPOLLIN);
  } else if (nread == 0) {
    fprintf(stderr, "client close.\n");
    close(fd);
    delete_event(epollfd, fd, EPOLLIN);
  } else {
    printf("read message is : %s", buf);
    modify_event(epollfd, fd, EPOLLOUT);
  }
}

static void do_write(int epollfd, int fd, char *buf) {
  if (int nwrite = write(fd, buf, strlen(buf)); nwrite == -1) {
    perror("write error:");
    close(fd);
    delete_event(epollfd, fd, EPOLLOUT);
  } else
    modify_event(epollfd, fd, EPOLLIN);
  memset(buf, 0, MAXSIZE);
}

static void add_event(int epollfd, int fd, int state) {
  struct epoll_event ev;
  ev.events = state;
  ev.data.fd = fd;
  epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}

static void delete_event(int epollfd, int fd, int state) {
  struct epoll_event ev;
  ev.events = state;
  ev.data.fd = fd;
  epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
}

static void modify_event(int epollfd, int fd, int state) {
  struct epoll_event ev;
  ev.events = state;
  ev.data.fd = fd;
  epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
}