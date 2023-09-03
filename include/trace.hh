#ifndef trace_hh
#define trace_hh

#include <cstdarg>

namespace trace {
void log(const char *color, const char *format, va_list args);
void log_imp(const char *format, ...);
void log_info(const char *format, ...);
void log_wrn(const char *format, ...);
void log_dbg(const char *format, ...);
void log_err(const char *format, ...);
};

#endif /* trace_hh */
