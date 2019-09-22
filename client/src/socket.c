#include <socket.h>

int create_socket(char *argv, int *socket_fd)
{
        int port_number = 0;
        struct  sockaddr_in server_addr;
	
	*socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	ERROR_CHECK(*socket_fd, "Socket failed.");

        memset(&server_addr, 0, sizeof(server_addr));
        port_number = atoi(argv);
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port_number);
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if(connect(*socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
        {
                return FAILURE;
        }
        return SUCCESS;
}
