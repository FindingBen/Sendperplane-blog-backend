#ifndef DB_HANDLER
#define DB_HANDLER

#include <libpq-fe.h>

void load_env(const char *path);

PGconn *initDatabase();

char *executeGetQueryToJson(const char *query);

int executePostQueryToJson(const char *query, const char *const *params);

char *formatResultAsJson(PGresult *result);

char *extractValuesForJson(char *value, char *col);


char *executeGetUserForAuth(const char *query, const char *const *param);


#endif