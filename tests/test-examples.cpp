///*!
// * This Source Code Form is subject to the terms of the Mozilla Public
// * License, v. 2.0. If a copy of the MPL was not distributed with this
// * file, You can obtain one at https://mozilla.org/MPL/2.0/.
// *
// * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
// * \license     Mozilla Public License 2.0
// * \author      Steffen Brummer
// */
//#include <doctest.h>
//#include <graphics/controls/TouchArea.h>
//#include <graphics/controls/Image.h>
//#include <utils/StopWatch.h>
//#include <stdexcept>
//#include <sstream>
//#include <vector>
//
//using namespace rsp::graphics;
//
///*
// * Good examples to use during interviews.
// *
// * \see https://www.toptal.com/c-plus-plus/interview-questions
// */
//
//#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wparentheses"
//#pragma GCC diagnostic ignored "-Wcatch-value"
//
//static std::stringstream sout{};
//
///**
// * \brief Get average of the two arguments
// * \param integer a
// * \param integer a
// * \return The average of the two arguments with double precision
// */
//static double Average(int a, int b)
//{
//    return (a + b) / 2.0;
//}
//
///**
// * \brief Test if value is within given limits
// * \param low low limit
// * \param high high limit
// * \param value Value to test
// * \return bool
// */
//static bool InRange(int low, int high, int value)
//{
//    if (low < value < high) {
//        return true;
//    }
//    return false;
//}
//
///**
// * \brief IsDerivedFrom
// */
//template<typename D, typename B>
//struct IsDerivedFromHelper
//{
//    class No { };
//    class Yes { No no[3]; };
//
//    static Yes Test( B* );
//    static No Test( ... );
//
//    enum { Is = sizeof(Test(static_cast<D*>(0))) == sizeof(Yes) };
//};
//
//template <class C, class P>
//static bool IsDerivedFrom() {
//    return IsDerivedFromHelper<C, P>::Is;
//}
//
///**
// * \brief IsSameClass
// */
//template <typename T, typename U>
//struct is_same
//{
//    static const bool value = false;
//};
//
//template <typename T>
//struct is_same<T, T>
//{
//    static const bool value = true;
//};
//
//template <class A, class B>
//static bool IsSameClass() {
//    return is_same<A, B>::value;
//}
//
//
///**
// * \brief F
// */
//class F
//{
//public:
//    static void Simple(int *a, int*b, int n)
//    {
//        for (int i = 0 ; i < n ; ++i) {
//            int product = 1;
//            for (int j = 0 ; j < n ; ++j) {
//                if (i != j) {
//                    product *= a[j];
//                }
//            }
//            b[i] = product;
//        }
//    }
//
//    static void Complex(int* a, int* b, int n)
//    {
//        int product = 1;
//
//        for (int i = 0; i < n; ++i) {
//            // For element "i" set b[i] to a[0] * ... * a[i - 1]
//            b[i] = product;
//            // Multiply with a[i] to set product to a[0] * ... * a[i]
//            product *= a[i];
//        }
//
//        product = 1;
//
//        for (int i = n - 1; i >= 0; --i) {
//            // For element "i" multiply b[i] with a[i + 1] * ... * a[n - 1]
//            b[i] *= product;
//            // Multiply with a[i] to set product to a[i] * ... * a[n - 1]
//            product *= a[i];
//        }
//    }
//
//};
//
//
//static int UglyArrayIndexing(int *a, int n)
//{
//    if (n < 5) {
//        throw std::runtime_error("To few array elements");
//    }
//    return (1 + 3)[a] - a[0] + (a + 1)[2];
//}
//
///**
// * \brief private virtual method override
// * \see http://www.gotw.ca/publications/mill18.htm
// */
//class Base
//{
//    virtual void method() {sout << "from Base" << std::endl;}
//public:
//    virtual ~Base() {method();}
//    void baseMethod() {method();}
//};
//
//class A : public Base
//{
//    void method() {sout << "from A" << std::endl;}
//public:
//    ~A() {method();}
//};
//
//
//class ConstMutable
//{
//public:
//    void Allowed(int a) const { mA = a; }
//    operator int() const { return mA; }
//
//protected:
//    mutable int mA;
//};
//
//class Book{
//public:
//    Book(){
//        sout << "Constructor" << std::endl;
//    }
//
//    //Overloaded new operator
//    void* operator new(size_t size){
//        sout << "Overloaded new operator" << std::endl;
//        return malloc(size);
//    }
//    //overloaded delete operator
//    void operator delete(void* ptr){
//        sout << "Overloaded delete operator" << std::endl;
//        free(ptr);
//    }
//    ~Book(){
//        sout << "Destructor" << std::endl;
//    }
//};
//
//class Employee
//{
//    int mId;
//    std::string mName{};
//public:
//    //Empty Constructor
//    Employee()
//        : mId(0),
//          mName("default")
//    {
//    }
//
//    //Overloaded constructor with int parameter
//    Employee(int aId)
//        : mId(aId)
//    {
//    }
//
//    //Overloaded constructor with a int parameter and a string
//    Employee(int aId, const std::string &arName)
//        : mId(aId),
//          mName(arName)
//    {
//    }
//
//    void display()
//    {
//        sout << mId << " " << mName << std::endl;
//    }
//};
//
//class Car {
//public:
//    Car(const std::string arModel) : mModel(arModel) {
//        sout << "Constructor\n";
//    }
//
//    Car(const Car &ob){
//        sout << "Copy Constructor\n";
//    }
//
//    Car(const Car &&ob){
//        sout << "Move Constructor\n";
//    }
//
//    Car& operator=(const Car &arOther){
//        sout << "Copy Assignment\n";
//        if (this != &arOther) {
//            mModel = arOther.mModel;
//        }
//        return *this;
//    }
//
//    Car& operator=(Car &&arOther){
//        sout << "Move Assignment\n";
//        if (this != &arOther) {
//            mModel = std::move(arOther.mModel);
//        }
//        return *this;
//    }
//
//    Car function(Car ob){
//        //do something...
//        return ob;
//    }
//
//    static Car Forward(Car a){
//        return a;
//    }
//
//protected:
//    std::string mModel{};
//};
//
//class Throws {
//public:
//    static void Integer() {
//        throw 42;
//    }
//    static void String() {
//        throw "42";
//    }
//    static void StdString() {
//        throw std::string("42");
//    }
//    static void Exception() {
//        throw std::runtime_error("42");
//    }
//};
//
//class MyValue {
//public:
//    MyValue() : mValue(0) {}
//    MyValue(int aValue) : mValue(aValue) {}
//
//    bool operator<(const MyValue &arOther) const {
//        return mValue < arOther.mValue;
//    }
//
//    bool operator<(int aValue) const {
//        return mValue < aValue;
//    }
//
//    operator int() const { return mValue; }
//
//protected:
//    int mValue;
//};
//
//std::ostream& operator<<(std::ostream& os, const MyValue& a);
//
//std::ostream& operator<<(std::ostream& os, const MyValue& a) {
//    return os << int(a);
//}
//
//TEST_CASE("Examples")
//{
//    sout.str("");
//
//    SUBCASE("Operator precedence") {
//        CHECK_EQ(Average(1, 2), 1.5);
//        CHECK_EQ(25u - 50, 4294967271);
//    }
//
//    SUBCASE("IsDerivedFrom template") {
//        CHECK_FALSE(IsDerivedFrom<TouchArea, Control>());
//        CHECK(IsDerivedFrom<Image, Control>());
//    }
//
//    SUBCASE("IsSame template") {
//        CHECK(IsSameClass<Control, Control>());
//        CHECK_FALSE(IsSameClass<Control, TouchArea>());
//    }
//
//    SUBCASE("Array products") {
//        rsp::utils::StopWatch sw;
//        int a[] = {2, 1, 5, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
//        int b[sizeof(a) / sizeof(int)];
//
//        sw.Reset();
//        for (int i = 0 ; i < 100000 ; ++i) {
//            F::Simple(a, b, sizeof(a) / sizeof(int));
//        }
//        MESSAGE("Timed to " << sw.Elapsed<std::chrono::milliseconds>() << "ms");
//        CHECK_EQ(b[0], 45);
//        CHECK_EQ(b[1], 90);
//        CHECK_EQ(b[2], 18);
//        CHECK_EQ(b[3], 10);
//
//        sw.Reset();
//        for (int i = 0 ; i < 100000 ; ++i) {
//            F::Complex(a, b, sizeof(a) / sizeof(int));
//        }
//        MESSAGE("Timed to " << sw.Elapsed<std::chrono::milliseconds>() << "ms");
//        CHECK_EQ(b[0], 45);
//        CHECK_EQ(b[1], 90);
//        CHECK_EQ(b[2], 18);
//        CHECK_EQ(b[3], 10);
//    }
//
//    SUBCASE("Ugly Indexing") {
//        int arr[] = {1, 2, 3, 4, 5, 6, 7};
//        CHECK_EQ(UglyArrayIndexing(arr, sizeof(arr) / sizeof(int)), 8);
//    }
//
//    SUBCASE("Polymorph method calling") {
//        Base* base = new A;
//        base->baseMethod();
//        delete base;
//        CHECK_EQ(sout.str(), "from A\nfrom A\nfrom Base\n");
//    }
//
//    SUBCASE("Unsigned char overrun trick question") {
//        unsigned char half_limit = 150;
//        for (unsigned char i = 0; i < 2 * half_limit; ++i) {
//            // Trick loop: 2 * half_limit = 300, because 2 is type int.
//            if (static_cast<int>(i) == 255) {
//                break;
//            }
//        }
//    }
//
//    SUBCASE("Change member from const method") {
//        ConstMutable cm;
//        cm.Allowed(3);
//        CHECK(cm == 3);
//    }
//
//    SUBCASE("Value is in range") {
//        CHECK(InRange(3, 5, 6));
//    }
//
//    SUBCASE("Constructor / New operator precedence") {
//        auto *book = new Book();
//        delete book;
//        CHECK_EQ(sout.str(), "Overloaded new operator\nConstructor\nDestructor\nOverloaded delete operator\n");
//    }
//
//    SUBCASE("Constructor overloading") {
//        Employee e1;
//        e1.display();
//
//        Employee e2(123);
//        e2.display();
//
//        Employee e3(123,"John");
//        e3.display();
//
//        CHECK_EQ(sout.str(), "0 default\n123 \n123 John\n");
//    }
//
//    SUBCASE("Move constructor") {
//        Car obj1 = Car::Forward(Car("BMW")); // move-assignment from rvalue temporary
//        Car obj2 = obj1.function(obj1);
//
//        CHECK_EQ(sout.str(), "Constructor\n"
//            "Move Constructor\n"
//            "Copy Constructor\n"
//            "Move Constructor\n");
//    }
//
//    SUBCASE("Move constructor vector") {
//        std::vector<Car> cars;
//        cars.reserve(2);
//        cars.push_back(Car("FIAT"));
//        cars.emplace_back("VW");
//
//        CHECK_EQ(sout.str(), "Constructor\n"
//            "Move Constructor\n"
//            "Constructor\n");
//    }
//
//    SUBCASE("Move assignment") {
//        Car o1("Hyundai");
//        Car o2("Tata");
//        o1 = o2;
//        o2 = Car("Toyota");
//
//        CHECK_EQ(sout.str(), "Constructor\n"
//            "Constructor\n"
//            "Copy Assignment\n"
//            "Constructor\n"
//            "Move Assignment\n");
//    }
//
//    SUBCASE("Exceptions") {
//        CHECK_THROWS_AS(Throws::Integer(), const int&);
//        CHECK_THROWS_AS(Throws::String(), const char*);
//        CHECK_THROWS_AS(Throws::StdString(), const std::string&);
//        CHECK_THROWS_AS(Throws::Exception(), const std::exception&);
//
//        try {
//            throw std::runtime_error("42");
//        }
//        catch(std::runtime_error e) {
//            CHECK_EQ(std::string(e.what()), "42");
//        }
//        try {
//            throw std::runtime_error("42");
//        }
//        catch(const std::exception &e) {
//            CHECK_EQ(std::string(e.what()), "42");
//        }
//    }
//
//    SUBCASE("Operators") {
//        MyValue a;
//        MyValue b(42);
//        sout << b << a;
//
//        CHECK(0 == a);
//        CHECK(42 == b);
//        CHECK(a < b);
//        CHECK_FALSE(b < a);
//        CHECK_FALSE(b < 42);
//        CHECK(b < 43);
//        CHECK(b > 5);
//        CHECK_EQ(sout.str(), "420");
//    }
//}
//
//#pragma GCC diagnostic pop
//
//
