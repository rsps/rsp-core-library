/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <logging/SetLevel.h>
#include <logging/OutStreamBuffer.h>

namespace rsp::logging {

std::ostream& SetLevel::operator ()(std::ostream &o) const
{
    auto *stream = dynamic_cast<OutStreamBuffer*>(o.rdbuf());

    if (stream) {
        stream->Lock();
        DEBUG("Locked by " << std::this_thread::get_id())
        stream->SetLevel(mValue);
    }
    else {
        auto *ls = dynamic_cast<LogStream*>(&o);
        if (ls) {
            ls->SetLevel(mValue);
        }
    }

    return o;
}

std::ostream& operator<<(std::ostream &o, SetLevel aLevel)
{
    return aLevel(o);
}

} /* namespace rsp::logging */
