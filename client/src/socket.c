#include <socket.h>

int create_socket(char *argv[])
{
	int  	sockfd = 0,
		status = 0,
                port_number = 0;
        struct  sockaddr_in server_addr;
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	ERROR_CHECK(sockfd, "Socket failed.");

        memset(&server_addr, 0, sizeof(server_addr));
        port_number = atoi(argv[1]);
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port_number);
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        status = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (status == -1)
        {
                return FAILURE;
        }
        return sockfd;
}
