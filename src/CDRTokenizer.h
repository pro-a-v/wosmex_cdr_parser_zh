#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <iostream>
#include <string>
#include <vector>
#include <cstddef>
#include <cstdio>
#include "definitions.h"
using namespace std;

class CDRTokenizer
{
public:
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
        char MESSAGE_ID[9];
        char OA[20];
        char DA[20];
        long  ext_error;
        long ext_err_type;
        long SRI_DIRECTION;
        long MAP_ERROR;
        long BLOCKING_REASON;
        long INT_ERROR;
    };
   vector <_cdr> cdrs;


   CDRTokenizer();
   ~CDRTokenizer();
   bool Split(string s);
   bool Split(char *s);
   string GetField(FIELD_NAME);
   char ErrorText[TEXT_ERROR_MAX_SIZE];
   void AddFileCDR(_cdr new_cdr);
   _cdr GetCDRbyNumber(long number);
   long GetCDRCount();

private:


    vector <string> fields;


};

#endif // TOKENIZER_H

