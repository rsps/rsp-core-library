/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#include <ranges>
#include <rsp/exceptions/CoreException.h>
#include <rsp/utils/ThreadList.h>
#include <iostream>

namespace rsp::utils {

ThreadList::ThreadList()
{
    SetInstance(this);
}

ThreadList::~ThreadList()
try {
    if (&GetInstance() == this) {
        SetInstance(nullptr);
    }
}
catch (...) {
    std::cerr << "Exception in ~ThreadList()" << std::endl;
}

ThreadList& ThreadList::AddThread(ThreadInterface& arThread)
{
    const auto & [fst, snd] = mMap.insert_or_assign(arThread.GetName(), arThread);
    mLogger.Info() << (snd ? "Inserted: " : "Assigned: ") << arThread.GetName();
    return *this;
}

ThreadList& ThreadList::RemoveThread(const ThreadInterface &arThread)
{
    mMap.erase(arThread.GetName());
    return *this;
}

ThreadInterface& ThreadList::GetThreadByName(const std::string_view aName) const
try
{
    return mMap.at(aName).get();
}
catch (const std::out_of_range &) {
    THROW_WITH_BACKTRACE1(exceptions::NotSetException, std::string("A thread with name '") + std::string(aName) + "' has not been registered.");
}

std::vector<std::string_view> ThreadList::GetThreadNames() const
{
    std::vector<std::string_view> result;
    for (const auto& key: mMap | std::views::keys) {
        result.emplace_back(key);
    }
    return result;
}

} // namespace rsp::utils
