#include <init.h>
#include <socket.h>
#include <database.h>

// Method to initialize the objects.
int init(char *argv)
{
	// create a socket for communication with client.
        if(create_socket(argv) == FAILURE)
        {
                PRINT("Socket creation failed.");
                return FAILURE;
        }
        else
        	PRINT("Socket created successfully.");

        // initialize the database.
        if(init_database() == FAILURE)
        {
        	PRINT("Database initialization failed.");
        	return FAILURE;
        }

	return SUCCESS;
}
