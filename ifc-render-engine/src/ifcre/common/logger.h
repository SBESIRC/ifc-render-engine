#pragma once

#include <string>
#include <fstream>

namespace ifcre {

#define debug(format, ...) \
    Logger::instance()->log(Logger::DEBUG, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define info(format, ...) \
    Logger::instance()->log(Logger::INFO, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define warn(format, ...) \
    Logger::instance()->log(Logger::WARN, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define error(format, ...) \
    Logger::instance()->log(Logger::EERROR, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define fatal(format, ...) \
    Logger::instance()->log(Logger::FATAL, __FILE__, __LINE__, format, ##__VA_ARGS__)


    class Logger
    {
    public:
        enum Level
        {
            DEBUG = 0,
            INFO,
            WARN,
            EERROR,
            FATAL,
            LEVEL_COUNT
        };

        static Logger* instance();
        void open(const std::string& filename);
        void close();
        void log(Level level, const char* file, int line, const char* format, ...);
        void max(int bytes);
        void level(int level);

    private:
        Logger();
        ~Logger();
        void rotate();

    private:
        std::string m_filename;
        std::ofstream m_fout;
        int m_max;
        int m_len;
        int m_level;
        static const char* s_level[LEVEL_COUNT];
        static Logger* m_instance;
    };

}
