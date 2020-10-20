#ifndef ELE_BASE_STRING_UTIL_H
#define ELE_BASE_STRING_UTIL_H

#include <cstddef>
#include <string>
#include <vector>

namespace ele
{
    namespace string_util
    {

        void split(const char *str, const char *sep,
                   std::vector<std::string> *result, int max_split = -1);

        void split(const char *str, size_t str_len, const char *sep,
                   std::vector<std::string> *result, int max_split = -1);

        std::string ltrim(const std::string &str, const char *ws = " \t");
        std::string rtrim(const std::string &str, const char *ws = " \t");
        std::string trim(const std::string &str, const char *ws = " \t");

        std::string toUpper(const std::string &str);
        std::string toLower(const std::string &str);

        std::string replace(const std::string &str,
                            const std::string &search, const std::string &replace, int max_replace = -1);

        std::string toString(int i);
        std::string toString(long l);
        std::string toString(long long ll);
        std::string toString(unsigned ui);
        std::string toString(unsigned long ul);
        std::string toString(unsigned long long ull);

        const char *find(const char *str, size_t str_len, const char *keyword);
        bool stricmp(const std::string &lhs, const std::string &rhs);

    } // namespace string_util
} // namespace ele

#endif