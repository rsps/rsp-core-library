/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_LOCALIZATION_ITRANSLATION_H_
#define INCLUDE_LOCALIZATION_ITRANSLATION_H_

#include <locale>
#include <memory>
#include <string_view>
#include <utils/Crc32.h>

namespace rsp::localization {

/**
 * \brief Interface class for translating text based on current std::locale.
 */
class ITranslation
{
public:
    virtual ~ITranslation() {};

    virtual ITranslation& SetLocale(const char *apLocale) { std::locale::global(std::locale(apLocale)); return *this; }

    virtual std::string_view Translate(std::uint32_t aHash, std::string_view aDefault) const = 0;

    constexpr std::string_view operator()(const char *apText) const {
        return Translate(rsp::utils::crc32::HashConst(apText), apText);
    }
};

} /* rsp::localization */

#endif /* INCLUDE_LOCALIZATION_ITRANSLATION_H_ */
