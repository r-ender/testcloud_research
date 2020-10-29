//Documents are stored in MongoDB’s data format, BSON. The C driver uses libbson to create BSON documents. Here: appending key-value pairs

#include <bson/bson.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	struct tm born = {0};
	struct tm died = {0};
	const char *lang_names[] = {"MATH-MATIC", "C++", "Assembler"};
	const char *schools[] = {"Ravensburger Ring", "Max-Born-Gymnasium"};
	const char *degrees[] = {"Abitur", "Bachelor"};
	const char *key;

	uint32_t i = 0;
	char buf[16];
	size_t keylen;
	bson_t *document, child, child2;		//The bson_t structure represents a BSON document, manages the underlying BSON encoded buffer, can append new data to the document
	char *str;

	document = bson_new();
	
	//Append birthdate (milliseconds since the UNIX epoch) to document, passing -1 for length argument tells libbson to calculate stringlen
	born.tm_year = 0;
	born.tm_mon = 0;
	born.tm_mday = 1;
	bson_append_date_time(document, "born", -1, mktime(&born) * 1000); //yields $numberlong comes from conversion BSON to JSON, document DBObject etc.

	//append deathdate
	died.tm_year = 92;
	died.tm_mon = 0;
	died.tm_mday = 1;
	BSON_APPEND_DATE_TIME(document, "died", mktime(&died)*1000); //BSON_APPEND_DATE_TIME would pass length -1 as default


	//append a subdocument
	bson_append_document_begin(document, "name",(int)strlen("name"), &child);
	bson_append_utf8(&child, "first",(int)strlen("first"), "Grace", (int)strlen("Grace"));
	bson_append_utf8(&child, "last",(int)strlen("last"),"Hopper",-1);
	bson_append_document_end(document, &child);

	bson_t child3;
	bson_append_document_begin(document, "nom", (int)strlen("nom"),&child3);
	bson_append_symbol(&child3,"mysymbol",-1,"a",-1);
	bson_append_document_end(document,&child3);


	//append array of strings, generate keys 0,1,2
	key = "languages";
	bson_append_array_begin(document, key, (int)strlen(key), &child);	// key = "languages" 
	for(i=0; i < (sizeof(lang_names)/sizeof(const char *));i++)
	{
		keylen = bson_uint32_to_string(i, &key, buf, sizeof(buf));
		bson_append_utf8(&child, key, (int) keylen, lang_names[i], -1);
	}
	bson_append_array_end(document,&child);


	//array of subdocuments
	key = "degrees";
	bson_append_array_begin(document,key, (int)strlen(key) , &child);
	for(i=0; i < (sizeof(degrees)/sizeof(const char *)); i++)
	{
		keylen = bson_uint32_to_string(i, &key, buf, sizeof(buf));
		bson_append_document_begin(&child, key, (int)keylen, &child2);
		BSON_APPEND_UTF8(&child2, "degree", degrees[i]);
		BSON_APPEND_UTF8(&child2, "school", schools[i]);
		bson_append_document_end(&child, &child2);
	}
	bson_append_array_end(document, &child);


	//print document as JSON string
	str = bson_as_canonical_extended_json(document, NULL);
	printf("%s\n", str);

	//open and save database-file in write mode
	FILE *db_file = fopen("appendingBSON.json","w");
	if (db_file == NULL) puts("Couldn't open file!\n");
	fprintf(db_file,"%s\n",str);
	close((int)db_file);

	bson_free(str);
	
	//clean up allocated bson documents
	bson_destroy(document);
	return 0;
}









































