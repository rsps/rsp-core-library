/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_UTILS_ENUMUTILS_H_
#define INCLUDE_UTILS_ENUMUTILS_H_

namespace rsp::utils {

template <int real, int expected, int item_size>
void checksize() {
    static_assert(real == expected);
};

template <int S, class E, class I = const char*, class = std::enable_if_t<std::is_enum_v<E>, E>>
void assert_enum_list()
{
    checksize<static_cast<int>(S / sizeof(I)), static_cast<unsigned int>(E::__END__), sizeof(I)>();
}


/**
 * Heavily inspired by this article:
 * \see https://www.scs.stanford.edu/~dm/blog/va-opt.html
 */
#define PARENS ()

// Rescan macro tokens 256 times
#define EXPAND(arg) EXPAND1(EXPAND1(EXPAND1(EXPAND1(arg))))
#define EXPAND1(arg) EXPAND2(EXPAND2(EXPAND2(EXPAND2(arg))))
#define EXPAND2(arg) EXPAND3(EXPAND3(EXPAND3(EXPAND3(arg))))
#define EXPAND3(arg) EXPAND4(EXPAND4(EXPAND4(EXPAND4(arg))))
#define EXPAND4(arg) arg

#define FOR_EACH(macro, a1, ...)                                    \
  __VA_OPT__(EXPAND(FOR_EACH_HELPER(macro, a1, __VA_ARGS__)))

#define FOR_EACH_HELPER(macro, a1, a2, ...)                     \
  macro(a1, a2)                                                 \
  __VA_OPT__(FOR_EACH_AGAIN PARENS (macro, a1, __VA_ARGS__))

#define FOR_EACH_AGAIN() FOR_EACH_HELPER

#define ENUM_CASE(type, name) case type::name: return #name;
#define ENUM_LIST(type, name) type::name,

#define _CAT(a, b, c, d) a ## b ## c ## d

#define MAKE_ENUM(type, storage, ...)              \
enum class type storage {                          \
    __VA_ARGS__                                    \
};                                                 \
                                                   \
constexpr const char* to_cstring(type _e)          \
{                                                  \
    switch (_e) {                                  \
    FOR_EACH(ENUM_CASE, type, __VA_ARGS__)         \
    default:                                       \
        return "Not in enum " #type;               \
    }                                              \
}                                                  \
                                                   \
const type _CAT(type, _ALL,,)[] = { FOR_EACH(ENUM_LIST, type, __VA_ARGS__) };

/**
 * Usage:
 *   MAKE_ENUM(MyType, ZERO, ONE, TWO, THREE);
 *
 * Generates:
 *   enum class MyType {
 *       ZERO, ONE, TWO, THREE
 *   };
 *
 *   constexpr const char* to_cstring(MyType _e)
 *   {
 *       switch (_e) {
 *       case MyType::ZERO: return "ZERO";
 *       case MyType::ONE: return "ONE";
 *       case MyType::TWO: return "TWO";
 *       case MyType::THREE: return "THREE";
 *       default:
 *           return "Not in enum MyType";
 *       }
 *   }
 *
 *   const MyType MyType_ALL[] = {MyType::ZERO,MyType::ONE,MyType::TWO,MyType::THREE};
 */


// constexpr static char arrayStr[] = "a,b,c";

constexpr std::size_t count(char const * str, char sep)
{
    std::size_t  ret { 1u };

    while ( *str ) {
      if ( sep == *str++ ) {
          ++ret;
      }
    }

    return ret;
}


} /* namespace rsp::utils */

#endif /* INCLUDE_UTILS_ENUMUTILS_H_ */
