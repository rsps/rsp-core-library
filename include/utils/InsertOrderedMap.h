/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef RSP_CORE_LIB_UTILS_INSERT_ORDERED_MAP_H
#define RSP_CORE_LIB_UTILS_INSERT_ORDERED_MAP_H

#include <exceptions/CoreException.h>
#include <algorithm>
#include <map>
#include <vector>
#include <functional>
#include <sstream>

namespace rsp::utils {

/**
 * \class InsertOrderedMap
 * \brief Aggregate class that wraps a std::map with key, value pairs, and a vector that maintains the insertion order.
 *
 * Currently it has reduced insertion possibilities to only using the [] operator.
 * There is also only const references to internal STL classes.
 * It should be enough to insert, update and search the map, and get content in the order of insertion by traversing the vector:
 *
 * Some member functions is on purpose kept in snake_case style, to mimic known std library functions with similar names.
 *
 * \code
 * InsertOrderedMap<int, string> map{...};
 *
 * for(auto k : map.GetOrderedList()) {
 *     std::cout << map.at(k.get()) << std::endl;
 * }
 * \endcode
 *
 * \tparam Key_T
 * \tparam Tp_T
 * \tparam Compare_T
 * \tparam Alloc_T
 */
template <typename Key_T, typename Tp_T, typename Compare_T = std::less<Key_T>,
          typename Alloc_T = std::allocator<std::pair<const Key_T, Tp_T> > >
class InsertOrderedMap
{
public:

    /**
     * Construct an empty container.
     */
    InsertOrderedMap() = default;

    /**
     * Default copy constructors
     */
    InsertOrderedMap(const InsertOrderedMap&) = default;
    InsertOrderedMap(InsertOrderedMap&&) = default;

    /**
     * Construct a container and populate it from initializer list.
     *
     * \param aList Initializer list with elements.
     */
    InsertOrderedMap(std::initializer_list<std::pair<const Key_T, Tp_T>> aList)
        : mMap(aList)
    {
        for(const auto &key : aList) {
            auto ret = mMap.find(key.first);
            mInsertionOrder.push_back(std::ref(ret->first));
        }
    }

    /**
     * Default assignment operators
     */
    InsertOrderedMap& operator=(const InsertOrderedMap&) = default;
    InsertOrderedMap& operator=(InsertOrderedMap&&) = default;

    /**
     * Random access operator, also used to add new elements.
     *
     * \param aKey Key to lookup.
     * \return Reference to value
     */
    Tp_T& operator[](const Key_T aKey)
    {
        auto ret = mMap.try_emplace(aKey);
        if (ret.second) { // Insertion happened
            mInsertionOrder.push_back(std::ref(ret.first->first));
        }
        return ret.first->second;
    }

    /**
     * Get reference to value or throw.
     *
     * \param aKey Key to lookup
     * \return Reference to value
     * \throw std::out_of_range if aKey does not exist
     */
    Tp_T& at(Key_T aKey)
    {
        Tp_T* result;
        try {
            result = &mMap.at(aKey);
        }
        catch(const std::exception &e) {
            std::stringstream ss;
            ss << "Failed to lookup key (" << aKey << "): " << e.what();
            THROW_WITH_BACKTRACE1(exceptions::CoreException, ss.str());
        }
        return *result;
    }

    const Tp_T& at(const Key_T aKey) const
    {
        const Tp_T* result;
        try {
            result = &mMap.at(aKey);
        }
        catch(const std::exception &e) {
            std::stringstream ss;
            ss << "Failed to lookup key (" << aKey << "): " << e.what();
            THROW_WITH_BACKTRACE1(exceptions::CoreException, ss.str());
        }
        return *result;
    }

    /**
     * Clear both internal containers.
     *
     * \return Reference to this
     */
    InsertOrderedMap& clear()
    {
        mInsertionOrder.clear();
        mMap.clear();
        return *this;
    }

    /**
     * Get the number of elements in the map.
     *
     * \return Number of elements.
     */
    [[nodiscard]] std::size_t size() const
    {
        return mMap.size();
    }

    /**
     * Remove a single element from the map.
     *
     * \param aKey Key to the element to remove.
     * \return Reference to this
     */
    InsertOrderedMap& Remove(const Key_T aKey)
    {
        auto it = mMap.find(aKey);
        if (it != mMap.end()) {
            mMap.erase(it);

            mInsertionOrder.erase(std::find_if(mInsertionOrder.begin(), mInsertionOrder.end(),
                [&](const std::reference_wrapper<const Key_T> &el)
                {
                    return el.get() == aKey;
                })
            );
        }

        return *this;

    }

    /**
     * Get access to the internal vector with keys in insertion order.
     *
     * \return const reference to std::vector
     */
    const std::vector<std::reference_wrapper<const Key_T>>& GetOrderList() const { return mInsertionOrder; }

    /**
     * Get access to internal map with key/values.
     *
     * \return const reference to std::map.
     */
    const std::map<Key_T, Tp_T>& GetMap() const { return mMap; }

protected:
    std::map<Key_T, Tp_T, Compare_T, Alloc_T> mMap{};
    std::vector<std::reference_wrapper<const Key_T>> mInsertionOrder{};
};

} // rsp::utils

#endif // RSP_CORE_LIB_UTILS_INSERT_ORDERED_MAP_H
