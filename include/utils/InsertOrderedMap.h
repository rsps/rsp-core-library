/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef INCLUDE_UTILS_INSERTORDEREDMAP_H_
#define INCLUDE_UTILS_INSERTORDEREDMAP_H_

#include <map>
#include <vector>
#include <functional>

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
 *     std::cout << map.at[k] << std::endl;
 * }
 * \endcode
 *
 * \tparam _Key
 * \tparam _Tp
 * \tparam _Compare
 * \tparam _Alloc
 */
template <typename _Key, typename _Tp, typename _Compare = std::less<_Key>,
          typename _Alloc = std::allocator<std::pair<const _Key, _Tp> > >
class InsertOrderedMap
{
public:

    /**
     * Construct an empty container.
     */
    InsertOrderedMap() {}

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
    InsertOrderedMap(std::initializer_list<std::pair<const _Key, _Tp>> aList)
        : mMap(aList)
    {
        for(auto key : mMap) {
            mInsertionOrder.push_back(key);
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
    _Tp& operator[](const _Key aKey)
    {
        auto ret = mMap.try_emplace(aKey);
        if (ret.second) { // Insertion happened
            mInsertionOrder.push_back(ret.first->first);
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
    _Tp& at(const _Key aKey)
    {
        return mMap.at(aKey);
    }

    const _Tp& at(const _Key aKey) const
    {
        return mMap.at(aKey);
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
    std::size_t size() const
    {
        return mMap.size();
    }

    InsertOrderedMap& Remove(const _Key aKey)
    {
        auto it = mMap.find(aKey);
        if (it != mMap.end()) {
            mMap.erase(it);

            mInsertionOrder.erase(std::find_if(mInsertionOrder.begin(), mInsertionOrder.end(),
                [&](const std::reference_wrapper<const _Key> &el)
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
    const std::vector<std::reference_wrapper<const _Key>>& GetOrderList() const { return mInsertionOrder; }

    /**
     * Get access to internal map with key/values.
     *
     * \return const reference to std::map.
     */
    const std::map<_Key, _Tp>& GetMap() const { return mMap; }

protected:
    std::map<_Key, _Tp, _Compare, _Alloc> mMap{};
    std::vector<std::reference_wrapper<const _Key>> mInsertionOrder{};
};

} // rsp::utils

#endif /* INCLUDE_UTILS_INSERTORDEREDMAP_H_ */
