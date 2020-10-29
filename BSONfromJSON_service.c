//For single documents, BSON can be created from JSON strings via bson_new_from_json

#include <bson/bson.h>
#include <stdio.h>


int main(int argc, char *argv[])
{
	bson_error_t error;
	bson_t *bson;
	char *string;

	const char *json = "{\"name\": {\"first\":\"Grace\", \"last\":\"Hopper\"}}";
	bson = bson_new_from_json((const uint8_t *)json, -1, &error);

	if(!bson) {
		fprintf(stderr, "%s\n", error.message);
		return EXIT_FAILURE;
	}

	string = bson_as_canonical_extended_json(bson, NULL);
	printf("%s\n", string);

	//open and save database-file in write mode
	FILE *db_file = fopen("BSONfromJSON.json","w");
	if (db_file == NULL) puts("Couldn't open file!\n");
	fprintf(db_file,"%s\n",string);
	close((int)db_file);


	bson_free(string);
	bson_destroy(bson);

	return 0;
}
