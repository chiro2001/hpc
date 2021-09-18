#include <arpa/inet.h>
#include <errno.h>
#include <malloc.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define PORT 8002  // 服务器监听端口
// #define SERVER_THREAD_NUM_MAX 32  // 最大线程数
#define SERVER_THREAD_NUM_MAX 512  // 最大线程数
int thread_max = SERVER_THREAD_NUM_MAX;

// 添加一个互斥锁，防止对全局变量的同时读写操作
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int glob = 0;
// 线程池
pthread_t pool[SERVER_THREAD_NUM_MAX];
int pool_index[SERVER_THREAD_NUM_MAX];
pthread_cond_t pool_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t pool_mutex = PTHREAD_MUTEX_INITIALIZER;
int pool_size = 0;
int pool_front = 0;
int pool_back = 0;

int exiting = 0;

void enqueue(int index) {
  pool_index[pool_back] = index;
  pool_back = (pool_back + 1) % SERVER_THREAD_NUM_MAX;
  pool_size++;
}

int dequeue() {
  int index = pool_index[pool_front];
  pool_front = (pool_front + 1) % SERVER_THREAD_NUM_MAX;
  pool_size--;
  return index;
}

int queue_full() { return pool_size == SERVER_THREAD_NUM_MAX; }

int queue_empty() { return pool_size == 0; }

int server_socket;

typedef struct {
  int client_socket;
  int index;
} ThreadData;

void sleep_ms(int ms) {
  struct timeval delay;
  delay.tv_sec = 0;
  delay.tv_usec = ms * 1000;  // 20 ms
  select(0, NULL, NULL, NULL, &delay);
}

void send_handle(ThreadData *thread_data) {
  if (!thread_data) {
    perror("Null on thread_data!\n");
  }
  int client_socket = thread_data->client_socket;
  int index = thread_data->index;
  printf("Start child thread: client = %d, index = %d\n", client_socket, index);
  // 释放 ThreadData 资源
  free(thread_data);
  char status[] = "HTTP/1.0 200 OK\r\n";
  char header[] =
      "Server: DWBServer\r\nContent-Type: text/html;charset=utf-8\r\n\r\n";
  const char body_template[] =
      "<html>"
      "<head>"
      "<title>C语言构建小型Web服务器</title>"
      "</head>"
      "<body>"
      "<h2>欢迎</h2>"
      "<p>Hello，World</p>"
      "<br />"
      "<code>glob = %d</code>"
      "</body>"
      "</html>";
  char body[1024];

  // printf("enter sleeping...\n");
  // sleep(10);  //让进程进入睡眠状态，单位是秒。
  // sleep_ms(1);
  // 处理信息
  // 进入锁区
  pthread_mutex_lock(&mutex);
  // 改变全局变量
  glob += 1;
  sprintf(body, body_template, glob);
  // 自动退出
  if (glob == thread_max) {
    kill(getpid(), SIGINT);
    printf("Killing myself...\n");
  }
  // 退出锁区
  pthread_mutex_unlock(&mutex);
  // printf("finish sleeping...\n");

  write(client_socket, status, sizeof(status) - 1);
  write(client_socket, header, sizeof(header) - 1);
  write(client_socket, body, strlen(body) * sizeof(char));

  close(client_socket);
  // printf("client %d closed.\n", client_socket);
  // 添加当前空缺位置到队列
  pthread_mutex_lock(&pool_mutex);
  while (queue_full()) {
    pthread_cond_wait(&pool_cond, &pool_mutex);
  }
  enqueue(index);
  pool[index] = 0;
  pthread_mutex_unlock(&pool_mutex);
  pthread_cond_signal(&pool_cond);
}

void on_exit_handler() {
  static int first_run = 1;
  if (first_run) {
    first_run = 0;
    if (server_socket) {
      // close(server_socket);
      // 强制释放资源
      if (shutdown(server_socket, 2)) {
        printf("shutdown socket error: %s (errno: %d)\n", strerror(errno),
               errno);
      }
      server_socket = 0;
    }
  } else {
    // 释放资源然后退出
    printf("on_exit_handler()...\n");
    for (int i = 0; i < pool_size; i++) {
      if (pool[i] == 0) continue;
      // 等待所有线程退出
      printf("Waiting pid %lu\n", pool[i]);
      pthread_join(pool[i], NULL);
    }
    system("killall wget");
  }
}

void signal_handler() {
  // 捕获 Ctrl+C
  exiting = 1;
  printf("Captured Ctrl+C SIGINT, exiting...\n");
  on_exit_handler();
  // exit(0);
}

int main(int argc, char **argv) {
  int port = PORT;
  if (argc >= 2) {
    port = atoi(argv[1]);
  }
  if (argc >= 3) {
    thread_max = atoi(argv[2]);
  }
  printf("Listen on port %d, thread_max = %d\n", port, thread_max);
  server_socket = socket(AF_INET, SOCK_STREAM, 0);  //初始化套接字
  struct sockaddr_in server_addr;

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;  // 服务地址和端口配置
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(port);

  bind(server_socket, (struct sockaddr *)&server_addr,
       sizeof(server_addr));  //将本地地址绑定到所创建的套接字上

  listen(
      server_socket,
      SERVER_THREAD_NUM_MAX);  //开始监听是否有客户端连接，第二个参数是最大监听数

  char buf[1024];
  int client_socket;

  // 注册监听 SIGINT
  signal(SIGINT, signal_handler);
  // 注册退出时函数
  atexit(on_exit_handler);

  int ret;
  pthread_t th;

  // 自己启动测试脚本
  // sprintf(buf, "sh run.sh %d &", SERVER_THREAD_NUM_MAX * 3);
  sprintf(buf, "sh run.sh %d %d &", port, thread_max);
  printf("Starting run.sh: %s\n", buf);
  int r = system(buf);
  printf("r = %d\n", r);

  // 装满可用 index
  for (int i = 0; i < SERVER_THREAD_NUM_MAX; i++) enqueue(i);

  while (1) {
    if (exiting || !server_socket) break;
    printf("====== waiting for client's request =====\n");
    if ((client_socket = accept(server_socket, (struct sockaddr *)NULL,
                                NULL)) == -1) {  //等待客户端（浏览器）连接
      if (exiting) {
        printf("exiting accept..\n");
        return 0;
      } else {
        printf("accept socket error: %s (errno: %d)\n", strerror(errno), errno);
      }
      continue;
    }
    printf("====== waiting for read request data =====\n");
    read(client_socket, buf, 1024);  //读取客户端内容，这里是HTTP的请求数据
    // printf("%s",buf);     // 打印读取的内容

    // Chiro: 在这里另开一个线程，用于处理用户请求
    printf("client_socket = %d\n", client_socket);
    // 取出线程池可用列表头部
    while (queue_empty()) {
      // 等待队列数据
      pthread_cond_wait(&pool_cond, &pool_mutex);
    }
    int index = dequeue();
    ThreadData *thread_data = NULL;
    thread_data = malloc(sizeof(ThreadData));
    thread_data->client_socket = client_socket;
    thread_data->index = index;
    ret =
        pthread_create(&th, NULL, (void *(*)(void *))send_handle, thread_data);
    if (ret != 0) {
      perror("Cannot start new thread!!\n");
      // return 1;
      break;
    }
  }
  return 0;
}
