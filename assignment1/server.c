// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pwd.h>
#include <sys/types.h>
#define PORT 8080

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";
    struct passwd* pwd_ptr;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    if (bind(server_fd, (struct sockaddr *)&address,
	sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    int status = 0;
    if(pid == -1)
    {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }   

    if(pid == 0)
    {
        pwd_ptr = getpwnam("nobody");
        printf("Child Process - UID of nobody=%ld\n",(long) pwd_ptr->pw_uid);
        if(setuid(pwd_ptr->pw_uid) != 0){
            perror("failed to set id of child process");
            exit(EXIT_FAILURE);
        } else {
            printf("The user ID is %d\n", getuid());
            valread = read(new_socket, buffer, 1024);
            printf("Read %d bytes: %s\n", valread, buffer);
            send(new_socket, hello, strlen(hello), 0);
            printf("Hello message sent\n");
            printf("*** Child process is done ***\n");
        }
    } else {
        while ((pid = wait(&status)) > 0);
        printf("*** Parent process is done ***\n");
    }

    // valread = read(new_socket, buffer, 1024);
    // printf("Read %d bytes: %s\n", valread, buffer);
    // send(new_socket, hello, strlen(hello), 0);
    // printf("Hello message sent\n");

    return 0;
}
