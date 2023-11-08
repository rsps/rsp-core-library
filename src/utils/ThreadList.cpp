/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#include <exceptions/CoreException.h>
#include <utils/ThreadList.h>

namespace rsp::utils {

ThreadList::ThreadList()
{
    SetInstance(this);
}

ThreadList::~ThreadList()
{
    if (&GetInstance() == this) {
        SetInstance(nullptr);
    }
}


ThreadList& ThreadList::AddThread(ThreadInterface& arThread)
{
    auto const &pair = mMap.insert_or_assign(arThread.GetName(), arThread);
    mLogger.Info() << (pair.second ? "Inserted: " : "Assigned: ") << arThread.GetName();
    return *this;
}

ThreadList& ThreadList::RemoveThread(ThreadInterface &arThread)
{
    mMap.erase(arThread.GetName());
    return *this;
}

ThreadInterface& ThreadList::GetThreadByName(std::string_view aName)
try
{
    return mMap.at(aName);
}
catch (const std::out_of_range &e) {
    THROW_WITH_BACKTRACE1(exceptions::NotSetException, std::string("A thread with name '") + std::string(aName) + "' has not been registered.");
}

std::vector<std::string_view> ThreadList::GetThreadNames() const
{
    std::vector<std::string_view> result;
    for (auto &item : mMap) {
        result.emplace_back(item.first);
    }
    return result;
}

} // namespace rsp::utils
