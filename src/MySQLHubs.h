#ifndef MYSQLHUBS_H
#define MYSQLHUBS_H
#include <vector>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <typeinfo>
#include "definitions.h"

class MySQLHubs
{
public:
    MySQLHubs();
    ~MySQLHubs();

    struct _hub
    {
        long db_id;
        char name[    MYSQL_HUBS_NAME_DATASIZE     ];
        bool from_db;
    };

    bool Set_ID(        char data[MYSQL_HUBS_ID_DATASIZE]);
    bool Set_Name(      char data[MYSQL_HUBS_NAME_DATASIZE], bool from_db);
    bool AddHub();
    bool AddHub(_hub new_hub);
    void Check_Hub(std::string _nubname, _hub *founded_hub);

    char ErrorText[TEXT_ERROR_MAX_SIZE];

private:
    struct _full_hubs
    {
        unsigned int count;
        unsigned int last_hub_from_db;
        bool has_new_hubs;
        std::vector<_hub> hub;
    };

    _hub tmp_hub;
    _full_hubs hubs;

};

#endif // MYSQLHUBS_H
