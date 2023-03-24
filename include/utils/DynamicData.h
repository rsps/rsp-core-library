/*!
 * \copyright   Copyright (c) 2023 RSP Systems A/S. All rights reserved.
 * \author      steffen
 */

#ifndef INCLUDE_UTILS_DYNAMICDATA_H_
#define INCLUDE_UTILS_DYNAMICDATA_H_

#include <exceptions/CoreException.h>
#include <vector>
#include <string>
#include <string_view>
#include "Variant.h"

namespace rsp::utils {

/**
 * \class EDynamicDataException
 * \brief Base class for all exceptions thrown by the DynamicData module.
 */
class EDynamicDataException : public exceptions::CoreException {
public:
    explicit EDynamicDataException(const std::string &aMsg) : CoreException(aMsg) {}
};

class EMemberNotExisting: public EDynamicDataException {
public:
    explicit EMemberNotExisting(const std::string &arName) : EDynamicDataException("Object does not have a member named: " + arName) {}
};

class EDynamicTypeError : public EDynamicDataException {
public:
    explicit EDynamicTypeError(const std::string &aMsg) : EDynamicDataException("Json Type Error: " + aMsg) {}
};

#define OPTIONAL(a) try { a; } catch(...) {}

/**
 * \class DynamicData
 * \brief Dynamic container for all sorts of data: Can hold any type as well as arrays and objects.
 *
 * Custom types needs to be added as Variant::Pointer elements, so they will have external ownership and are
 * not streamable.
 *
 */
class DynamicData: public Variant
{
public:
    /**
     * \interface Decoder
     * \brief Interface to use by DynamicData decoders
     */
    struct Decoder
    {
        virtual ~Decoder() {};
        virtual DynamicData Decode(const std::string &arStream) = 0;
    };

    /**
     * \interface Decoder
     * \brief Interface to use by DynamicData decoders
     */
    struct Encoder
    {
        virtual ~Encoder() {};
        virtual std::string Encode(const DynamicData &arData) = 0;
    };

    /**
     * \interface Serializable
     * \brief Interface to use by data objects that are able to convert to/from DynamicData
     */
    class Serializable
    {
    public:
        virtual ~Serializable() {};
        virtual DynamicData ToData() const = 0;
        virtual void FromData(const DynamicData &arData) = 0;
    };

    typedef unsigned int  size_type;

    /**
     * \fn DynamicData()
     * \brief Constructs a null value
     */
    DynamicData() : Variant() {}

    DynamicData(const DynamicData&);
    DynamicData(DynamicData&&);
    /**
     * \brief Construct a DynamicData holding the given value
     * \tparam T Type of value to contain
     * \param aValue Value to contain
     *
     * Use template to declare inherited constructors
     */
    template<class T>
    DynamicData(T aValue) : Variant(aValue) {}

    virtual ~DynamicData() {}

    DynamicData& operator=(const DynamicData&);
    DynamicData& operator=(DynamicData&&);
    /**
     * \brief Assign all types supported by Variant class
     *
     * Use template to declare inherited assignment operators
     */
    template<class T>
    DynamicData& operator=(T aValue) { rsp::utils::Variant::operator=(aValue); return *this; }

    /**
     * \brief Try to assign member value to lvalue.
     * \tparam T
     * \tparam I
     * \param arLValue
     * \param arIndex
     * \return True if successful
     */
    template<class T, class I>
    bool TryAssign(T& arLValue, const I& arIndex) const try
    {
        arLValue = (*this)[arIndex];
        return true;
    }
    catch(...) {
        return false;
    }

    /**
     * \brief Try to get member value else return default.
     * \tparam T
     * \tparam I
     * \param arIndex
     * \param arDefault
     * \return Value or default
     */
    template<class T, class I>
    T TryGet(I &arIndex, const T& arDefault) try
    {
        return (*this)[arIndex];
    }
    catch(...) {
        return arDefault;
    }

    /**
     * \brief Check if value content is an array
     * \return True if content is an array
     */
    bool IsArray() const { return (GetType() == Types::Array); }
    /**
     * \brief Check if value content is an object
     * \return True if content is an object
     */
    bool IsObject() const { return (GetType() == Types::Object); }

    /**
     * \brief Index operators for object members
     *
     * \param aKey Name of object member
     * \return Reference to value
     */
    DynamicData& operator[](std::string_view aKey);
    DynamicData& operator[](const std::string& arKey) { return (*this)[std::string_view(arKey)]; }
    DynamicData& operator[](const char* apKey)  { return (*this)[std::string_view(apKey)]; }

    const DynamicData& operator[](std::string_view aKey) const;
    const DynamicData& operator[](const std::string& arKey) const { return (*this)[std::string_view(arKey)]; }
    const DynamicData& operator[](const char* apKey) const { return (*this)[std::string_view(apKey)]; }

    /**
     * \brief Index operators for array members
     *
     * \param aIndex Index number
     * \return Reference to value
     */
    DynamicData& operator[](size_type aIndex);
    DynamicData& operator[](int aIndex) { return (*this)[static_cast<size_type>(aIndex)]; }

    const DynamicData& operator[](size_type aIndex) const;
    const DynamicData& operator[](int aIndex) const { return (*this)[static_cast<size_type>(aIndex)]; }

    /**
     * \brief Get the number of element in a object or array
     * \return unsigned int Number of elements
     */
    size_type GetCount() const;

    /**
     * \brief Get a list of member names if this is a data object.
     *
     * \return Vector of strings
     */
    std::vector<std::string> GetMemberNames() const;

    /**
     * \brief Check if a data object has a member with the specified name
     * \param aKey
     * \return
     */
    bool MemberExists(std::string_view aKey) const;

    /**
     * \brief Add a new element to the array
     * \param aValue DynamicData to append to array
     * \return Reference to this
     */
    DynamicData& Add(DynamicData aValue);

    /**
     * \brief Ass new member element to the object
     * \param aKey Name of member to add
     * \param aValue DynamicData to assign to member
     * \return Reference to this
     */
    DynamicData& Add(std::string_view aKey, DynamicData aValue);

    /**
     * \brief Remove a specified element from the array.
     * \param aIndex Integer index of element to remove
     * \return Reference to this
     */
    DynamicData& Remove(size_type aIndex);

    /**
     * \brief Remove a named element from an object
     * \param aKey String name of member to remove
     * \return Reference to this
     */
    DynamicData& Remove(std::string_view aKey);

    /**
     * \brief Clear this object. It will be a DynamicData Null object afterwards.
     */
    void Clear() override;

    /**
     * \brief Deep compare two DynamicData objects. Type and content must match.
     * \param arOther
     * \return True if equal
     */
    bool operator==(const DynamicData &arOther) const;

    /**
     * \brief Deep compare two DynamicData objects. Type and content must match.
     * \param arOther
     * \return True if NOT equal
     */
    bool operator!=(const DynamicData &arOther) const
    {
        return !((*this) == arOther);
    }

    const std::string GetName() const { return mName; }
    const std::vector<DynamicData>& GetItems() const { return mItems; }

protected:
    std::string mName{}; // Name if this value is an object member
    std::vector<DynamicData> mItems{}; // Owned list, used if this is of type object or array.

    void tryArray() const;
    void tryObject() const;
    void forceObject();
    void forceArray();
};

std::ostream& operator<< (std::ostream& os, const DynamicData& arValue);

} /* namespace rsp::utils */

#endif /* INCLUDE_UTILS_DYNAMICDATA_H_ */
