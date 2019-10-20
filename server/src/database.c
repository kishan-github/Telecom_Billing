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

	if(create_tables() != SUCCESS)
	{
		PRINT("Creation of tables failed.");
		return FAILURE;
	}
	else
		PRINT("Tables created successfully.");

	return SUCCESS;
}

int deinit_database()
{
	char query[MAX_LEN];

	sprintf(query, DELETE_USER_DETAILS_TABLE);
	if (mysql_query(mysql, query))
	{
		PRINT("Deletion of user details table failed.");
		return FAILURE;
	}

	sprintf(query, DELETE_USER_STATUS_TABLE);
	if (mysql_query(mysql, query))
	{
		PRINT("Deletion of user status table failed.");
		return FAILURE;
	}

	return SUCCESS;
}

int create_tables()
{
	char query[MAX_LEN];

	sprintf(query, CREATE_USER_DETAILS_TABLE);
	if (mysql_query(mysql, query))
	{
		PRINT("Creation of user details table failed.");
		return FAILURE;
	}

	sprintf(query, CREATE_USER_STATUS_TABLE);
	if (mysql_query(mysql, query))
	{
		PRINT("Creation of user status table failed.");
		return FAILURE;
	}

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
		PRINT("Failed to add user details in user_details table.");
		fprintf(stderr, "%s\n", mysql_error(mysql));
		return FAILURE;
	}

	sprintf(query, "INSERT INTO user_status (status) VALUES ('%d')", USER_AVAILABLE);
	if (mysql_query(mysql, query))
	{
		PRINT("Failed to add user details in user_status table.");
		fprintf(stderr, "%s\n", mysql_error(mysql));
		return FAILURE;
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
		PRINT("SQL query failed.");
		fprintf(stderr, "%s\n", mysql_error(mysql));
		return FAILURE;
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

int get_user_status(int user_id, user_status_t *status)
{
	MYSQL_RES *res;
	MYSQL_ROW row;
	int index = 0;
	char query[MAX_LEN];

	sprintf(query, "SELECT status FROM user_status where user_id = '%d'", user_id);
	if (mysql_query(mysql, query))
	{
		PRINT("SQL query failed.");
		fprintf(stderr, "%s\n", mysql_error(mysql));
		return FAILURE;
	}

	res = mysql_store_result(mysql);
	row = mysql_fetch_row(res);
	if(row)
	{
		*status = atoi(row[index]);
	}
	else
	{
		PRINT("No result for query (%s)", query);
		mysql_free_result(res);
		return FAILURE;
	}

	mysql_free_result(res);

	return SUCCESS;
}

int get_user_id(char *number, int *user_id)
{
	MYSQL_RES *res;
	MYSQL_ROW row;
	int index = 0;
	char query[MAX_LEN];

	sprintf(query, "SELECT user_id FROM user_details where ph_no = '%s'", number);
	if (mysql_query(mysql, query))
	{
		PRINT("SQL query failed.");
		fprintf(stderr, "%s\n", mysql_error(mysql));
		return FAILURE;
	}

	res = mysql_store_result(mysql);
	row = mysql_fetch_row(res);
	if(row)
	{
		*user_id = atoi(row[index]);
	}
	else
	{
		PRINT("No result for query (%s)", query);
		mysql_free_result(res);
		return FAILURE;
	}

	mysql_free_result(res);

	return SUCCESS;
}

int get_user_connection_fd(int user_id, int *receiver_connfd)
{
	MYSQL_RES *res;
	MYSQL_ROW row;
	int index = 0;
	char query[MAX_LEN];

	sprintf(query, "SELECT connfd FROM user_details where user_id = '%d'", user_id);
	if (mysql_query(mysql, query))
	{
		PRINT("SQL query failed.");
		fprintf(stderr, "%s\n", mysql_error(mysql));
		return FAILURE;
	}

	res = mysql_store_result(mysql);
	row = mysql_fetch_row(res);
	if(row)
	{
		*receiver_connfd = atoi(row[index]);
	}

	mysql_free_result(res);

	return SUCCESS;
}

int set_user_status(int user_id, user_status_t status)
{
	char query[MAX_LEN];

	sprintf(query, "UPDATE user_status SET status = '%d' where user_id = '%d'", status, user_id);
	if (mysql_query(mysql, query))
	{
		PRINT("SQL query failed.");
		fprintf(stderr, "%s\n", mysql_error(mysql));
		return FAILURE;
	}

	return SUCCESS;
}

int get_user_number(int user_id, char *number)
{
	MYSQL_RES *res;
	MYSQL_ROW row;
	int index = 0;
	char query[MAX_LEN];

	sprintf(query, "SELECT ph_no FROM user_details where user_id = '%d'", user_id);
	if (mysql_query(mysql, query))
	{
		PRINT("SQL query failed.");
		fprintf(stderr, "%s\n", mysql_error(mysql));
		return FAILURE;
	}

	res = mysql_store_result(mysql);
	row = mysql_fetch_row(res);
	if(row)
	{
		strcpy(number, row[index]);
	}

	mysql_free_result(res);

	return SUCCESS;
}
