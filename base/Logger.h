//
// Created by clay on 3/6/23.
//

#ifndef ATRI_LOGGER_H
#define ATRI_LOGGER_H

#include <bits/types/struct_timeval.h>
#include <cstdio>
#include <ctime>
#include <string>
#include <sys/time.h>

class Logger {
public:
    enum LogLevel {
        TRACE, DEBUG, INFO, WARN, ERROR, FATAL
    };
    static constexpr char tag_hash[][8] = {
        "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
    };
    Logger(LogLevel level, const char *file, int line, const std::string &info) {
        if(level < log_level) return;
        std::string s;
        for(auto i : info) {
            switch (i) {
                case '\n':
                    s.append("\\n");
                    break;
                case '\t':
                    s.append("\\t");
                    break;
                case '\r':
                    s.append("\\r");
                    break;
                case '\0':
                    s.append("\\0");
                    break;
                default:
                    s.push_back(i);
            }
        }
        time_t t;
        timeval tv;
        gettimeofday(&tv, NULL);
        t = tv.tv_sec;
    
        char fmt[] = "%Y%m%e %H:%M:%S.%%dZ";
        char time_s[128], time_r[128];
        strftime(time_s , sizeof(time_s), fmt, gmtime(&t));
        sprintf(time_r, time_s, tv.tv_usec);
        printf("[%s] [%s] [%s:%d]  %s\n", tag_hash[level], time_r, file, line, s.c_str());
        if(level == FATAL)
            abort();
    }
    static void setLogLevel(LogLevel level) { log_level = level; }
private:
    static LogLevel log_level;
};
#define TRACE_LOG(trace_msg) Logger(Logger::TRACE, __FILE__, __LINE__, trace_msg)
#define DEBUG_LOG(debug_msg) Logger(Logger::DEBUG, __FILE__, __LINE__, debug_msg)
#define INFO_LOG(info_msg) Logger(Logger::INFO, __FILE__, __LINE__, info_msg)
#define WARN_LOG(warn_msg) Logger(Logger::WARN, __FILE__, __LINE__, warn_msg)
#define ERROR_LOG(error_msg) Logger(Logger::ERROR, __FILE__, __LINE__, error_msg)
#define FATAL_LOG(fatal_msg) Logger(Logger::FATAL, __FILE__, __LINE__, fatal_msg)

#endif //ATRI_LOGGER_H
