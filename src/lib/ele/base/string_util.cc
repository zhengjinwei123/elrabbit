#include "string_util.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstring>

namespace ele
{
    namespace string_util
    {

        void split(const char *str, const char *sep,
                   std::vector<std::string> *result, int max_split)
        {
            size_t sep_len = ::strlen(sep);
            if (0 == sep_len)
            {
                return;
            }

            int count = 0;
            const char *last = str;

            for (;;)
            {
                if (max_split > 0 && ++count >= max_split)
                {
                    result->push_back(std::string(last));
                    break;
                }

                const char *next = ::strstr(last, sep);
                if (nullptr == next)
                {
                    result->push_back(std::string(last));
                    break;
                }
                result->push_back(std::string(last, next - last));

                last = next + sep_len;
            }
        }

        void split(const char *str, size_t str_len, const char *sep,
                   std::vector<std::string> *result, int max_split)
        {
            size_t sep_len = ::strlen(sep);
            if (0 == sep_len)
            {
                return;
            }

            int count = 0;
            const char *end = str + str_len;
            const char *sep_end = sep + sep_len;
            const char *last = str;

            for (;;)
            {
                if (max_split > 0 && ++count >= max_split)
                {
                    result->push_back(std::string(last, end));
                }

                const char *next = std::search(last, end, sep, sep_end);
                if (next == end)
                {
                    result->push_back(std::string(last, end));
                    break;
                }

                result->push_back(std::string(last, next - last));
                last = next + sep_len;
            }
        }

        std::string ltrim(const std::string &str, const char *ws)
        {
            size_t start = str.find_first_not_of(ws);
            if (std::string::npos == start)
            {
                return "";
            }
            return str.substr(start);
        }

        std::string rtrim(const std::string &str, const char *ws)
        {
            size_t end = str.find_last_not_of(ws);
            if (std::string::npos == end)
            {
                return "";
            }
            return str.substr(0, end + 1);
        }

        std::string trim(const std::string &str, const char *ws)
        {
            return ltrim(rtrim(str, ws), ws);
        }

        std::string toUpper(const std::string &str)
        {
            std::string upper(str);
            std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
            return upper;
        }

        std::string toLower(const std::string &str)
        {
            std::string lower(str);
            std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
            return lower;
        }

        std::string replace(const std::string &str,
                            const std::string &search, const std::string &replace, int max_replace)
        {
            std::string ret(str);

            if (0 == max_replace)
            {
                return ret;
            }

            size_t pos = 0;
            int count = 0;

            for (;;)
            {
                pos = ret.find(search, pos);
                if (std::string::npos == pos) {
                    break;
                }
                ret.replace(pos, search.length(), replace);
                pos += replace.length();

                if (max_replace > 0 && ++count >= max_replace) {
                    break;
                }
            }

            return ret;
        }

        std::string toString(int i)
        {
            char buffer[32];
            memset(buffer, 0, sizeof(buffer));
            ::snprintf(buffer, sizeof(buffer), "%d", i);
            return std::string(buffer);
        }

        std::string toString(long l)
        {
            char buffer[64];
            memset(buffer, 0, sizeof(buffer));
            ::snprintf(buffer, sizeof(buffer), "%ld", l);
            return std::string(buffer);
        }

        std::string toString(long long ll)
        {
            char buffer[64];
            memset(buffer, 0, sizeof(buffer));
            ::snprintf(buffer, sizeof(buffer), "%lld", ll);
            return std::string(buffer);
        }

        std::string toString(unsigned ui)
        {
            char buffer[32];
            memset(buffer, 0, sizeof(buffer));
            ::snprintf(buffer, sizeof(buffer), "%u", ui);
            return std::string(buffer);
        }

        std::string toString(unsigned long ul)
        {
            char buffer[64];
            memset(buffer, 0, sizeof(buffer));
            ::snprintf(buffer, sizeof(buffer), "%lu", ul);
            return std::string(buffer);
        }

        std::string toString(unsigned long long ull)
        {
            char buffer[64];
            memset(buffer, 0, sizeof(buffer));
            ::snprintf(buffer, sizeof(buffer), "%llu", ull);
            return std::string(buffer);
        }

        const char *find(const char *str, size_t str_len, const char *keyword)
        {
            size_t keyword_len = ::strlen(keyword);
            if (0 == keyword_len) {
                return nullptr;
            }

            const char *ret = std::search(str, str + str_len, keyword, keyword + keyword_len);
            if (ret == str + str_len) {
                return nullptr;
            }
            return ret;
        }

        bool stricmp(const std::string &lhs, const std::string &rhs)
        {
            if (lhs.size() != rhs.size()) {
                return false;
            }

            size_t lhs_size = lhs.size();
            for (size_t i = 0; i < lhs_size; ++i) {
                if (::tolower(lhs[i]) != ::tolower(rhs[i])) {
                    return false;
                }
            }
            return true;
        }

    } // namespace string_util
} // namespace ele