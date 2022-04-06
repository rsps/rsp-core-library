/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#include <doctest.h>
#include <graphics/controls/TouchArea.h>
#include <graphics/controls/Image.h>
#include <utils/StopWatch.h>
#include <stdexcept>
#include <sstream>

using namespace rsp::graphics;

/*
 * Good examples to use during interviews.
 *
 * \see https://www.toptal.com/c-plus-plus/interview-questions
 */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wparentheses"

/**
 * \brief Get average of the two arguments
 * \param integer a
 * \param integer a
 * \return The average of the two arguments with double precision
 */
static double Average(int a, int b)
{
    return (a + b) / 2.0;
}

/**
 * \brief Test if value if within given limits
 * \param low low limit
 * \param high high limit
 * \param value Value to test
 * \return bool
 */
static bool InRange(int low, int high, int value)
{
    if (low < value < high) {
        return true;
    }
    return false;
}

/**
 * \brief IsDerivedFrom
 */
template<typename D, typename B>
struct IsDerivedFromHelper
{
    class No { };
    class Yes { No no[3]; };

    static Yes Test( B* );
    static No Test( ... );

    enum { Is = sizeof(Test(static_cast<D*>(0))) == sizeof(Yes) };
};

template <class C, class P>
static bool IsDerivedFrom() {
    return IsDerivedFromHelper<C, P>::Is;
}

/**
 * \brief IsSameClass
 */
template <typename T, typename U>
struct is_same
{
    static const bool value = false;
};

template <typename T>
struct is_same<T, T>
{
    static const bool value = true;
};

template <class A, class B>
static bool IsSameClass() {
    return is_same<A, B>::value;
}


/**
 * \brief F
 */
class F
{
public:
    static void Simple(int *a, int*b, int n)
    {
        for (int i = 0 ; i < n ; ++i) {
            int product = 1;
            for (int j = 0 ; j < n ; ++j) {
                if (i != j) {
                    product *= a[j];
                }
            }
            b[i] = product;
        }
    }

    static void Complex(int* a, int* b, int n)
    {
        int product = 1;

        for (int i = 0; i < n; ++i) {
            // For element "i" set b[i] to a[0] * ... * a[i - 1]
            b[i] = product;
            // Multiply with a[i] to set product to a[0] * ... * a[i]
            product *= a[i];
        }

        product = 1;

        for (int i = n - 1; i >= 0; --i) {
            // For element "i" multiply b[i] with a[i + 1] * ... * a[n - 1]
            b[i] *= product;
            // Multiply with a[i] to set product to a[i] * ... * a[n - 1]
            product *= a[i];
        }
    }

};


static int UglyArrayIndexing(int *a, int n)
{
    if (n < 5) {
        throw std::runtime_error("To few array elements");
    }
    return (1 + 3)[a] - a[0] + (a + 1)[2];
}

/**
 * \brief private virtual method override
 * \see http://www.gotw.ca/publications/mill18.htm
 */
class Base
{
    virtual void method() {sout << "from Base" << std::endl;}
public:
    virtual ~Base() {method();}
    void baseMethod() {method();}

    static std::stringstream sout;
};
std::stringstream Base::sout{};

class A : public Base
{
    void method() {sout << "from A" << std::endl;}
public:
    ~A() {method();}
};


class ConstMutable
{
public:
    void Allowed(int a) const { mA = a; }
    operator int() const { return mA; }

protected:
    mutable int mA;
};


TEST_CASE("Examples")
{
    rsp::utils::StopWatch sw;

    CHECK_EQ(Average(1, 2), 1.5);

    CHECK_EQ(25u - 50, 4294967271);

    CHECK_FALSE(IsDerivedFrom<TouchArea, Control>());
    CHECK(IsDerivedFrom<Image, Control>());

    CHECK(IsSameClass<Control, Control>());
    CHECK_FALSE(IsSameClass<Control, TouchArea>());

    int a[] = {2, 1, 5, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    int b[sizeof(a) / sizeof(int)];

    sw.Reset();
    for (int i = 0 ; i < 100000 ; ++i) {
        F::Simple(a, b, sizeof(a) / sizeof(int));
    }
    MESSAGE("Timed to " << sw.Elapsed<std::chrono::milliseconds>() << "ms");
    CHECK_EQ(b[0], 45);
    CHECK_EQ(b[1], 90);
    CHECK_EQ(b[2], 18);
    CHECK_EQ(b[3], 10);

    sw.Reset();
    for (int i = 0 ; i < 100000 ; ++i) {
        F::Complex(a, b, sizeof(a) / sizeof(int));
    }
    MESSAGE("Timed to " << sw.Elapsed<std::chrono::milliseconds>() << "ms");
    CHECK_EQ(b[0], 45);
    CHECK_EQ(b[1], 90);
    CHECK_EQ(b[2], 18);
    CHECK_EQ(b[3], 10);

    int arr[] = {1, 2, 3, 4, 5, 6, 7};
    CHECK_EQ(UglyArrayIndexing(arr, sizeof(arr) / sizeof(int)), 8);

    Base* base = new A;
    base->baseMethod();
    delete base;
    CHECK_EQ(Base::sout.str(), "from A\nfrom A\nfrom Base\n");

    unsigned char half_limit = 150;
    for (unsigned char i = 0; i < 2 * half_limit; ++i) {
        // Trick loop: 2 * half_limit = 300, because 2 is type int.
//        MESSAGE(static_cast<int>(i));
        if (static_cast<int>(i) == 255) {
            break;
        }
    }

    ConstMutable cm;
    cm.Allowed(3);
    CHECK(cm == 3);

    CHECK(InRange(3, 5, 6));
}

#pragma GCC diagnostic pop


