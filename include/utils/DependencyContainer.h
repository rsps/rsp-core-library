/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_UTILS_DEPENDENCYCONTAINER_H_
#define INCLUDE_UTILS_DEPENDENCYCONTAINER_H_

#include <utils/Function.h>

class DC
{
    template<typename T>
    DC& registerFactory(rsp::utils::Function<T(void)> aFactory));
};

typedef DC DependencyContainer;

#endif /* INCLUDE_UTILS_DEPENDENCYCONTAINER_H_ */
