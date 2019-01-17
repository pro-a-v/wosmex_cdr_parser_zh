#include <iostream>
#include <semaphore.h>
#include <cstdio>
#include <fcntl.h>
#include <iostream>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <csignal>
#include <unistd.h>

#include "INIReader.h"
#include "FileLoader.h"
#include "MySQLData.h"
#include "MySQLOperators.h"
#include "MySQLHubs.h"

#include <glog/logging.h>
#include <glog/raw_logging.h>


std::string SemaphoreName;
sem_t *mysem;

void free_semaphore()
{
    sem_unlink(SemaphoreName.c_str());  
    sem_close(mysem);
}

void signalHandlerGoogle()  //inline void signalHandler(int signal, siginfo_t *siginfo, void *context)
{
    free_semaphore();
    exit(-1);
}

void signalHandler(int signal)  //inline void signalHandler(int signal, siginfo_t *siginfo, void *context)
{
    free_semaphore();
    exit(-1);
}

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}



int main(int argc, char **argv)
{
    char* cwd;
    cwd = getcwd( NULL, 255 );
    if( cwd == NULL ) 
    {
            std::cout << "Cannot get my working directory"  << std::endl;
            exit(-1);
    }
    std::string work_path, log_info, log_error;
    work_path = cwd;
    log_info = work_path + "/logs/log.INFO";
    log_error = work_path + "/logs/log.INFO";

    // Initialize Google's logging library.
     google::InitGoogleLogging(argv[0]);
     FLAGS_logtostderr = 1;
     google::SetLogDestination(google::INFO,log_info.c_str());
     google::SetLogDestination(google::WARNING,log_error.c_str());
     google::SetLogDestination(google::ERROR,log_error.c_str());

    if (argc!=2)
    {
        std::cout << "Specify filename to load as first argument ";
        return EXIT_FAILURE;
    }

    google::InstallFailureSignalHandler();
    //google::InstallFailureFunction(&signalHandlerGoogle);

    //registering catch of signal
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    SemaphoreName = ((char*)argv[1]);
    SemaphoreName = SemaphoreName.substr(SemaphoreName.find_last_of("\\/")+1, SemaphoreName.size()- SemaphoreName.find_last_of("\\/")-1);


    if ((mysem = sem_open(SemaphoreName.c_str(), O_CREAT|O_EXCL, 0644, 1)) == SEM_FAILED)
    {
    std::cout << "File now parsing by onother program (system answer): " <<  std::strerror(errno);
    return 0;
    }
                                            

    INIReader *reader; reader = new INIReader("config.ini");
    FileLoader *file_loader = new FileLoader();
    MySQLData *mysql_data = new MySQLData();
    MySQLOperators *CDRsOperators = new MySQLOperators();
    MySQLHubs *CDRsHubs = new MySQLHubs();
    MySQLOperStats *CDRsOperStats = new MySQLOperStats();


    if (reader->ParseError() < 0)
    {
        std::cout << "Can't load 'config.ini'";  free_semaphore();   return EXIT_FAILURE;
    }

    // Данные для проверки шапки/футера файла с СДР-ами
    file_loader->SetHeaderWord(reader->Get("global", "HeaderWord", "Header").c_str());
    file_loader->SetFooterWord(reader->Get("global", "FooterWord", "Footer").c_str());
    file_loader->SetIGNORE_SRC_OPERS(reader->Get("global", "IGNORE_SRC_OPERS", "").c_str());

    file_loader->SetFileName(argv[1]);

    // Данные для подключения к бд
    // Заполняет данные о операторах
    mysql_data->Connect(
              reader->Get("database", "IP", "localhost").c_str(),
              reader->Get("database", "USER", "root").c_str(),
              reader->Get("database", "PASSWORD", "").c_str(),
              reader->Get("database", "DATABASE", "mysql").c_str(),
              reader->GetInteger("database", "PORT", 3306)
              );

    if(mysql_data->isConnected()==false)            {    std::cout << "Can't connect to DB: "          << mysql_data->ErrorText;   free_semaphore();    return EXIT_FAILURE;   };
    // Check if file was loaded
    bool file_was_loaded;
    if(!mysql_data->FileWasProcessed(file_loader->FileNameShort,&file_was_loaded))   {    std::cout << "Can't fill operators from DB: " << mysql_data->ErrorText;   free_semaphore();    return EXIT_FAILURE;    };
    if(file_was_loaded)  {    std::cout << "File " << file_loader->FileNameShort << " was already been processed";   free_semaphore();    return 0;    };

    // load file into memory
    if (!file_loader->LoadFile())                     {     std::cout <<  file_loader->GetError();  free_semaphore();    return EXIT_FAILURE;  };
    if (file_loader->RecordsCount() >0)  { std::cout << "Usefull records in file = " << file_loader->RecordsCount() << std::endl;  }
    else {  std::cout << "No usefull records in file  ";  free_semaphore();    return 0; }

    // Get Data from DB
    if(!mysql_data->FillOperators(CDRsOperators))   {    std::cout << "Can't fill operators from DB: " << mysql_data->ErrorText;  free_semaphore();     return EXIT_FAILURE;    };
    // Hubs
    if(!mysql_data->FillHubs(CDRsHubs))             {    std::cout << "Can't fill hubs from DB: "      << mysql_data->ErrorText;  free_semaphore();     return EXIT_FAILURE;   };
    delete reader; // освобождаем память, весь конфиг мы уже считали





   //std::cout << "Strings: "  << file_loader->GetStringsCount()-1;
   google::FlushLogFiles(google::GLOG_ERROR);


   for ( unsigned int i=1; i < file_loader->GetStringsCount()-1; i++)
   {
       // form statistic data from string
       MySQLOperators::_operator src_file_oper;
       MySQLOperators::_operator dst_file_oper;
       MySQLHubs::_hub src_file_hub;
       MySQLHubs::_hub dst_file_hub;
       FileLoader::_cdr file_cdr;



        if (!file_loader->ConvertToLong(file_loader->GetField(i,FileLoader::STATUS),"file::string::field::status",&file_cdr.status)) { std::cout <<  file_loader->GetError(); free_semaphore();  return EXIT_FAILURE;  };
        if (!file_loader->ConvertToLong(file_loader->GetField(i,FileLoader::EXT_ERROR),"file::string::field::ext_error",&file_cdr.ext_error))         {  std::cout <<  file_loader->GetError(); free_semaphore();  return EXIT_FAILURE; };
        if (!file_loader->ConvertToLong(file_loader->GetField(i,FileLoader::EXT_CODE),"file::string::field::ext_err_type",&file_cdr.ext_err_type))    { std::cout <<  file_loader->GetError(); free_semaphore();  return EXIT_FAILURE; };
        if (!file_loader->ConvertToLong(file_loader->GetField(i,FileLoader::SRI_DIRECTION),"file::string::field::SRI_DIRECTION",&file_cdr.SRI_DIRECTION))    { std::cout <<  file_loader->GetError(); free_semaphore();  return EXIT_FAILURE; };
        if (!file_loader->ConvertToLong(file_loader->GetField(i,FileLoader::INT_ERROR),"file::string::field::INT_ERROR",&file_cdr.INT_ERROR))    { std::cout <<  file_loader->GetError(); free_semaphore();  return EXIT_FAILURE; };
        if (!file_loader->ConvertToLong(file_loader->GetField(i,FileLoader::MAP_ERROR),"file::string::field::INT_ERROR",&file_cdr.MAP_ERROR))    { std::cout <<  file_loader->GetError(); free_semaphore(); return EXIT_FAILURE; };
        if (!file_loader->ConvertToLong(file_loader->GetField(i,FileLoader::RETRIES),"file::string::field::RETRIES",&file_cdr.DLVR_ATTEMPTS))    { std::cout <<  file_loader->GetError(); free_semaphore(); return EXIT_FAILURE; };
        if (!file_loader->ConvertToLong(file_loader->GetField(i,FileLoader::CDR_TYPE),"file::string::field::CDR_TYPE",&file_cdr.CDR_TYPE))    { std::cout <<  file_loader->GetError(); free_semaphore(); return EXIT_FAILURE; };
        if (!file_loader->ConvertToLong(file_loader->GetField(i,FileLoader::BLOCKING_REASON),"file::string::field::BLOCKING_REASON",&file_cdr.BLOCKING_REASON))    { std::cout <<  file_loader->GetError(); free_semaphore(); return EXIT_FAILURE; };
        file_cdr.OA = file_loader->ConvertToOA_OD(file_loader->GetField(i,FileLoader::ORIG_ADDR));
        file_cdr.DA = file_loader->ConvertToOA_OD(file_loader->GetField(i,FileLoader::DESTN_ADDR));
        file_cdr.MESSAGE_ID = file_loader->GetField(i,FileLoader::MESSAGE_ID);
        file_loader->ConvertToDateTime(file_loader->GetField(i,FileLoader::TIME),&file_cdr.dt);

            // Check&Fill DST_OPER
            CDRsOperators->Check_Oper(file_loader->GetField(i,FileLoader::DST_OPER),  &dst_file_oper);
           if (dst_file_oper.from_db==false) {
               // Save operator into db
               if(!mysql_data->SaveOperator(&dst_file_oper)) { std::cout <<  mysql_data->ErrorText; free_semaphore();  return EXIT_FAILURE; };
               // Save operator into local vector
               CDRsOperators->AddOperator(dst_file_oper);
           };

            // Check&Fill SRC_OPER
            CDRsOperators->Check_Oper(file_loader->GetField(i,FileLoader::SRC_OPER),  &src_file_oper);
           if (src_file_oper.from_db==false) {
               // Save operator into db
               if(!mysql_data->SaveOperator(&src_file_oper)){ std::cout <<  mysql_data->ErrorText; free_semaphore(); return EXIT_FAILURE; };
               // Save operator into local vector
               CDRsOperators->AddOperator(src_file_oper);
           };

            // Check&Fill DST_HUB
            CDRsHubs->Check_Hub(file_loader->GetField(i,FileLoader::DST_HUB),&dst_file_hub);
            if(dst_file_hub.from_db==false) {
                // Save operator into db
                if(!mysql_data->SaveHub(&dst_file_hub)) { std::cout <<  mysql_data->ErrorText; free_semaphore();  return EXIT_FAILURE; };
                 // Save hub into local vector
                CDRsHubs->AddHub(dst_file_hub);
            };

            // Check&Fill SRC_HUB
            CDRsHubs->Check_Hub(file_loader->GetField(i,FileLoader::SRC_HUB),&src_file_hub);
            if(src_file_hub.from_db==false) {
                // Save operator into db
                if(!mysql_data->SaveHub(&src_file_hub)) { std::cout <<  mysql_data->ErrorText; free_semaphore(); return EXIT_FAILURE; };
                 // Save hub into local vector
                CDRsHubs->AddHub(src_file_hub);
            };
            //------------------------

            // Fill statistic field
            file_cdr.dst_oper_id=dst_file_oper.db_id;
            file_cdr.src_hub_id=src_file_hub.db_id;
            file_cdr.dst_hub_id=dst_file_hub.db_id;
            file_cdr.src_oper_id=src_file_oper.db_id;

            // Agregate dates found in file
            MySQLOperStats::_data tmp;
                tmp.year=file_cdr.dt.tm_year;
                tmp.mon=file_cdr.dt.tm_mon;
                tmp.day=file_cdr.dt.tm_mday;

            CDRsOperStats->AddDateInFile(tmp);

            // Add cdr to collection
            file_loader->AddFileCDR(file_cdr);
   }
   // Get Data from DB
   // Statistic
   if(!mysql_data->FillOperStats(CDRsOperStats))   {    std::cout << "Can't fill oper_stat from DB: " << mysql_data->ErrorText;   free_semaphore();    return EXIT_FAILURE;   };
   // --------------------

   // Update Statistic using data from file
   for (long i=0; i < file_loader->cdrs.size();i++)
   {
        if (file_loader->cdrs[i].CDR_TYPE==3)   // only cdr_type = 3 going to agregate statistic
        {
        MySQLOperStats::_oper_stat file_stat;

        file_stat.dst_hub_id  = file_loader->cdrs[i].dst_hub_id;
        file_stat.src_hub_id  = file_loader->cdrs[i].src_hub_id;
        file_stat.dst_oper_id = file_loader->cdrs[i].dst_oper_id;
        file_stat.src_oper_id = file_loader->cdrs[i].src_oper_id;
        file_stat.dt          = file_loader->cdrs[i].dt;
        file_stat.ext_error   = file_loader->cdrs[i].ext_error;
        file_stat.ext_err_type= file_loader->cdrs[i].ext_err_type;
        file_stat.status      = file_loader->cdrs[i].status;
        file_stat.from_db     = false;
        file_stat.cnt_diff    = 1;
        file_stat.db_id=-1;

        CDRsOperStats->AddFileOperStat(file_stat);
        };
   };
   // ===============================================================================
   // ===============================================================================
   // ===============================================================================
    std::cout << currentDateTime() << " inserts into DB" << std::endl;
    if(!mysql_data->StartTransaction())  {    std::cout <<  mysql_data->ErrorText << std::endl;;    free_semaphore();   return EXIT_FAILURE;   };
    std::cout << currentDateTime() << " inserts into DB - Trans File" << std::endl;    
    if(!mysql_data->InsertTransactionFile(file_loader->FileNameShort)) {    std::cout <<  mysql_data->ErrorText << std::endl;  mysql_data->RollbackTransaction();  std::cout << "ROLLBACK " << mysql_data->ErrorText << std::endl; free_semaphore();   return EXIT_FAILURE;   };
    std::cout << currentDateTime() << " inserts into DB - CDRs" << std::endl;    
    if(!mysql_data->InsertTransactionCDRs(file_loader->cdrs)) {    std::cout <<  mysql_data->ErrorText << std::endl;;   mysql_data->RollbackTransaction(); std::cout << "ROLLBACK " << mysql_data->ErrorText << std::endl; free_semaphore();   return EXIT_FAILURE;   };
    std::cout << currentDateTime() << " inserts into DB - OperStats" << std::endl;    
    if(!mysql_data->InsertTransactionOperStats(CDRsOperStats)) {    std::cout <<  mysql_data->ErrorText << std::endl;;   mysql_data->RollbackTransaction(); std::cout << "ROLLBACK " << mysql_data->ErrorText << std::endl; free_semaphore();   return EXIT_FAILURE;   };
    std::cout << currentDateTime() << " inserts into DB - Commit" << std::endl;    
    if(!mysql_data->CommitTransaction())  {    std::cout <<  mysql_data->ErrorText << std::endl;  mysql_data->RollbackTransaction();  std::cout << "ROLLBACK " << mysql_data->ErrorText << std::endl;  free_semaphore();  return EXIT_FAILURE;   };
    std::cout << currentDateTime() << " inserts into DB - end" << std::endl;
  //a->RemoveFile();
  //delete file_loader;   // Will removed after program exit
    free_semaphore();

return 0;
}

