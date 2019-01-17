#ifndef FILELOADER_H
#define FILELOADER_H

#include <iostream>
#include "definitions.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <cassert>
#include <errno.h>
#include <vector>
#include <libexplain/unlink.h>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/bind.hpp>


#include <iostream> // std::cin, std::cout
#include <fstream> // std::ifstream
#include <locale>


class FileLoader
{
public:
    FileLoader();
    ~FileLoader();

    enum FIELD_NAME
    {

        TIME = 0,
        CDR_TYPE = 1,
        SRI_DIRECTION = 2,
        DST_HUB = 3,
        SRC_HUB = 4,
        DST_OPER = 5,
        SRC_OPER = 8,
        DESTN_ADDR = 12,
        ORIG_ADDR = 13,
        MESSAGE_ID = 14,
        INT_ERROR = 16,
        EXT_CODE = 17,
        EXT_ERROR = 18,
        RETRIES = 19,
        BLOCKING_REASON = 20,
        STATUS = 21,
        MAP_ERROR = 23
    };

    struct _cdr
    {
        long CDR_TYPE;
        long src_oper_id;
        long dst_oper_id;
        long src_hub_id;
        long dst_hub_id;
        long DLVR_ATTEMPTS;
        struct tm dt;
        long  status;
        std::string MESSAGE_ID;
        std::string OA;
        std::string DA;
        long  ext_error;
        long ext_err_type;
        long SRI_DIRECTION;
        long MAP_ERROR;
        long BLOCKING_REASON;
        long INT_ERROR;
    };
    std::vector <_cdr> cdrs;

    bool SetFileName(std::string filename);
    bool SetHeaderWord(const char *); // for INIReader
    bool SetFooterWord(const char *); // for INIReader
    bool SetIGNORE_SRC_OPERS(const char *); // for INIReader
    unsigned int RecordsCount();


    bool LoadFile();
    bool RemoveFile();
    const char *GetError();
    unsigned long GetStringsCount();


    std::string GetField(unsigned int line, FIELD_NAME field_pos);
    void AddFileCDR(_cdr new_cdr);

    bool ConvertToLong(std::string curr_value,std::string field_name, long *result);
    std::string ConvertToOA_OD(std::string curr_value);
    bool ConvertToMessageId(std::string curr_value, char *result);
    bool ConvertToDateTime(std::string curr_value, struct tm *dt);

    std::string FileNameShort;

private:

    std::string ErrorText;
    std::string Filename;
    std::string FooterWorld;
    std::string HeaderWorld;
    std::vector < std::vector <std::string> > raw_fields;
    std::vector <std::string> IGNORE_SRC_OPERS;


    bool HasFooter();
    bool HasHeader();


};

#endif // FILELOADER_H
