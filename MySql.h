#ifndef __MYSQL_H__
#define __MYSQL_H__

#include <mysql/mysql.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "BO.h"

#define  MAX_BUF_SIZE 4096



MYSQL *g_conn;
MYSQL_RES *g_res;
MYSQL_ROW g_row;

typedef struct mySqlTask
{
char* sqlbuf;
char* tableName;
BO_Unit_t** boList;
}mySqlTask_t;

typedef struct csvTask
{
    FILE ** fptr;
}csvTask_t;

void print_mysql_error(const char *msg);

int executesql(const char *sql);

int init_mysql();

void CanbusMySqlTest();

void RecordDataCsv();

FILE * recordFile;
#endif // MYSQL_H_INCLUDED
