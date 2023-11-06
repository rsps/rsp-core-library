/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef RSP_CORE_LIB_INCLUDE_UTILS_THREAD_LIST_H
#define RSP_CORE_LIB_INCLUDE_UTILS_THREAD_LIST_H

#include <logging/LogChannel.h>
#include <map>
#include <string>
#include <string_view>
#include "Singleton.h"
#include "ThreadInterface.h"
#include <vector>

namespace rsp::utils {

class ThreadList : public Singleton<ThreadList>, logging::NamedLogger<ThreadList>
{
public:
    ThreadList();
    ~ThreadList() override;

    ThreadList(ThreadList &&arOther) = default;
    ThreadList& operator=(ThreadList &&arOther) = default;

    ThreadList(const ThreadList &arOther) = delete;
    ThreadList& operator=(const ThreadList &arOther) = delete;

    ThreadList& AddThread(ThreadInterface& arThread);
    ThreadList& RemoveThread(ThreadInterface& arThread);

    ThreadInterface& GetThreadByName(std::string_view aName);

    [[nodiscard]] std::vector<std::string_view> GetThreadNames() const;

protected:
    std::map<std::string_view, ThreadInterface&> mMap{};
};

} // namespace rsp::utils

#endif //RSP_CORE_LIB_INCLUDE_UTILS_THREAD_LIST_H
