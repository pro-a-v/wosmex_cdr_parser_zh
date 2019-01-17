#ifndef MYSQLOPERATORS_H
#define MYSQLOPERATORS_H
#include <vector>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <typeinfo>
#include "definitions.h"

class MySQLOperators
{
public:
    MySQLOperators();
    ~MySQLOperators();

    struct _operator
    {
        long db_id;
        std::string opname;
        bool from_db;
    };

    bool AddOperator();
    bool AddOperator(_operator oper);
    bool Set_ID(        char data[MYSQL_OPERATORS_id_DATASIZE]);
    bool Set_opname(    char data[MYSQL_OPERATORS_opname_DATASIZE],     bool from_db);
    void Check_Oper(std::string _opname,  _operator *founded_oper);
    char ErrorText[TEXT_ERROR_MAX_SIZE];

private:
    _operator tmp_oper;

    struct _full_operators
    {
        unsigned int count;
        unsigned int last_oper_from_db;
        bool has_new_operators;
        std::vector<_operator> oper;
    };

    _full_operators operators;
};

#endif // MYSQLOPERATORS_H
