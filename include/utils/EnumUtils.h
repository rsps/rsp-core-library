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

} /* namespace rsp::utils */

#endif /* INCLUDE_UTILS_ENUMUTILS_H_ */
