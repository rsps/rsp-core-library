/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef INCLUDE_SECURITY_CRYPTBASE_H_
#define INCLUDE_SECURITY_CRYPTBASE_H_

#include <string_view>
#include <vector>

namespace rsp::security {

class CryptBase
{
public:
    virtual ~CryptBase() {}
    virtual void Init(std::string_view aIvSeed, std::string_view aSecret) = 0;
    virtual void Update(const std::uint8_t *apData, std::size_t aSize) = 0;
    virtual std::vector<std::uint8_t> Finalize() = 0;
};

} // namespace rsp::security

#endif /* INCLUDE_SECURITY_CRYPTBASE_H_ */
