#include "FileLoader.h"


FileLoader::FileLoader()
{

}

FileLoader::~FileLoader()
{

}

bool FileLoader::LoadFile()
{

    // std::ifstream ifile(Filename.c_str(),std::ios_base::binary);
    // const std::locale empty_locale;
    // ifile.imbue(std::locale(empty_locale, new std::codecvt_utf8<wchar_t>));

    std::ifstream ifile(Filename.c_str(),std::ios_base::binary);
    ifile.imbue(std::locale("en_US.UTF-8"));


     if (ifile.is_open())
     {
         std::string line;

             while (getline(ifile, line))
             {
                 std::vector <std::string> raw_string_parts;
                 split(raw_string_parts, line, boost::is_any_of("|"));
                 std::for_each(raw_string_parts.begin(), raw_string_parts.end(), boost::bind(&boost::trim<std::string>, _1, std::locale() ));
                 // ===================================================
                 
                 // header / footer
                 if (raw_string_parts.size()<2)
                 raw_fields.push_back(raw_string_parts);
                 
                 if (raw_string_parts.size()>8)
                 if (IGNORE_SRC_OPERS.size()>0)
                 if (std::find(IGNORE_SRC_OPERS.begin(), IGNORE_SRC_OPERS.end(), raw_string_parts[SRC_OPER]) != IGNORE_SRC_OPERS.end())
                 {
                   // Element in vector ignore.
                 }
                 else
                 {
                     raw_fields.push_back(raw_string_parts);
                 }

            }

     ifile.close();

    }
    else
    {
        ErrorText = "Coudn't open file "+Filename+" with error: " + strerror(errno);
        return false;

    }

     if (!HasHeader())
     {
         ErrorText = "Coudn't find Header in file " + Filename;
         return false;
     };

     if (!HasFooter())
     {
         ErrorText = "Coudn't find Header in file " + Filename;
         return false;
     };

return true;
}

bool FileLoader::RemoveFile()
{

  if (unlink(Filename.c_str())!=0)
  {
      char *err_text;
      err_text = (char*)malloc(4096);
      explain_message_unlink(err_text, 4096, Filename.c_str());
      ErrorText = err_text;
      free(err_text);
      return false;
  };

return true;
}

bool FileLoader::SetFileName(std::string filename)
{

        Filename = filename;
        FileNameShort = Filename;
        FileNameShort = FileNameShort.substr(FileNameShort.find_last_of("\\/")+1,FileNameShort.size()-FileNameShort.find_last_of("\\/")-1);


return true;
}



bool FileLoader::SetHeaderWord(const char *headername)
{

    HeaderWorld = headername;

return true;
}

bool FileLoader::SetFooterWord(const char *footername)
{
    FooterWorld = footername;

    return true;
}

bool FileLoader::SetIGNORE_SRC_OPERS(const char *data)
{
  std::string line;
  line = data;
  split(IGNORE_SRC_OPERS, line, boost::is_any_of(" "));
  std::for_each(IGNORE_SRC_OPERS.begin(), IGNORE_SRC_OPERS.end(), boost::bind(&boost::trim<std::string>, _1, std::locale() ));
}



const char *FileLoader::GetError()
{
    return ErrorText.c_str();
}


unsigned long FileLoader::GetStringsCount()
{
    return raw_fields.size();
};

bool FileLoader::HasFooter()
{
    if (raw_fields.empty())
     {
         ErrorText = "Footer World not found in last string";
         return false;
     }

    if (raw_fields.back()[0].find(FooterWorld)!= std::string::npos)
        return true;

    ErrorText = "Footer World not found in first string";
    return false;
};

bool FileLoader::HasHeader()
{
    if (raw_fields.empty())
     {
         ErrorText = "Header World not found in last string";
         return false;
     }

    if (raw_fields.front()[0].find(HeaderWorld)!= std::string::npos)
        return true;

    ErrorText = "Header World not found in first string";
    return false;
};


bool FileLoader::ConvertToLong(std::string curr_value,std::string field_name, long *result)
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
           ErrorText = "field "+field_name+" have non numeric sumbols value("+curr_value+") ";
           return false;
        }
    };
    long tmp = atol(curr_value.c_str());
    memcpy(result,&tmp,sizeof(long));

    if (tmp < 0)
    {
        ErrorText = "field " + field_name + " with value "+ curr_value +" cannot be converted to long ";
        return false;
    }

return true;
}

std::string FileLoader::ConvertToOA_OD(std::string curr_value)
{
    std::string result;

    if(curr_value[0]!='+') result="+";
    
    std::replace( curr_value.begin(), curr_value.end(), '\'', ' ');
    result = result + curr_value;
    
    result = result.substr(0,20);

return result;
}

bool FileLoader::ConvertToMessageId(std::string curr_value, char *result)
{
    char tmp[8]; memset(tmp,0x00,8);

    int length;

    if (curr_value.length()>=8)  length=8;
    else length= curr_value.length();

    strncpy(tmp,curr_value.c_str(),length);
    memcpy(result,tmp,8);

return true;
};


bool FileLoader::ConvertToDateTime(std::string curr_value, struct tm *result)
{
// 081015120230
    if (curr_value.length()!=12)
    {
        ErrorText = "field CDR_DATETIME have size != 12    value("+curr_value+") ";
        return false;
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
           ErrorText = "field CDR_DATETIME have non numeric sumbols value("+ curr_value +") ";
           return false;
        }
    };

    struct tm tmp;
    // 081015120230 08-10-15 12:02:30
     tmp.tm_mday = atoi(curr_value.substr(0,2).c_str());
     tmp.tm_mon = atoi(curr_value.substr(2,2).c_str());
     tmp.tm_year = 2000 + atoi(curr_value.substr(4,2).c_str());
     tmp.tm_hour = atoi(curr_value.substr(6,2).c_str());
     tmp.tm_min = atoi(curr_value.substr(8,2).c_str());
     tmp.tm_sec = atoi(curr_value.substr(10,2).c_str());

     memcpy(result,&tmp,sizeof(struct tm));
return true;
}


std::string FileLoader::GetField(unsigned int line, FIELD_NAME field_pos)
{
 return raw_fields[line][field_pos];
}

void FileLoader::AddFileCDR(_cdr new_cdr)
{
    cdrs.push_back(new_cdr);
}


unsigned int FileLoader::RecordsCount()
{
    return raw_fields.size();
}
