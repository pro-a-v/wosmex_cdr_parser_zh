#ifndef MYSQLDATA_H
#define MYSQLDATA_H

#include <mysql/mysql.h>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <vector>
#include <map>
#include "definitions.h"
#include "MySQLOperators.h"
#include "FileLoader.h"
#include "MySQLHubs.h"
#include "MySQLOperStats.h"


class MySQLData
{
public:
    MySQLData();
    ~MySQLData();
    void MYSQLNULL();
    void Connect(const char *host, const char *user,const  char *pass, const char *db, int port);
    bool CheckConnect();
    bool FillOperators(MySQLOperators *);
    bool FillHubs(MySQLHubs *);
    bool FillOperStats(MySQLOperStats *);
    bool FileWasProcessed(std::string filename,bool *result);
    //----
    bool StartTransaction();
    bool InsertTransaction(char *insert_query);
    bool InsertTransactionFile(std::string insert_query);
    bool InsertTransactionCDRs(std::vector<FileLoader::_cdr> token);
    bool InsertTransactionOperStats(MySQLOperStats *CDRsOperStats);
    bool RollbackTransaction();
    bool CommitTransaction();
    //---
    bool SaveOperator(MySQLOperators::_operator *new_oper);
    bool SaveHub(MySQLHubs::_hub *new_hub);
    char ErrorText[TEXT_ERROR_MAX_SIZE];
    bool ConvertToLong(std::string curr_value,std::string field_name, long *result);
    bool ConvertToInt(std::string curr_value,std::string field_name, int *result);
    std::string LongToStrWithNULL(long value);
    bool isConnected();

    // Mysql Helper
    struct _query_fields
    {
        int id;
        char name[MYSQL_FIELDNAME_SIZE];
        unsigned int name_lenght;
        enum_field_types field_type;
    };

private:
    MYSQL mysql;
    MYSQL_RES *rs;
    MYSQL_ROW row;
    MYSQL_FIELD *field;

    bool connected;
    bool transaction_started;

    struct _full_query_fields
    {
        unsigned int count;
        std::vector<_query_fields> field;
    };

    _full_query_fields query_fields;

    bool CompareStrings(char field_name_1[MYSQL_FIELDNAME_SIZE],char field_name_2[MYSQL_FIELDNAME_SIZE]);
    bool FieldByNameHelper();
    int  FieldByName(std::string field_name);

};


#endif // MYSQLDATA_H
