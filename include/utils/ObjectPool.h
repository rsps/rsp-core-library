/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_UTILS_OBJECT_POOL_H
#define RSP_CORE_LIB_UTILS_OBJECT_POOL_H

#include <exceptions/CoreException.h>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <iostream>

namespace rsp::utils {

class EObjectPoolException: public exceptions::CoreException
{
public:
    explicit EObjectPoolException(const char *aMsg)
        : CoreException(aMsg)
    {
    }
};


template<class T>
class ObjectPool
{
public:
    ObjectPool(std::size_t aSize)
        : mPool(aSize),
          mAvailable(aSize),
          mUsed(aSize)
    {
        std::size_t i = 0;
        for (auto &v : mPool) {
            mAvailable.emplace(&v);
        }
    }

    T* Get()
    {
        if (mAvailable.size() == 0) {
            THROW_WITH_BACKTRACE1(EObjectPoolException, "ObjectPool is exhausted.");
        }

        T* result = *mAvailable.begin(); // mAvailable is never empty here...
        mAvailable.erase(result);
        mUsed.emplace(result);

        return result;
    }

    void Put(T** t)
    {
        mUsed.erase(*t);
        mAvailable.emplace(*t);
        *t = nullptr;
    }

    int Available() const
    {
        return mAvailable.size();
    }

private:
    std::vector<T> mPool;
    std::unordered_set<T*> mAvailable;
    std::unordered_set<T*> mUsed;
};

} // namespace rsp::utils

#endif // RSP_CORE_LIB_UTILS_OBJECT_POOL_H
