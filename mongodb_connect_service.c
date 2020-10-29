//This example establishes a connection to a standalone server on localhost, registers the client application as “connect-example” and performs a simple command.

#include <mongoc/mongoc.h>
#include <stdio.h>

int main (int argc, char *argv[])
{
	const char *uri_string = "mongodb://localhost:27017";	//keyword const makes this string uneditable
	mongoc_uri_t *uri;					//provides an abstraction on top of the MongoDB connection URI format (here probably: mongodb://, other example http:// )
	mongoc_client_t *client; 
	mongoc_database_t *database;
	mongoc_collection_t *collection;			//provides access to a MongoDB collection, useful for actions for most CRUD operations (insert, update, delete, etc.)
	bson_t *command, reply, *insert;
	bson_error_t error;
	char *str;

	//initialize libmongoc
	mongoc_init();

	//get MongoDB URI from command line (optional)
	if(argc > 1) uri_string = argv[1];

	//create a MongoDB object from given string
	uri = mongoc_uri_new_with_error(uri_string, &error);
	if(!uri){
		fprintf(stderr, "failed to parse URI: %s\n error message: %s\n", uri_string, error.message);
		return EXIT_FAILURE;
	}

	//Create a new client instance
	client = mongoc_client_new_from_uri(uri);
	if (!client) return EXIT_FAILURE;

	//register application name to make it traceable in profile logs
	mongoc_client_set_appname(client, "connect-example");

	//get handle on database db_name and collection coll_name
	database = mongoc_client_get_database(client, "db_name");
	collection = mongoc_client_get_collection(client, "db_name","coll_name");

	//ping database, print result as JSON, perform insert
	command = BCON_NEW("ping", BCON_INT32 (1));
	if(!(mongoc_client_command_simple(client, "admin", command, NULL, &reply, &error))){
		fprintf(stderr, "%s\n", error.message);
		return EXIT_FAILURE; 
	}

	str = bson_as_json(&reply, NULL);
	printf("%s\n", str);

	insert = BCON_NEW("hello", BCON_UTF8("world"));

	if(!(mongoc_collection_insert_one(collection, insert, NULL, NULL, &error))) fprintf(stderr, "%s\n", error.message);

	bson_destroy(insert);
	bson_destroy(&reply);
	bson_destroy(command);
	bson_free(str);

	//release handles and clean up
	mongoc_collection_destroy(collection);
	mongoc_database_destroy(database);
	mongoc_uri_destroy(uri);
	mongoc_client_destroy(client);
	mongoc_cleanup();

	
	puts("Compile-Test!\n");
	return 0;
}
