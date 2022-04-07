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

static std::stringstream sout{};

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
 * \brief Test if value is within given limits
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
};

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

class Book{
public:
    Book(){
        sout << "Constructor" << std::endl;
    }

    //Overloaded new operator
    void* operator new(size_t size){
        sout << "Overloaded new operator" << std::endl;
        return malloc(size);
    }
    //overloaded delete operator
    void operator delete(void* ptr){
        sout << "Overloaded delete operator" << std::endl;
        free(ptr);
    }
    ~Book(){
        sout << "Destructor" << std::endl;
    }
};

class Employee
{
    int mId;
    std::string mName{};
public:
    //Empty Constructor
    Employee()
        : mId(0),
          mName("default")
    {
    }

    //Overloaded constructor with int parameter
    Employee(int aId)
        : mId(aId)
    {
    }

    //Overloaded constructor with a int parameter and a string
    Employee(int aId, const std::string &arName)
        : mId(aId),
          mName(arName)
    {
    }

    void display()
    {
        sout << mId << " " << mName << std::endl;
    }
};

TEST_CASE("Examples")
{
    sout.clear();

    SUBCASE("Operator precedence") {
        CHECK_EQ(Average(1, 2), 1.5);
        CHECK_EQ(25u - 50, 4294967271);
    }

    SUBCASE("IsDerivedFrom template") {
        CHECK_FALSE(IsDerivedFrom<TouchArea, Control>());
        CHECK(IsDerivedFrom<Image, Control>());
    }

    SUBCASE("IsSame template") {
        CHECK(IsSameClass<Control, Control>());
        CHECK_FALSE(IsSameClass<Control, TouchArea>());
    }

    SUBCASE("Array products") {
        rsp::utils::StopWatch sw;
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
    }

    SUBCASE("Ugly Indexing") {
        int arr[] = {1, 2, 3, 4, 5, 6, 7};
        CHECK_EQ(UglyArrayIndexing(arr, sizeof(arr) / sizeof(int)), 8);
    }

    SUBCASE("Polymorph method calling") {
        Base* base = new A;
        base->baseMethod();
        delete base;
        CHECK_EQ(sout.str(), "from A\nfrom A\nfrom Base\n");
    }

    SUBCASE("Unsigned char overrun trick question") {
        unsigned char half_limit = 150;
        for (unsigned char i = 0; i < 2 * half_limit; ++i) {
            // Trick loop: 2 * half_limit = 300, because 2 is type int.
            if (static_cast<int>(i) == 255) {
                break;
            }
        }
    }

    SUBCASE("Change member from const method") {
        ConstMutable cm;
        cm.Allowed(3);
        CHECK(cm == 3);
    }

    SUBCASE("Value is in range") {
        CHECK(InRange(3, 5, 6));
    }

    SUBCASE("Constructor / New operator precedence") {
        auto *book = new Book();
        delete book;
        MESSAGE(sout);
        CHECK_EQ(sout.str(), "Overloaded new operator\nConstructor\nDestructor\nOverloaded delete operator\n");
    }

    SUBCASE("Constructor overloading") {
        Employee e1;
        e1.display();

        Employee e2(123);
        e2.display();

        Employee e3(123,"John");
        e3.display();

        CHECK_EQ(sout.str(), "0 default\n123 \n123 John\n");
    }

}

#pragma GCC diagnostic pop


