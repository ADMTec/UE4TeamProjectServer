#include "ServerConstants.hpp"
#include "UE4DevelopmentLibrary/Exception.hpp"


void ServerConstants::Initialize()
{
    {
        TextFileLineReader reader;
        reader.Initialize("ServerSetting.ini");
        auto opt_odbc = reader.GetString("ODBC");
        auto opt_db_id = reader.GetString("DB_ID");
        auto opt_db_pw = reader.GetString("DB_PW");
        auto opt_port = reader.GetInt32("ZONE_SERVER_PORT");
        auto opt_max_conn = reader.GetInt32("ZONE_SERVER_MAX_CONNECTION");
        auto opt_worker_size = reader.GetInt32("ZONE_SERVER_NUM_IO_WORKER");
        auto inter_ip = reader.GetString("INTERMEDIATE_SERVER_IP");
        auto inter_port = reader.GetInt32("INTERMEDIATE_SERVER_PORT");
        if (!opt_odbc || !opt_db_id || !opt_db_pw || !opt_port || !opt_max_conn 
            || !opt_worker_size || !inter_ip || !inter_port)
        {
            throw StackTraceException(ExceptionType::kLogicError, "no data");
        }
#ifdef _UNICODE
        std::string str1 = std::string(*opt_odbc);
        std::string str2 = std::string(*opt_db_id);
        std::string str3 = std::string(*opt_db_pw);
        odbc_name = std::wstring(str1.begin(), str1.end());
        db_id = std::wstring(str2.begin(), str2.end());
        db_pw = std::wstring(str3.begin(), str3.end());
#else
        odbc_name = *opt_odbc;
        db_id = *opt_db_id;
        db_pw = *opt_db_pw;
#endif
        port = *opt_port;
        max_connection = *opt_max_conn;
        num_io_worker_ = *opt_worker_size;
        intermediate_server_ip = *inter_ip;
        intermediate_server_port = *inter_port;
    }
    TextFileLineReader reader;
    reader.Initialize("ServerConstant.txt");
}