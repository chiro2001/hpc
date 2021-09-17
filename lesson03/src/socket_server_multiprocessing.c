#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/types.h>


#define PORT 8000  // 服务器监听端口

void send_handle(int client_socket) {
  char status[] = "HTTP/1.0 200 OK\r\n";
  char header[] =
      "Server: DWBServer\r\nContent-Type: text/html;charset=utf-8\r\n\r\n";
  char body[] =
      "<html><head><title>C语言构建小型Web服务器</title></head><body><h2>欢迎</"
      "h2><p>Hello，World</p></body></html>";

  printf("enter sleeping...\n");
  sleep(10);  //让进程进入睡眠状态，单位是秒。
  printf("finish sleeping...\n");

  write(client_socket, status, sizeof(status));
  write(client_socket, header, sizeof(header));
  write(client_socket, body, sizeof(body));

  close(client_socket);
}

int main() {
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);  //初始化套接字
  struct sockaddr_in server_addr;

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;  // 服务地址和端口配置
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(PORT);

  bind(server_socket, (struct sockaddr *)&server_addr,
       sizeof(server_addr));  //将本地地址绑定到所创建的套接字上

  listen(server_socket, 5);  //开始监听是否有客户端连接，第二个参数是最大监听数

  char buf[1024];
  int client_socket;

  while (1) {
    printf("======waiting for client's request=====\n");
    if ((client_socket = accept(server_socket, (struct sockaddr *)NULL,
                                NULL)) == -1) {  //等待客户端（浏览器）连接
      printf("accept socket error :%s(errno:%d)\n", strerror(errno), errno);
      continue;
    }
    printf("======waiting for read request data=====\n");
    read(client_socket, buf, 1024);  //读取客户端内容，这里是HTTP的请求数据
    // printf("%s",buf);     // 打印读取的内容

    // Chiro: 改成多进程处理
    // 仅仅让子进程处理，主进程回到 lisent.
    pid_t pid = fork();
    if (pid == 0) {
      // 子进程
      send_handle(client_socket);
      return 0;
    } else {
      // 父进程
      // do nothing.
    }
  }
  close(server_socket);

  return 0;
}
