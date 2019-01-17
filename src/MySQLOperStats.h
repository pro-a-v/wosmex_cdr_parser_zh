#ifndef MYSQLOPERSTATS_H
#define MYSQLOPERSTATS_H
#include <vector>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <typeinfo>
#include <ctime>
#include <sstream>
#include "definitions.h"

class MySQLOperStats
{
public:
    MySQLOperStats();
    ~MySQLOperStats();

    struct _oper_stat
    {
        long db_id;
        long src_oper_id;
        long dst_oper_id;
        long src_hub_id;
        long dst_hub_id;
        struct tm dt;
        long  status;
        long  ext_error;
        long ext_err_type;
        long cnt_diff;
        bool from_db;
    };

    struct _data
    {
        int year;
        int mon;
        int day;
    };


    void AddDateInFile(_data tmp_data);
    void AddOperStat(_oper_stat operstat);
    void AddFileOperStat(_oper_stat operstat);
    void OperStatDtWhere(char *where_query);
    std::vector<_oper_stat> operstats;

private:
    std::vector<_data> datetimes_in_file;


};

#endif // MYSQLOPERSTATS_H
