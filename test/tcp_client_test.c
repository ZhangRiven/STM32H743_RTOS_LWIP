#include "lwip/opt.h"
#include "lwip/api.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "string.h"
#include "cmsis_os.h"

#include <stdio.h>

#define IP_ADDR        "192.168.10.102"
#define PORT           8080
static void client_socket_thread(void *arg)
{
  int sock;
  struct sockaddr_in address;
  uint8_t send_buf[]= "This is a TCP Client test...\r\n";
  /* create a TCP socket */
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
  {
      printf("Socket error\n");
    return;
  }
  
  address.sin_family = AF_INET;
  address.sin_port = htons(PORT);
  address.sin_addr.s_addr = inet_addr(IP_ADDR);
  memset(&(address.sin_zero), 0, sizeof(address.sin_zero));

  if (connect(sock,(struct sockaddr *)&address,sizeof(struct sockaddr)) == -1)
  {
      printf("Connect failed!\n");
      closesocket(sock);
   }
  else
  {
      printf("Connect to iperf server successful!\n");
  }
  while (1) 
  {
      if (write(sock,send_buf,sizeof(send_buf)) < 0)
          break;
      vTaskDelay(1000);
  }
  closesocket(sock);
}

void client_socket_init()
{
  sys_thread_new("HTTP", client_socket_thread, NULL, DEFAULT_THREAD_STACKSIZE * 4, osPriorityAboveNormal);
}
