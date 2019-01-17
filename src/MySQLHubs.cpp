#include "MySQLHubs.h"

MySQLHubs::MySQLHubs()
{
    hubs.has_new_hubs=false;
    hubs.count=0;
}

MySQLHubs::~MySQLHubs()
{

}

bool MySQLHubs::Set_ID(        char data[MYSQL_HUBS_ID_DATASIZE])
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


    tmp_hub.db_id=atol(data);
    if (tmp_hub.db_id <= 0)
    {
        sprintf(ErrorText,"Operator id = %s cannot convert to long %ld ",data,tmp_hub.db_id);
        return false;
    }

return true;
};

bool MySQLHubs::Set_Name(char data[MYSQL_HUBS_NAME_DATASIZE], bool from_db)
{
    if (from_db==false)
        hubs.has_new_hubs=true;

    strncpy(tmp_hub.name,data,MYSQL_HUBS_NAME_DATASIZE);
    tmp_hub.from_db = from_db;

 return true;
};

void MySQLHubs::Check_Hub(std::string _nubname, _hub *founded_hub)
{
    founded_hub->db_id=-1;

    // name of hub can be absent
    if (_nubname.length()==0)
    {
        founded_hub->from_db = 1;
        return;
    };

    memset(founded_hub->name,0x00,MYSQL_HUBS_NAME_DATASIZE);
    strncpy(founded_hub->name,_nubname.c_str(),MYSQL_HUBS_NAME_DATASIZE);

    bool found_hub; found_hub=false;

    for (std::vector<_hub>::iterator it = hubs.hub.begin(); it != hubs.hub.end(); ++it) {
        if (std::strcmp(founded_hub->name, it->name) == 0)
        {
        found_hub=true;
        founded_hub->from_db = 1;
        founded_hub->db_id = it->db_id;
        break;
        }
    };

    if (found_hub==false){
        founded_hub->from_db = 0;
    };

};

bool MySQLHubs::AddHub()
{
    hubs.count++;
    hubs.hub.push_back(tmp_hub);
    // for while optimization
    if (tmp_hub.from_db==true) hubs.last_hub_from_db=hubs.count;

return true;
}

bool MySQLHubs::AddHub(_hub new_hub)
{
    hubs.count++;
    hubs.hub.push_back(new_hub);
return true;
}
