#include <database.h>

MYSQL *mysql = NULL;

// Create the database and initialize it.
int init_database()
{
	// Init sql.
	mysql = mysql_init(NULL);

	/*connect with the database*/
	if (!mysql_real_connect(mysql, SERVER, USER, PASSWORD, DATABASE, 0, NULL, 0))
	{
		fprintf(stderr, "%s\n", mysql_error(mysql));
		mysql_close(mysql);
		return FAILURE;
	}
	else
		PRINT("Successfully connected to database.");

	return SUCCESS;
}

int add_number_in_database(int socket_fd, char *phone_number)
{
	char query[MAX_LEN];
	bool is_available = false;

	// Check if number is already available in database.
	if(is_number_already_in_database(phone_number, &is_available) == FAILURE)
	{
		PRINT("Something went wrong....");
		return FAILURE;
	}

	if(is_available)
	{
		PRINT("Number is already registered.");
		return SUCCESS;
	}

	// Add the number to database.
	sprintf(query, "INSERT INTO user_details (ph_no, connfd) VALUES ('%s','%d')", phone_number, socket_fd);
	if (mysql_query(mysql, query))
	{
		fprintf(stderr, "%s\n", mysql_error(mysql));
		mysql_close(mysql);
		exit(FAILURE);
	}

	PRINT(" Number %s is successfully added in database.", phone_number);
	return SUCCESS;
}

int is_number_already_in_database(char *phone_number, bool *is_available)
{
	MYSQL_RES *res;
	MYSQL_ROW row;
	int num_fields;
	int index = 0;
	char query[MAX_LEN];

	sprintf(query, "SELECT ph_no FROM user_details");
	if (mysql_query(mysql, query))
	{
		fprintf(stderr, "%s\n", mysql_error(mysql));
		mysql_close(mysql);
		exit(FAILURE);
	}

	res = mysql_store_result(mysql);
	num_fields = mysql_num_fields(res);
	while ((row = mysql_fetch_row(res)))
	{
		for (index = 0; index < num_fields; index++)
		{
			if (strcmp(row[index], phone_number) == 0)
			{
				*is_available = true;
				return SUCCESS;
			}
		}
	}
	mysql_free_result(res);

	return SUCCESS;
}
