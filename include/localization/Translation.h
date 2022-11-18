/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_LOCALIZATION_TRANSLATION_H_
#define INCLUDE_LOCALIZATION_TRANSLATION_H_

#include <localization/ITranslation.h>

namespace rsp::localization {

class Translation: public ITranslation
{
public:
    Translation() {};
    Translation(const char *apLocale) { SetLocale(apLocale); }

    std::string_view Translate(uint32_t aHash, std::string_view aDefault) const override;
};


} /* namespace rsp::localization */

#endif /* INCLUDE_LOCALIZATION_TRANSLATION_H_ */
