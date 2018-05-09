#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

char* run_the_world()
{

    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    int bufferSize = 5 * 1024 * 1024; // 5MB
    char buffer[bufferSize];


    portno = atoi("80");
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname("example.com");
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    // send
    char* data = "GET / HTTP/1.1\nHOST: example.com\n\n";
    strcpy(buffer, data);
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0)
         error("ERROR writing to socket");
    bzero(buffer,bufferSize);
    n = read(sockfd,buffer,bufferSize-1);
    if (n < 0)
         error("ERROR reading from socket");
    // printf("%s\n",buffer);

    char *start = strstr(buffer, "<p>");
    start += 3 * sizeof(char);
    char *end=strstr(start, "</p>");

    int diff = (end - start) / sizeof(char);
    // printf("used size: %d\n", diff);

    // huge memory leak!!!
    start[diff] = 0;

    // printf("and the result is \n\n%s", start);

    return start;
}
