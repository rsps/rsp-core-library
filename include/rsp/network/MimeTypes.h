/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef RSP_CORE_LIB_INCLUDE_NETWORK_MIMETYPES_H
#define RSP_CORE_LIB_INCLUDE_NETWORK_MIMETYPES_H

#include <map>
#include <string>
#include <string_view>
#include <rsp/utils/string_view_ci.h>

namespace rsp::network {

class MimeTypes
{
public:
    static std::string_view GetType(std::string_view aExtension);
    static std::string GetExtension(std::string_view aMimeType);

private:
    using List_t = utils::string_view_map_ci;
    static List_t mTypeMap;
};

} // rsp::network

#endif //RSP_CORE_LIB_INCLUDE_NETWORK_MIMETYPES_H
