/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_UTILS_DEPENDENCY_CONTAINER_H
#define RSP_CORE_LIB_UTILS_DEPENDENCY_CONTAINER_H

#include <functional>

class DependencyContainer;
typedef DependencyContainer DC; // Alias
typedef DependencyContainer DI; // Alias

class DependencyContainer
{
    template<typename T>
    DC& registerFactory(std::function<T(void)> aFactory)
    {
        return *this;
    }
};


#endif // RSP_CORE_LIB_UTILS_DEPENDENCY_CONTAINER_H
