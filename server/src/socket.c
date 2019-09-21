#include <socket.h>

int create_socket(char *argv[])
{
	int 	port_number = 0,
            	status	= 0,
		socket_fd = 0;
        struct sockaddr_in server_addr;

        port_number = atoi(argv[1]);
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port_number);
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	/* Create socket */
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	ERROR_CHECK(socket_fd, "Socket creation failed.");
	
	/* Binding */
        status = bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
        ERROR_CHECK(status, "Bind failed.");

        /* listen */
        status = listen(socket_fd, MAX_CLIENT);
        ERROR_CHECK(status, "Listen failed.");

        return socket_fd;
}
