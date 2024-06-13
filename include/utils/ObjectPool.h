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


/**
 * Simple double linked list of nodes with given element type.
 * \tparam T Default constructible type
 */
template<class T>
class ObjectPool
{
public:
    explicit ObjectPool(size_t aSize)
    {
        mPool.resize(aSize);
        for (auto &v : mPool) {
            if (!mpAvailable) {
                mpAvailable = &v;
            }
            else {
                mpAvailable->mpNext = &v;
                mpAvailable->mpNext->mpPrevious = mpAvailable;
                mpAvailable = mpAvailable->mpNext;
            }
        }
    }

    ObjectPool(const ObjectPool &arOther) = default;
    ObjectPool(ObjectPool &&arOther) = default;
    ObjectPool& operator=(const ObjectPool &arOther) = default;
    ObjectPool& operator=(ObjectPool &&arOther) = default;


    /**
     * Get the next available element from the pool.
     * \return Reference to element
     */
    T& Get()
    {
        if (!mpAvailable) {
            THROW_WITH_BACKTRACE1(EObjectPoolException, "ObjectPool is exhausted.");
        }

        Node* result = mpAvailable;
        result->DetachFrom(mpAvailable);
        result->PushTo(mpUsed);

        return result->mElement;
    }

    /**
     * Return the element to the pool.
     * \param arElement Reference to element
     */
    void Put(T& arElement)
    {
        if (!mpUsed) {
            THROW_WITH_BACKTRACE1(EObjectPoolException, "Element does not belong to ObjectPool.");
        }
        auto *node = reinterpret_cast<NodePtr_t>(&arElement);
        node->DetachFrom(mpUsed);
        node->PushTo(mpAvailable);
    }

    [[nodiscard]] size_t Available() const
    {
        if (mpAvailable) {
            return mpAvailable->GetIndex() + 1u;
        }
        return 0;
    }

private:
    struct Node;
    using NodePtr_t = Node*;
    struct Node {
        T mElement{};
        NodePtr_t mpPrevious = nullptr;
        NodePtr_t mpNext = nullptr;

        Node() noexcept = default;
        Node(const Node &arOther) = default;
        Node(Node &&arOther) = default;
        Node& operator=(const Node &arOther) = default;
        Node& operator=(Node &&arOther) = default;

        void DetachFrom(NodePtr_t& arList)
        {
            if (arList == this) {
                arList = mpPrevious;
            }
            if (!mpNext) { // Last in list
                if (mpPrevious) { // Not alone in list
                    mpPrevious->mpNext = mpNext;
                }
            }
            else if (mpPrevious) { // Mid in list
                mpPrevious->mpNext = mpNext;
                mpNext->mpPrevious = mpPrevious;
            }
            else { // First in list
                mpNext->mpPrevious = nullptr;
            }
            mpNext = nullptr;
            mpPrevious = nullptr;
        }

        /**
         * Add node to end of list.
         * \param arList
         */
        void PushTo(NodePtr_t& arList)
        {
            if (arList) { // Current end of list should point to this
                arList->mpNext = this;
                mpPrevious = arList;
            }
            arList = this; // Set new end of list
        }

        size_t GetIndex() {
            size_t result = 0;
            auto p = this;
            while (p->mpPrevious) {
                result++;
                p = p->mpPrevious;
            }
            return result;
        }
    };

    std::vector<Node> mPool{};
    NodePtr_t mpAvailable = nullptr; // Pointer to last element in available list
    NodePtr_t mpUsed = nullptr;      // Pointer to last element in used list
};

} // namespace rsp::utils

#endif // RSP_CORE_LIB_UTILS_OBJECT_POOL_H
