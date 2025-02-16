#ifndef trace_hh
#define trace_hh

#include <cstdarg>

#define ANSI_COLOR_RED		"\x1b[31m"
#define ANSI_COLOR_GREEN	"\x1b[32m"
#define ANSI_COLOR_YELLOW	"\x1b[33m"
#define ANSI_COLOR_BLUE		"\x1b[34m"
#define ANSI_COLOR_MAGENTA	"\x1b[35m"
#define ANSI_COLOR_CYAN		"\x1b[36m"
#define ANSI_COLOR_WHITE	"\x1b[37m"
#define ANSI_COLOR_RESET	"\x1b[0m"
#define ANSI_BOLD		"\x1b[1m"

namespace trace {
void log(const char *color, const char *format, va_list args);
void log_ansi(const char *code, const char *format, ...);
void log_imp(const char *format, ...);
void log_info(const char *format, ...);
void log_wrn(const char *format, ...);
void log_dbg(const char *format, ...);
void log_err(const char *format, ...);
void log_buffer(const unsigned char *data, int size);
};

#endif /* trace_hh */
