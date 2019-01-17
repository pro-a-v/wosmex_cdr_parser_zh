#include "MySQLOperators.h"

MySQLOperators::MySQLOperators()
{
    operators.has_new_operators=false;
    operators.count=0;
};

MySQLOperators::~MySQLOperators()
{

};




bool MySQLOperators::Set_ID(        char data[MYSQL_OPERATORS_id_DATASIZE])
{
    unsigned int data_lenght = strlen(data);

    unsigned int i;
    for (i=0;i<data_lenght;i++)
    {

        // check simbols in 0-9 range
        if (
                (data[i]=='0') || (data[i]=='1') || (data[i]=='2') || (data[i]=='3') || (data[i]=='4') ||
                (data[i]=='5') || (data[i]=='6') || (data[i]=='7') || (data[i]=='8') || (data[i]=='9')
           )
        {
            ;
        }
        else
        {
           sprintf(ErrorText,"Operator id = %s have non numeric sumbols ",data);
           return false;
        }
    };


    tmp_oper.db_id=atol(data);
    if (tmp_oper.db_id <= 0)
    {
        sprintf(ErrorText,"Operator id = %s cannot convert to long %ld ",data,tmp_oper.db_id);
        return false;
    }

return true;
};




void MySQLOperators::Check_Oper(std::string _opname,  _operator *founded_oper)
{

    // _opname can be null
    if (_opname.length()==0)
    {
        founded_oper->db_id = -1;
        founded_oper->from_db = 1;
        return;
    }


    founded_oper->db_id=0;
    founded_oper->from_db=false;
    founded_oper->opname = _opname;

    bool found_oper; found_oper=false;

    for (std::vector<_operator>::iterator it = operators.oper.begin(); it != operators.oper.end(); ++it) {
        if (founded_oper->opname.compare(it->opname) == 0)
        {
        founded_oper->from_db = 1;
        founded_oper->db_id = it->db_id;
        found_oper=true;
        break;
        }
    };

    if (found_oper==false)
    {
    founded_oper->from_db = 0;
    };

};


bool MySQLOperators::Set_opname(char data[MYSQL_OPERATORS_opname_DATASIZE], bool from_db)
{
    if (from_db==false)
        operators.has_new_operators=true;

    tmp_oper.opname = data;
    tmp_oper.from_db = from_db;

 return true;
};


bool MySQLOperators::AddOperator()
{
    operators.count++;
    operators.oper.push_back(tmp_oper);
    // for while optimization
    if (tmp_oper.from_db==true) operators.last_oper_from_db=operators.count;

return true;
}


bool MySQLOperators::AddOperator(_operator oper)
{
    operators.count++;
    operators.oper.push_back(oper);
    // for while optimization
    if (oper.from_db==true) operators.last_oper_from_db=operators.count;

return true;
}
