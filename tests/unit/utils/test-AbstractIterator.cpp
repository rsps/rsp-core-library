/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/

#include "doctest.h"
#include <algorithm>
#include <utils/AbstractIterator.h>
#include <vector>

using namespace rsp::utils;

struct IElement
{
    virtual ~IElement() = default;
    [[nodiscard]] virtual int GetValue() const = 0;
    virtual void SetValue(int aValue) = 0;
};

struct IContainer
{
    using Iterator = AbstractIterator<IElement>;
    using ConstIterator = AbstractIterator<const IElement>;
    static_assert(std::forward_iterator<Iterator>);
    static_assert(std::forward_iterator<ConstIterator>);

    virtual ~IContainer() = default;
    virtual Iterator begin() = 0;
    virtual Iterator end() = 0;
    virtual ConstIterator cbegin() = 0;
    virtual ConstIterator cend() = 0;
    virtual size_t size() = 0;
};

class Element : public IElement
{
public:
    explicit Element(int aValue) noexcept : mValue(aValue) {}

    [[nodiscard]] int GetValue() const override
    {
        return mValue;
    }
    void SetValue(int aValue) override
    {
        mValue = aValue;
    }
protected:
    int mValue;
};

class Container : public IContainer
{
public:
    Container()
    {
        for (int i = 0; i < 5 ; i++) {
            mVector.emplace_back(i);
        }
    }

    Iterator begin() override
    {
        return {mVector, true};
    }

    Iterator end() override
    {
        return {mVector, false};
    }

    ConstIterator cbegin() override
    {
        return {mVector, true};
    }

    ConstIterator cend() override
    {
        return {mVector, false};
    }

    size_t size() override
    {
        return mVector.size();
    }

protected:
    std::vector<Element> mVector{};
};


TEST_CASE("AbstractIterator")
{
    CHECK_NOTHROW(Container dummy);

    Container container;
    IContainer &ref_container = container;

    SUBCASE("for-loop") {
        int expected = 0;
        size_t count = 0;

        for (auto &e: ref_container) {
            CHECK_EQ(e.GetValue(), expected++);
            count++;
        }
        CHECK_EQ(count, container.size());
    }

    SUBCASE("find") {
        auto cit = std::find_if(container.cbegin(), container.cend(), [](const IElement &e) {
            return e.GetValue() == 4;
        });
        CHECK_NE(cit, container.cend());
//        cit->SetValue(5); // This does not compile
        cit++;
        CHECK_EQ(cit, container.cend());

        auto it = std::find_if(container.begin(), container.end(), [](const IElement &e) {
            return e.GetValue() == 4;
        });
        CHECK_NE(it, container.end());
        it->SetValue(5);
        it++;
        CHECK_EQ(it, container.end());

        cit = std::find_if(container.cbegin(), container.cend(), [](const IElement &e) {
            return e.GetValue() == 4;
        });
        CHECK_EQ(cit, container.cend());

        cit = std::find_if(container.cbegin(), container.cend(), [](const IElement &e) {
            return e.GetValue() == 5;
        });
        CHECK_NE(cit, container.cend());
    }

}
