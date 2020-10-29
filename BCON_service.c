//BSON C Object Notation, BCON for short, is an alternative way of constructing BSON documents in a manner closer to the intended format.

#include <bson/bson.h>

int main (int argc, char *argv[])
{
	struct tm born = {0};
	struct tm died = {0};
	bson_t *document;
	char *str;

	born.tm_year = 6;
	born.tm_mon = 11;
	born.tm_mday = 9;

	died.tm_year = 92;
	died.tm_mon = 0;
	died.tm_mday = 1;


	document = BCON_NEW(
		"born", BCON_DATE_TIME (mktime(&born) * 1000),
		"died", BCON_DATE_TIME (mktime(&died) * 1000),
		"name", "{",
		"first", BCON_UTF8("Grace"),
		"last", BCON_UTF8 ("Hopper"),
		      "}",
		      "languages", "[",
		      BCON_UTF8 ("MATH-MATIC"),
		      BCON_UTF8 ("FLOW-MATIC"),
		      BCON_UTF8 ("COBOL"),
		      "]",
		      "degrees", "[",
		      "{", "degree", BCON_UTF8 ("BA"), "school", BCON_UTF8 ("Vassar"), "}",
		      "{", "degree", BCON_UTF8 ("PhD"), "school", BCON_UTF8 ("Yale"), "}",
		      "]");

	str = bson_as_canonical_extended_json(document, NULL);
	printf("%s\n", str);

	//open and save database-file in write mode
	FILE *db_file = fopen("BCON_file.json","w");
	if (db_file == NULL) puts("Couldn't open file!\n");
	fprintf(db_file,"%s\n",str);
	close((int)db_file);

	bson_free(str);
	bson_destroy(document);

	return 0;
}

