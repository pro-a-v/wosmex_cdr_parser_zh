#include "MySQLData.h"

MySQLData::MySQLData()
{
   transaction_started=false;
}

MySQLData::~MySQLData()
{
     mysql_close(&mysql);
}

void MySQLData::MYSQLNULL()
{

    if (mysql_query(&mysql, "show tables;") == 0  )
    {
        if (mysql_field_count(&mysql) > 0)
        {
         rs = mysql_store_result(&mysql);
         mysql_free_result(rs);
        }
    }
    else
    {
        printf("The operation failed in 1 \n");
    };

};

void MySQLData::Connect(const char *host, const char *user, const char *pass, const char *db, int port)
{

    if (mysql_init(&mysql) == 0)
    {
        connected = false;
    }
    else
    {
       my_bool reconnect = 1;
       mysql_options(&mysql,MYSQL_OPT_RECONNECT,&reconnect);
       int seconds = 100;
       mysql_options(&mysql,MYSQL_OPT_CONNECT_TIMEOUT,&seconds);
       mysql_options(&mysql,MYSQL_OPT_READ_TIMEOUT,&seconds);
       mysql_options(&mysql,MYSQL_OPT_WRITE_TIMEOUT,&seconds);
       mysql_options(&mysql,MYSQL_INIT_COMMAND,"SET autocommit=0");
       mysql_options(&mysql,MYSQL_INIT_COMMAND,"SET unique_checks=0");
       mysql_options(&mysql,MYSQL_INIT_COMMAND,"SET foreign_key_checks=0");
       mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "utf8");


    };

    if (!mysql_real_connect(&mysql, host, user, pass, db, port , NULL, CLIENT_COMPRESS))
    {
        connected = false;
        snprintf(ErrorText,4096,"Error code: %d  %s %s",mysql_errno(&mysql),mysql_sqlstate(&mysql),mysql_error(&mysql));
    }
    else
    {
        connected = true;
        // disable auto commit
        my_bool autocommit = 0;
        mysql_autocommit(&mysql,autocommit);
    };

};

bool MySQLData::ConvertToLong(std::string curr_value,std::string field_name, long *result)
{
    // it can be NULL in DB
    if(curr_value.length()==0)
    {
       long tmp =-1;
       memcpy(result, &tmp, sizeof(long));
       return true;
    }

    unsigned int i;
    for (i=0;i<curr_value.length();i++)
    {

        // check simbols in 0-9 range
        if (
                (curr_value[i]=='0') || (curr_value[i]=='1') || (curr_value[i]=='2') || (curr_value[i]=='3') || (curr_value[i]=='4') ||
                (curr_value[i]=='5') || (curr_value[i]=='6') || (curr_value[i]=='7') || (curr_value[i]=='8') || (curr_value[i]=='9')
           )
        {
            ;
        }
        else
        {
           sprintf(ErrorText,"field %s have non numeric sumbols value(%s) ",field_name.c_str(),curr_value.c_str());
           return false;
        }
    };
    long tmp = atol(curr_value.c_str());
    memcpy(result,&tmp,sizeof(long));

    if (tmp < 0)
    {
        sprintf(ErrorText,"field %s with value %s cannot be converted to long ",field_name.c_str(),curr_value.c_str());
        return false;
    }

return true;
}

bool MySQLData::ConvertToInt(std::string curr_value,std::string field_name, int *result)
{
    // it can be NULL in DB
    if(curr_value.length()==0)
    {
       long tmp =-1;
       memcpy(result, &tmp, sizeof(long));
       return true;
    }

    unsigned int i;
    for (i=0;i<curr_value.length();i++)
    {

        // check simbols in 0-9 range
        if (
                (curr_value[i]=='0') || (curr_value[i]=='1') || (curr_value[i]=='2') || (curr_value[i]=='3') || (curr_value[i]=='4') ||
                (curr_value[i]=='5') || (curr_value[i]=='6') || (curr_value[i]=='7') || (curr_value[i]=='8') || (curr_value[i]=='9')
           )
        {
            ;
        }
        else
        {
           sprintf(ErrorText,"field %s have non numeric sumbols value(%s) ",field_name.c_str(),curr_value.c_str());
           return false;
        }
    };
    int tmp = atoi(curr_value.c_str());
    memcpy(result,&tmp,sizeof(int));

    if (tmp < 0)
    {
        sprintf(ErrorText,"field %s with value %s cannot be converted to int ",field_name.c_str(),curr_value.c_str());
        return false;
    }

return true;
}


bool MySQLData::isConnected()
{
    return connected;
}

bool MySQLData::CheckConnect()
{
    if (mysql_ping(&mysql)!=0)
    {
            connected = true;
            return true;
    }
    else
    {
            connected = false;
            return false;
    };
return true;
}

bool MySQLData::FillOperators(MySQLOperators *CDRsOperators)
{
    int state;
    state = mysql_query(&mysql,"select ID, OpName from operators;"); //, E212 , FineName

    if (state != 0)
    {
            sprintf(ErrorText,"Error load operators from DB: %s",mysql_error(&mysql));
            return false;
    };
    rs = mysql_store_result(&mysql);

    if (!FieldByNameHelper()) return false;


    if (FieldByName("ID")==-1)   { sprintf(ErrorText,"Field ID not found in operators "); return false; }
    if (FieldByName("OpName")==-1)   { sprintf(ErrorText,"Field OpName not found in operators "); return false; }
    //if (FieldByName("E212")==-1)   { sprintf(ErrorText,"Field E212 not found in operators "); return false; }
    //if (FieldByName("FineName")==-1)   { sprintf(ErrorText,"Field FineName not found in operators "); return false; }

    while( (row = mysql_fetch_row(rs)) != NULL )
    {
        char tmp[MYSQL_FIELDDATA_MAXSIZE];

        strncpy(tmp,row[FieldByName("ID")],MYSQL_FIELDDATA_MAXSIZE);
        CDRsOperators->Set_ID(tmp);
        memset(tmp,0x00,MYSQL_FIELDDATA_MAXSIZE);

        strncpy(tmp,row[FieldByName("OpName")],MYSQL_FIELDDATA_MAXSIZE);
        CDRsOperators->Set_opname(tmp,true);
        memset(tmp,0x00,MYSQL_FIELDDATA_MAXSIZE);

        /*
        strncpy(tmp,row[FieldByName("E212")],MYSQL_FIELDDATA_MAXSIZE);
        CDRsOperators->Set_E212(tmp,true);
        memset(tmp,0x00,MYSQL_FIELDDATA_MAXSIZE);

        strncpy(tmp,row[FieldByName("FineName")],MYSQL_FIELDDATA_MAXSIZE);
        CDRsOperators->Set_finename(tmp,true);
        memset(tmp,0x00,MYSQL_FIELDDATA_MAXSIZE);
        */

        CDRsOperators->AddOperator();
    };

    mysql_free_result(rs);

return true;
};


bool MySQLData::SaveOperator(MySQLOperators::_operator *new_oper)
{
    int state;
    char *str_query,str_escaped_query[255];
    mysql_real_escape_string(&mysql,str_escaped_query,new_oper->opname.c_str(),new_oper->opname.length());
    str_query = (char *)  malloc(100); memset(str_query,0x00,100);
    // forming insert query
    strcpy(str_query,"insert into operators(OpName) values('");
    strcat(str_query,str_escaped_query);
    strcat(str_query,"');");

    state = mysql_query(&mysql,str_query);
    free(str_query); // free memory, query stand needless
    if (state != 0)
    {
            sprintf(ErrorText,"Error insert operator %s into operators to DB: %s",new_oper->opname.c_str(), mysql_error(&mysql));
            return false;
    };

    if ((rs = mysql_store_result(&mysql)) == 0 && mysql_field_count(&mysql) == 0 && mysql_insert_id(&mysql) != 0)
    {
        new_oper->db_id = mysql_insert_id(&mysql);
        mysql_commit(&mysql);
    }
    else
    {
        mysql_rollback(&mysql);
        sprintf(ErrorText,"Error insert operator %s into operators to DB no result id found",new_oper->opname.c_str());
        return false;
    }


    mysql_free_result(rs);

return true;
}

bool MySQLData::FileWasProcessed(std::string filename, bool *result)
{
    int state;
    char *str_query,str_escaped_query[255];
    mysql_real_escape_string(&mysql,str_escaped_query,filename.c_str(),filename.size());
    str_query = (char *)  malloc(100); memset(str_query,0x00,100);
    // forming insert query
    strcpy(str_query,"select 1 from processed_files where file_name like '");
    strcat(str_query,str_escaped_query);
    strcat(str_query,"';");

    state = mysql_query(&mysql,str_query);
    free(str_query); // free memory, query stand needless
    if (state != 0)
    {
            sprintf(ErrorText,"Error check for file processed %s from DB: %s",filename.c_str(),mysql_error(&mysql));
            return false;
    };
    MYSQL_RES *mysqlResult = NULL;
    mysqlResult = mysql_store_result(&mysql);
    if (mysql_num_rows(mysqlResult) > 0 ) // there are rows
    {
       (*result)=true;
    }
    else
    {
        (*result)=false;    // Was not processed
    }

    mysql_free_result(rs);



return true;

}


bool MySQLData::SaveHub(MySQLHubs::_hub *new_hub)
{
    int state;
    char *str_query,str_escaped_query[255];
    mysql_real_escape_string(&mysql,str_escaped_query,new_hub->name,strlen(new_hub->name));
    str_query = (char *)  malloc(100); memset(str_query,0x00,100);
    // forming insert query
    strcpy(str_query,"insert into hubs(NAME) values('");
    strcat(str_query,str_escaped_query);
    strcat(str_query,"');");

    state = mysql_query(&mysql,str_query);
    free(str_query); // free memory, query stand needless
    if (state != 0)
    {
            sprintf(ErrorText,"Error insert hub %s into hubs to DB: %s",new_hub->name,mysql_error(&mysql));
            return false;
    };

    if ((rs = mysql_store_result(&mysql)) == 0 && mysql_field_count(&mysql) == 0 && mysql_insert_id(&mysql) != 0)
    {
        new_hub->db_id = mysql_insert_id(&mysql);
        mysql_commit(&mysql);
    }
    else
    {
        mysql_rollback(&mysql);
        sprintf(ErrorText,"Error insert hub %s into hubs to DB no result id found",new_hub->name);
        return false;
    }

    mysql_free_result(rs);

return true;
};



bool MySQLData::FillHubs(MySQLHubs *CDRsHubs)
{
    int state;
    state = mysql_query(&mysql,"select ID, NAME from hubs;");

    if (state != 0)
    {
            sprintf(ErrorText,"Error load hubs from DB: %s",mysql_error(&mysql));
            return false;
    };
    rs = mysql_store_result(&mysql);

    if (!FieldByNameHelper()) return false;

    if (FieldByName("ID")==-1)   { sprintf(ErrorText,"Field ID not found in operators "); return false; }
    if (FieldByName("NAME")==-1)   { sprintf(ErrorText,"Field NAME not found in operators "); return false; }

    while( (row = mysql_fetch_row(rs)) != NULL )
    {
        char tmp[MYSQL_FIELDDATA_MAXSIZE];

        strncpy(tmp,row[FieldByName("ID")],MYSQL_FIELDDATA_MAXSIZE);
        CDRsHubs->Set_ID(tmp);
        memset(tmp,0x00,MYSQL_FIELDDATA_MAXSIZE);

        strncpy(tmp,row[FieldByName("NAME")],MYSQL_FIELDDATA_MAXSIZE);
        CDRsHubs->Set_Name(tmp,true);
        memset(tmp,0x00,MYSQL_FIELDDATA_MAXSIZE);


        CDRsHubs->AddHub();
    };

    mysql_free_result(rs);

return true;
};


bool MySQLData::FillOperStats(MySQLOperStats *CDRsOperStats)
{
    // Agregate date where limitation
    char where_dt[1024]; memset(where_dt,0x00,1024);
    CDRsOperStats->OperStatDtWhere(&where_dt[0]);


    int state;
   // char chr_query[1024]; memset(chr_query,0x00,1024);
    std::string chr_query;
    chr_query = "SELECT `src_oper_id`,`dst_oper_id`,`src_hub_id`,`dst_hub_id`,`dt`,`status`, `ext_error`, `ext_err_type`, `id` ";
    chr_query = chr_query + "FROM OPER_STAT where ( ";
    chr_query = chr_query + where_dt;
    chr_query = chr_query + " );";


    state = mysql_query(&mysql,chr_query.c_str());

    if (state != 0)
    {
            sprintf(ErrorText,"Error load oper_stats from DB: %s",mysql_error(&mysql));
            return false;
    };
    rs = mysql_store_result(&mysql);

    if (!FieldByNameHelper()) return false;

    if (FieldByName("src_oper_id")==-1)   { sprintf(ErrorText,"Field src_oper_id not found in oper_stats "); return false; }
    if (FieldByName("dst_oper_id")==-1)   { sprintf(ErrorText,"Field dst_oper_id not found in oper_stats "); return false; }
    if (FieldByName("src_hub_id")==-1)   { sprintf(ErrorText,"Field src_hub_id not found in oper_stats "); return false; }
    if (FieldByName("dst_hub_id")==-1)   { sprintf(ErrorText,"Field dst_hub_id not found in oper_stats "); return false; }
    if (FieldByName("dt")==-1)          { sprintf(ErrorText,"Field dt not found in oper_stats "); return false; }
    if (FieldByName("status")==-1)   { sprintf(ErrorText,"Field status not found in oper_stats "); return false; }
    if (FieldByName("ext_error")==-1)   { sprintf(ErrorText,"Field ext_error not found in oper_stats "); return false; }
    if (FieldByName("ext_err_type")==-1)   { sprintf(ErrorText,"Field ext_error_type not found in oper_stats "); return false; }
    if (FieldByName("id")==-1)   { sprintf(ErrorText,"Field id not found in oper_stats "); return false; }


    while( (row = mysql_fetch_row(rs)) != NULL )
    {
        MySQLOperStats::_oper_stat tmp_stat;
        tmp_stat.from_db=true;
        tmp_stat.cnt_diff=0;
            if (!ConvertToLong((row[FieldByName("src_oper_id")] ? row[FieldByName("src_oper_id")]:""),"src_oper_id", &tmp_stat.src_oper_id)) return false;
            if (!ConvertToLong((row[FieldByName("dst_oper_id")] ? row[FieldByName("dst_oper_id")]:""),"dst_oper_id", &tmp_stat.dst_oper_id)) return false;
            if (!ConvertToLong((row[FieldByName("src_hub_id")] ? row[FieldByName("src_hub_id")]:""),"src_hub_id", &tmp_stat.src_hub_id)) return false;
            if (!ConvertToLong((row[FieldByName("dst_hub_id")] ? row[FieldByName("dst_hub_id")]:""),"dst_hub_id", &tmp_stat.dst_hub_id)) return false;
            if (!ConvertToLong((row[FieldByName("status")] ? row[FieldByName("status")]:""),"status", &tmp_stat.status)) return false;
            if (!ConvertToLong((row[FieldByName("ext_error")] ? row[FieldByName("ext_error")]:""),"ext_error", &tmp_stat.ext_error)) return false;
            if (!ConvertToLong((row[FieldByName("ext_err_type")] ? row[FieldByName("ext_err_type")]:""),"ext_err_type", &tmp_stat.ext_err_type)) return false;
            if (!ConvertToLong((row[FieldByName("id")] ? row[FieldByName("id")]:""),"id", &tmp_stat.db_id)) return false;
            // MysQL Date Parsing
            std::string tmp = row[FieldByName("dt")];
            std::string tmp_year = tmp.substr(0,4);
            std::string tmp_mon = tmp.substr(5,2);
            std::string tmp_day = tmp.substr(8,2);
            if (!ConvertToInt(tmp_year,"dt:year", &tmp_stat.dt.tm_year)) return false;
            if (!ConvertToInt(tmp_mon,"dt:mon", &tmp_stat.dt.tm_mon)) return false;
            if (!ConvertToInt(tmp_day,"dt:day", &tmp_stat.dt.tm_mday)) return false;

            CDRsOperStats->AddOperStat(tmp_stat);

    };

    mysql_free_result(rs);

return true;
};





bool MySQLData::FieldByNameHelper()
{
    query_fields.count=0;
    query_fields.field.clear();

    //===========================
    unsigned int i;
    unsigned int num_fields = mysql_num_fields(rs);
    for(i = 0 ; i < num_fields; i++)
    {
        field = mysql_fetch_field(rs);


            if (field->name_length > MYSQL_FIELDNAME_SIZE )
            {
                snprintf(ErrorText,TEXT_ERROR_MAX_SIZE,"Field Name = %s size %i bigger than %i",field->name,field->name_length,MYSQL_FIELDNAME_SIZE);

                return false;
            };

            _query_fields tmp_bd_field;


            tmp_bd_field.id=i;
            tmp_bd_field.name_lenght= field->name_length;
            tmp_bd_field.field_type = field->type;
            strncpy(tmp_bd_field.name,field->name,MYSQL_FIELDNAME_SIZE);

            query_fields.field.push_back(tmp_bd_field);
            query_fields.count++;
    };
return true;
};

// insert into operators(OpName,E212,FineName)values


int MySQLData::FieldByName(std::string field_name)
{
    char tmp_field_name[MYSQL_FIELDNAME_SIZE];
    strncpy(tmp_field_name,field_name.c_str(),MYSQL_FIELDNAME_SIZE);

    unsigned int i;
    for (i=0; i<query_fields.count; i++)
    {
        if (CompareStrings(query_fields.field[i].name, tmp_field_name))
            return query_fields.field[i].id;

    };

return -1;
};


bool MySQLData::CompareStrings(char field_name_1[MYSQL_FIELDNAME_SIZE],char field_name_2[MYSQL_FIELDNAME_SIZE])
{
    unsigned int str_1_len = strlen(field_name_1);
    unsigned int str_2_len = strlen(field_name_2);
    if (str_1_len!=str_2_len) return false;

    unsigned int i;
    for (i=0; i<str_1_len; i++)
    {
        if (field_name_1[i]!=field_name_2[i]) return false;
    };

return true;
}

bool MySQLData::StartTransaction()
{
    int state;
    state = mysql_query(&mysql,"START TRANSACTION;");

    if (state != 0)
    {
            snprintf(ErrorText,TEXT_ERROR_MAX_SIZE,"Error to start transaction in DB: %s",mysql_error(&mysql));
            return false;
    };
return true;
}

bool MySQLData::RollbackTransaction()
{
    int state;
    state = mysql_query(&mysql,"ROLLBACK;");

    if (state != 0)
    {
            snprintf(ErrorText,TEXT_ERROR_MAX_SIZE,"Error to rollback transaction in DB: %s",mysql_error(&mysql));
            return false;
    };
return true;
}


bool MySQLData::CommitTransaction()
{
    int state;
    state = mysql_query(&mysql,"COMMIT;");

    if (state != 0)
    {
            snprintf(ErrorText,TEXT_ERROR_MAX_SIZE,"Error to commit transaction in DB: %s",mysql_error(&mysql));
            return false;
    };
return true;
}


bool MySQLData::InsertTransaction(char *insert_query)
{
    int state;
    state = mysql_query(&mysql,insert_query);

    if (state != 0)
    {
            snprintf(ErrorText,TEXT_ERROR_MAX_SIZE,"Error to insert %s in transaction to DB: %s",insert_query,mysql_error(&mysql));
            return false;
    }

return true;
}

bool MySQLData::InsertTransactionFile(std::string insert_query)
{
    int state;
    char *str_query,str_escaped_query[255];
    mysql_real_escape_string(&mysql,str_escaped_query,insert_query.c_str(),insert_query.size());
    str_query = (char *)  malloc(100); memset(str_query,0x00,100);
    // forming insert query
    strcpy(str_query,"insert into processed_files(dt,file_name) values(CURRENT_TIMESTAMP,'");
    strcat(str_query,str_escaped_query);
    strcat(str_query,"');");

    state = mysql_query(&mysql,str_query);
    free(str_query); // free memory, query stand needless
    if (state != 0)
    {
            sprintf(ErrorText,"Error insert into processed_files %s to DB: %s",insert_query.c_str(),mysql_error(&mysql));
            return false;
    };

    if ((rs = mysql_store_result(&mysql)) == 0 && mysql_field_count(&mysql) == 0 && mysql_insert_id(&mysql) != 0)
    {
        return true;

    }
    else
    {
        sprintf(ErrorText,"Error insert into processed_files");
        return false;
    }


    mysql_free_result(rs);

return true;
}

bool MySQLData::InsertTransactionCDRs(std::vector<FileLoader::_cdr> token)
{
    std::string insert_query;
    bool first_insert=true;
    insert_query = "INSERT INTO `CDRs` (`CDR_TYPE`, `CDR_TIME`, `SRC_HUB`, `DST_HUB`, `SRC_OPERATOR`, `DST_OPERATOR`, `OA`, `DA`, `DLVR_ATTEMPTS`, `STATUS`, `MESSAGE_ID`, `INT_ERROR`, `EXT_CODE`, `EXT_ERROR`, `BLOCKING_REASON`, `SRI_DIRECTION`, `MAP_ERROR`) VALUES ";

    for (std::vector<FileLoader::_cdr>::iterator it = token.begin(); it != token.end(); ++it) {

        if (!first_insert)
        {
           insert_query = insert_query + ",";
        }
        first_insert=false;


           insert_query = insert_query + " (";
           insert_query = insert_query+ LongToStrWithNULL(it->CDR_TYPE) +  // `CDR_TYPE`,
                ",'" + LongToStrWithNULL(it->dt.tm_year) +"-" + LongToStrWithNULL(it->dt.tm_mon) + "-" + LongToStrWithNULL(it->dt.tm_mday) + " " + LongToStrWithNULL(it->dt.tm_hour) + ":" +  LongToStrWithNULL(it->dt.tm_min) + ":" + LongToStrWithNULL(it->dt.tm_sec) +  // `CDR_TIME`,
                "'," + LongToStrWithNULL(it->src_hub_id) + //  `SRC_HUB`,
                ","  + LongToStrWithNULL(it->dst_hub_id) + //  `DST_HUB`,
                ","  + LongToStrWithNULL(it->src_oper_id) + //  `SRC_OPERATOR`,
                ","  + LongToStrWithNULL(it->dst_oper_id) + //  `DST_OPERATOR`,
                ",'" + it->OA + // `OA`,
                "','"+ it->DA + //  `DA`,
                "'," + LongToStrWithNULL(it->DLVR_ATTEMPTS) + // `DLVR_ATTEMPTS`,
                ","  + LongToStrWithNULL(it->status) +        //`STATUS`,
                ",'" + it->MESSAGE_ID +                       // `MESSAGE_ID`,
                "'," + LongToStrWithNULL(it->INT_ERROR) +     //   `INT_ERROR`,
                ","  + LongToStrWithNULL(it->ext_err_type) +  // `EXT_CODE`,
                ","  + LongToStrWithNULL(it->ext_error) +     // `EXT_ERROR`,
                ","  + LongToStrWithNULL(it->BLOCKING_REASON) + // `BLOCKING_REASON`,
                ","  + LongToStrWithNULL(it->SRI_DIRECTION) +  // `SRI_DIRECTION`,
                ","  + LongToStrWithNULL(it->MAP_ERROR) +      // `MAP_ERROR`
                ")";
    };
    insert_query = insert_query + ";";

    int state = mysql_query(&mysql,insert_query.c_str());
    if (state != 0)
    {
            //snprintf(ErrorText,TEXT_ERROR_MAX_SIZE,"Error insert into CDRs %s",mysql_error(&mysql));
            snprintf(ErrorText,TEXT_ERROR_MAX_SIZE,"Error insert into CDRs %s \n %s \n",mysql_error(&mysql), insert_query.c_str());
            return false;
    };

    if ((rs = mysql_store_result(&mysql)) == 0 && mysql_field_count(&mysql) == 0 && mysql_insert_id(&mysql) != 0)
    {
        ; // all is ok. running
    }
    else
    {
        snprintf(ErrorText,TEXT_ERROR_MAX_SIZE,"Error insert into CDRs %s \n %s \n",mysql_error(&mysql), insert_query.c_str() );
        return false;
    }

return true;
}

bool MySQLData::InsertTransactionOperStats(MySQLOperStats *CDRsOperStats)
{


    for (std::vector<MySQLOperStats::_oper_stat>::iterator it = CDRsOperStats->operstats.begin(); it != CDRsOperStats->operstats.end(); ++it)
    {
    if(it->cnt_diff > 0)
    {
        std::string update_insert_query; update_insert_query = "";
        if (it->from_db==true)   // UPDATE
        {
            update_insert_query = update_insert_query + "UPDATE OPER_STAT SET cnt = cnt + " +  LongToStrWithNULL(it->cnt_diff)  + " where id = " + LongToStrWithNULL(it->db_id) + "; \n";
        }
        else           // INSERT  src_oper_id  dst_oper_id   src_hub_id	dst_hub_id	  dt	status	 ext_error	ext_err_type	 cnt
        {
            update_insert_query = update_insert_query + "INSERT INTO OPER_STAT (`src_oper_id`,`dst_oper_id`,`src_hub_id`,`dst_hub_id`,`dt`,`status`,`ext_error`,`ext_err_type`,`cnt`)";
            update_insert_query = update_insert_query +
                    " VALUES(" +
                    LongToStrWithNULL(it->src_oper_id) + "," +
                    LongToStrWithNULL(it->dst_oper_id) + "," +
                    LongToStrWithNULL(it->src_hub_id) + "," +
                    LongToStrWithNULL(it->dst_hub_id) + "," +
                    "'" + LongToStrWithNULL(it->dt.tm_year) +"-" + LongToStrWithNULL(it->dt.tm_mon) + "-" + LongToStrWithNULL(it->dt.tm_mday) + "'," +
                    LongToStrWithNULL(it->status) + "," +
                    LongToStrWithNULL(it->ext_error) + "," +
                    LongToStrWithNULL(it->ext_err_type) + "," +
                    LongToStrWithNULL(it->cnt_diff) +
                    "); \n";
        }

        int state = mysql_query(&mysql,update_insert_query.c_str());
        if (state != 0)
        {
                snprintf(ErrorText,TEXT_ERROR_MAX_SIZE,"Error insert/update %s : %s",update_insert_query.c_str(),mysql_error(&mysql));
                return false;
        };

        if (   (rs = mysql_store_result(&mysql)) == 0
               &&
               mysql_field_count(&mysql) == 0
               &&
               ( mysql_insert_id(&mysql) == 1  ||  mysql_affected_rows(&mysql)==1 )     // insert or update
            )
        {
            ; // all is ok. running
        }
        else
        {
            snprintf(ErrorText,TEXT_ERROR_MAX_SIZE,"Error insert %s : %s",update_insert_query.c_str(),mysql_error(&mysql));
            return false;
        }
    };
    };




return true;
}


std::string MySQLData::LongToStrWithNULL(long value)
{
    if (value<0) return "NULL";
    else return std::to_string(static_cast<unsigned long long>(value));

}

