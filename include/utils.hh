#ifndef utils_hh
#define utils_hh

#include <cstdint>
#include <string>

using namespace std;

namespace utils {
uint16_t ntohs(uint16_t val);
uint32_t ntohl(uint32_t val);
unsigned int str_to_bin(string &str);
}

#endif /* utils_hh */
