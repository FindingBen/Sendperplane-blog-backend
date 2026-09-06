#include "handler/db.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>


void load_env(const char *path){

    FILE *file = fopen(path, "r");
    if(file == NULL){
        fprintf(stderr, "Could not open env file: %s\n", path);
        return;
    }

    char line[256];
    while(fgets(line, sizeof(line), file)){
        line[strcspn(line, "\r\n")] = '\0';

        if(line[0] == '\0' || line[0] == '#'){
            continue;
        }

        char *entry = malloc(strlen(line) + 1);
        if(entry == NULL){
            continue;
        }
        strcpy(entry, line);

        _putenv(entry);
    }

    fclose(file);
}

PGconn *initDatabase(){

    char conninfo[256];
    snprintf(conninfo, sizeof(conninfo),
        "user=%s password=%s dbname=%s host=%s port=%s",
        getenv("DB_USER"), getenv("DB_PASSWORD"), getenv("DB_DATABASE"),
        getenv("DB_HOST"), getenv("DB_PORT"));

    PGconn *conn = PQconnectdb(conninfo);

    return conn;
}


char *executeGetQueryToJson(const char *query){

    PGconn *conn = initDatabase();


    if(PQstatus(conn) != CONNECTION_OK){
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return NULL;
    }

    PGresult *result = PQexec(conn, query);


    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Query execution failed: %s", PQerrorMessage(conn));
        PQclear(result);
        PQfinish(conn);
        return NULL;
    }

    char *jsonResult = formatResultAsJson(result);

    PQclear(result);
    PQfinish(conn);

    return jsonResult;

}

char *executeGetUserForAuth(const char *query, const char *const *params){

    PGconn *conn = initDatabase();

    if(PQstatus(conn) != CONNECTION_OK){
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return NULL;
    }

    PGresult *result = PQexecParams(conn, query, 1, NULL, params, NULL, NULL, 0);

    if(PQresultStatus(result) != PGRES_TUPLES_OK){
        fprintf(stderr, "Query execution failed: %s", PQerrorMessage(conn));
        PQclear(result);
        PQfinish(conn);
        return NULL;
    }
    char *jsonResult = formatResultAsJson(result);
    PQclear(result);
    PQfinish(conn);

    return jsonResult;
}

int executePostQueryToJson(const char *query, const char *const *params){

    PGconn *conn = initDatabase();

    if(PQstatus(conn) != CONNECTION_OK){
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return 0;
    }

    PGresult *result = PQexecParams(conn, query, 3, NULL, params, NULL, NULL, 0);

    if(PQresultStatus(result) != PGRES_COMMAND_OK){
        fprintf(stderr, "Insert failed: %s", PQerrorMessage(conn));
        PQclear(result);
        PQfinish(conn);
        return 0;
    }

    PQclear(result);
    PQfinish(conn);

    return 1;

}

char *formatResultAsJson(PGresult *result){

    int numOfRows = PQntuples(result);
    int numOfCols = PQnfields(result);

    int totalSize = numOfRows * (2 + numOfCols * 256) + numOfRows -1 + 3;

    char *json = (char *)malloc(totalSize); // allocated memory for json repsonse
    json[0] = '\0';

    if(numOfRows == 0){
        strcat(json,"[]");
        return json;
    }

    for(int i = 0; i < numOfRows;++i){ //[{"col":"val"
        strcat(json,"{");
        for(int j = 0;j<numOfCols;++j){
            if (j > 0) strcat(json, ",");
            char *rowVal = PQgetvalue(result,i,j);
            char *colVal = PQfname(result, j);

            strcat(json,"\"");
            strncat(json,colVal,totalSize - strlen(json) -5);
            strcat(json, "\":\"");
            strncat(json,rowVal,totalSize - strlen(json) - 5);
            strcat(json,"\"");


        }
        strcat(json,"}");
        if(i < numOfRows -1 ) strcat(json,",");
    }

    return json;
}

char *extractValuesForJson(char *value, char *col){

    char *key = strstr(value, col);
    if(key == NULL){
        return NULL;
    }

    char *value_ = strchr(key, ':') + 2; // skip colon + opening quote

    size_t len = strcspn(value_, "\""); // count chars up to the closing quote, without mutating value

    char *extracted_val = malloc(len + 1);
    if(extracted_val == NULL){
        return NULL;
    }

    memcpy(extracted_val, value_, len);
    extracted_val[len] = '\0';

    return extracted_val;

}