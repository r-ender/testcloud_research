//To insert documents into a collection, first obtain a handle to a mongoc_collection_t via a mongoc_client_t. Then, use mongoc_collection_insert_one() to add BSON documents to the collection. 

#include <mongoc/mongoc.h>
#include <bson/bson.h>
#include <stdio.h>


int main(int argc, char *argv[])
{
	mongoc_client_t *client;
	mongoc_collection_t *collection;
	bson_error_t error;
	bson_oid_t oid, oid2;
	bson_t *doc, *doc2;

	mongoc_init();

	client = mongoc_client_new("mongodb://localhost:27017/?appname=insert-example");
	collection = mongoc_client_get_collection(client, "BSI_test_db","BSI_test_collection");

	//generate docs with object ID
	doc = bson_new();
	bson_oid_init(&oid, NULL);
	bson_append_oid(doc, "_id", -1, &oid);
	bson_append_utf8(doc,"hello",-1,"BSI-world",-1);

	doc2 = bson_new();
	bson_oid_init(&oid2, NULL);
	bson_append_oid(doc2, "_id2", (int)strlen("_id2"),&oid2);
	bson_append_utf8(doc,"hello2",-1,"BSI-world2",-1);

	//insert document into collection
	if(!mongoc_collection_insert_one(collection,doc, NULL, NULL, &error)) fprintf(stderr, "%s\n", error.message);
	if(!mongoc_collection_insert_one(collection,doc2,NULL, NULL, &error)) fprintf(stderr, "%s\n", error.message);

	//clean up collection
	bson_destroy(doc);
	bson_destroy(doc2);
	mongoc_collection_destroy(collection);
	mongoc_client_destroy(client);
	mongoc_cleanup();

	return 0;
}
