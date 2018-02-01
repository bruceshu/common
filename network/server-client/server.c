

#include <sys/types.h>  
#include <sys/socket.h>  
#include <stdio.h>  
#include <unistd.h>  
#include <string.h>  
#include <arpa/inet.h>  
#include <pthread.h>

#define PORT 9999
#define BUFF_SIZE 1024
#define CLIENT_NUM 3

int creat_socket()
{
    int listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(listen_socket == -1)  
    {  
        printf("create socket failed!\n");
        return -1;
    }

    struct sockaddr_in addr;  
    memset(&addr, 0, sizeof(addr));  
      
    addr.sin_family = AF_INET;    /* Internet地址族 */  
    addr.sin_port = htons(PORT);  /* 端口号 */  
    //addr.sin_addr.s_addr = htonl(INADDR_ANY);   /* IP地址 */  
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");/* IP地址 */  

    int ret = bind(listen_socket, (struct sockaddr *)&addr, sizeof(addr));
    if(ret == -1)  
    {  
        printf("bind socket failed!\n");
        return -1;  
    } 

    ret = listen(listen_socket, CLIENT_NUM);        //监听  
    if(ret == -1)  
    {  
        printf("listen socket failed!\n");
        return -1;  
    }  
    
    return listen_socket;  
}

int wait_client(int listen_socket)  
{  
    struct sockaddr_in cliaddr;  
    int addrlen = sizeof(cliaddr);  
    
    printf("waiting client to connect...\n");  

    //创建一个和客户端交流的套接字 
    int client_socket = accept(listen_socket, (struct sockaddr *)&cliaddr, &addrlen); 
    if(client_socket == -1)  
    {  
        printf("accept socket failed!\n");
        return -1;  
    }  
      
    printf("success to recive a client ：%s\n", inet_ntoa(cliaddr.sin_addr));  
      
    return client_socket;  
}  

//将客户端信息原样输出
void *hanld_client(void * arg)   
{  
    char buf[BUFF_SIZE] = {0};  
    int client_socket = *(int *)(arg);
    
    while(1)  
    {  
        int ret = read(client_socket, buf, BUFF_SIZE-1);  
        if(ret == -1)  
        {  
            printf("read from client socket failed!\n");
            break;  
        }  
        if(ret == 0)  
        {  
            printf("read zero byte!\n");
            break;  
        }  
        
        buf[ret] = '\0';
         
        printf("%s\n", buf);
        
        write(client_socket, buf, ret);  
          
        if(strncmp(buf, "end", 3) == 0)  
        {  
            break;  
        }  
    }  
    
    close(client_socket);  
}  

int main()  
{  
    int listen_socket = creat_socket();  

    while(1)
    {
        int client_socket = wait_client(listen_socket);  

        pthread_t id;  
        pthread_create(&id, NULL, hanld_client, (void *)&client_socket);  //创建一个线程，来处理客户端。  
        pthread_detach(id);   //把线程分离出去。  
    }
      
    close(listen_socket);  
      
    return 0;  
}  
