/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef RSP_CORE_LIB_INCLUDE_NETWORK_STRING_VIEW_CI_H
#define RSP_CORE_LIB_INCLUDE_NETWORK_STRING_VIEW_CI_H

#include <cctype>
#include <map>
#include <string_view>

namespace rsp::utils {

namespace detail {

/**
 * \brief Trait class for case insensitive comparison
 * \see https://stackoverflow.com/questions/11635/case-insensitive-string-comparison-in-c
 */
struct ci_char_traits : public std::char_traits<char>
{
    static bool eq(char c1, char c2)
    {
        return toupper(c1) == toupper(c2);
    }

    static bool ne(char c1, char c2)
    {
        return toupper(c1) != toupper(c2);
    }

    static bool lt(char c1, char c2)
    {
        return toupper(c1) < toupper(c2);
    }

    static int compare(const char* s1, const char* s2, size_t n)
    {
        while (n-- != 0) {
            auto a = toupper(*s1);
            auto b = toupper(*s2);
            if (a < b)
                return -1;
            if (a > b)
                return 1;
            ++s1;
            ++s2;
        }
        return 0;
    }

    static const char* find(const char* s, int n, char a)
    {
        auto a_upper = toupper(a);
        while (n-- > 0 && toupper(*s) != a_upper) {
            ++s;
        }
        return s;
    }
};

}   // namespace detail


using string_view_ci = std::basic_string_view<std::string_view::value_type, detail::ci_char_traits>;

/**
 * \brief Case insensitive std::less comparator for map<string,*> types
 */
struct CaseInsensitiveComparator
{
    bool operator()(std::string_view a, std::string_view b) const noexcept
    {
        string_view_ci ci_view1{a.data(), a.size()};
        string_view_ci ci_view2{b.data(), b.size()};
        return ci_view1 < ci_view2;
    }
};

using string_view_map_ci = std::map<std::string_view, std::string_view, CaseInsensitiveComparator>;

} // namespace rsp::utils
#endif //RSP_CORE_LIB_INCLUDE_NETWORK_STRING_VIEW_CI_H
