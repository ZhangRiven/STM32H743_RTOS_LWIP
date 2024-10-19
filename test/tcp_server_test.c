#include "lwip/opt.h"
#include "lwip/api.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "string.h"
#include "cmsis_os.h"

#include <stdio.h>

#define PORT          (8088)
#define RECV_DATA     (1024)
 
static void server_socket_thread(void *arg)
{
  int sock,connected;
  struct sockaddr_in address,client_addr;
  char *recv_data;
  socklen_t sin_size;
  int recv_data_len;
   
  recv_data = (char *)pvPortMalloc(RECV_DATA);
  if (recv_data == NULL)
  {
      printf("No memory\n");
      return;
  }
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
  {
      printf("Socket error\n");
      if (recv_data) free(recv_data);
      return;
  }
  
  /* bind to port 80 at any interface */
  address.sin_family = AF_INET;
  address.sin_port = htons(PORT);
  address.sin_addr.s_addr = INADDR_ANY;
  memset(&(address.sin_zero), 0, sizeof(address.sin_zero));
  if (bind(sock, (struct sockaddr *)&address, sizeof (address)) < 0)
  {
      printf("unable to bind\r\n");
      if (recv_data) free(recv_data);
      return;
  }
  
  /* listen for incoming connections (TCP listen backlog = 5) */
  if(listen(sock, 5) == -1)
  {
      printf("listen error\r\n");
      if (recv_data) free(recv_data);
      return;
  }

  while (1) 
  {
      sin_size = sizeof(struct sockaddr_in);
      connected = accept(sock, (struct sockaddr *)&client_addr, &sin_size);
      printf("new client connected from (%s, %d)\n",inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
      {
          int flag = 1;
          setsockopt(connected,IPPROTO_TCP,     /* set option at TCP level */
                     TCP_NODELAY,     /* name of option */
                     (void *) &flag, /* the cast is historical cruft */
                     sizeof(int));    /* length of option value */
      }
      while (1)
      {
          recv_data_len = recv(connected, recv_data, RECV_DATA, 0);
          if(recv_data_len <= 0)
              break;
          printf("recv %d len data\n",recv_data_len);
          write(connected,recv_data,recv_data_len);
      }
      if(connected >= 0)
          closesocket(connected);
       connected = -1;
  }
  if(recv_data) free(recv_data);
}

void server_socket_init()
{
  sys_thread_new("HTTP", server_socket_thread, NULL, DEFAULT_THREAD_STACKSIZE * 4, osPriorityAboveNormal);
}
