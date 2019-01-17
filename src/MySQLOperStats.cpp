#include "MySQLOperStats.h"

std::string intToString(int i)
{
    std::stringstream ss;
    std::string s;
    if (i<10) ss << "0";
    ss << i;
    s = ss.str();

    return s;
}


MySQLOperStats::MySQLOperStats()
{

}



void MySQLOperStats::AddOperStat(_oper_stat operstat)
{
    operstats.push_back(operstat);
}

void MySQLOperStats::AddFileOperStat(_oper_stat fileoperstat)
{
    bool found_date;
    found_date=false;


    for (std::vector<_oper_stat>::iterator it = operstats.begin(); it != operstats.end(); ++it)
    {
        if (  (fileoperstat.dst_hub_id == it->dst_hub_id) &&
              (fileoperstat.src_hub_id == it->src_hub_id) &&
              (fileoperstat.dst_oper_id == it->dst_oper_id) &&
              (fileoperstat.src_oper_id == it->src_oper_id) &&
              (fileoperstat.dt.tm_year == it->dt.tm_year) &&
              (fileoperstat.dt.tm_mon == it->dt.tm_mon) &&
              (fileoperstat.dt.tm_mday == it->dt.tm_mday) &&
              (fileoperstat.status == it->status) &&
              (fileoperstat.ext_error == it->ext_error) &&
              (fileoperstat.ext_err_type == it->ext_err_type)
              )
        {
        found_date=true;
        long cnt_value=it->cnt_diff;
        cnt_value++;
        it->cnt_diff = cnt_value;
        break;
        }
    };

        if (found_date==false)
        {
        operstats.push_back(fileoperstat);
        };



}


void MySQLOperStats::AddDateInFile(_data tmp_data)
{
    bool found_date;
    found_date=false;

    for (std::vector<_data>::iterator it = datetimes_in_file.begin(); it != datetimes_in_file.end(); ++it) {
        if (  (tmp_data.year == it->year) &&  (tmp_data.mon == it->mon) && (tmp_data.day == it->day)      )
        {
        found_date=true;
        break;
        }
    };

    if (found_date==false)
    {
    datetimes_in_file.push_back(tmp_data);
    };


}

void MySQLOperStats::OperStatDtWhere(char *where_query)
{

    for (std::vector<_data>::iterator it = datetimes_in_file.begin(); it != datetimes_in_file.end(); ++it)
    {
         strcat(where_query," dt='");
         strcat(where_query, intToString(it->year).c_str());
         strcat(where_query, "-");
         strcat(where_query, intToString(it->mon).c_str());
         strcat(where_query, "-");
         strcat(where_query, intToString(it->day).c_str());
         strcat(where_query,"' or");
    };
    int where_query_length=strlen(where_query);
    where_query[where_query_length-2]=0x00;
}
