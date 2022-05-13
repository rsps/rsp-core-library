/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_UTILS_DATACONTAINER_H_
#define INCLUDE_UTILS_DATACONTAINER_H_

#include <memory>
#include <type_traits>
#include <posix/FileIO.h>
#include "CoreException.h"
#include "EnumFlags.h"

namespace rsp::utils {

/**
 * \brief Base class for DataContainer exceptions.
 */
struct DataContainerException : public CoreException
{
    explicit DataContainerException(const std::string &arMsg)
      : CoreException(arMsg)
    {
    }
};

struct EInvalidSignature : public DataContainerException
{
    explicit EInvalidSignature()
        : DataContainerException("Invalid signature")
    {
    }
};


/**
 * \brief Interface for data signature objects
 */
struct IDataSignature
{
    virtual ~IDataSignature() {}
    virtual std::size_t GetSize() const = 0;
    virtual std::uint8_t* Get() = 0;
    virtual const std::uint8_t* Get() const = 0;
    virtual void Calc(const std::uint8_t *apData, std::size_t aSize) = 0;
    virtual void Verify(const std::uint8_t *apData, std::size_t aSize) = 0;
};

/**
 * \brief Interface for data storage objects
 */
struct IDataStorage
{
    virtual ~IDataStorage() {}
    virtual void ReadSignature(std::uint8_t *apBuffer, std::size_t aSize) = 0;
    virtual void WriteSignature(std::uint8_t *apBuffer, std::size_t aSize) = 0;
    virtual void Read(std::uint8_t *apData, std::size_t aSize) = 0;
    virtual void Write(const std::uint8_t *apData, std::size_t aSize) = 0;
};

/**
 * \brief Interface for data content objects
 */
struct IDataContent
{
    virtual ~IDataContent() {}
    virtual std::size_t GetSize() const = 0;
    virtual std::uint8_t* GetData() = 0;
    virtual const std::uint8_t* GetData() const = 0;
};

std::ostream& operator<<(std::ostream& os, const IDataSignature &arSignature);
std::ostream& operator<<(std::ostream& os, const IDataContent &arContent);


/**
 * \brief Signature implementation that uses CRC32 with an optional secret.
 */
class Crc32DataSignature : public IDataSignature
{
public:
    /**
     * \brief Initialize the signature object with the secret to use during crc32 calculation.
     * \param aSecret String with secret, empty string gives raw CRC32 signature.
     */
    void Init(std::string_view aSecret);

    std::uint8_t* Get() override { return reinterpret_cast<std::uint8_t*>(&mCRC); }
    const std::uint8_t* Get() const override { return reinterpret_cast<const std::uint8_t*>(&mCRC); }
    std::size_t GetSize() const override { return sizeof(std::uint32_t); }
    void Calc(const uint8_t *apData, std::size_t aSize) override;
    void Verify(const uint8_t *apData, std::size_t aSize) override;

protected:
    std::uint32_t mCRC{0};
    std::string_view mSecret{};
};


/**
 * \brief Storage implementation for generic file storage
 */
class FileDataStorage : public IDataStorage
{
public:
    /**
     * \brief Initialize the storage object with the filename to use.
     * \param aFileName String with path to file.
     */
    void Init(std::string_view aFileName);

    void Read(uint8_t *apData, std::size_t aSize) override;
    void Write(const uint8_t *apData, std::size_t aSize) override;
    void ReadSignature(uint8_t *apBuffer, std::size_t aSize) override;
    void WriteSignature(uint8_t *apBuffer, std::size_t aSize) override;

protected:
    std::string mFileName{};
    rsp::posix::FileIO mFile{};
};


/**
 * \brief Generic base class for high integrity data containers with basic load and save functionality.
 */
template <class _DATA, class _SIGNATURE, class _STORAGE>
class DataContainerBase : public IDataContent
{
    static_assert(std::is_base_of<IDataSignature, _SIGNATURE>::value, "_SIGNATURE must inherit from IDataSignature");
    static_assert(std::is_base_of<IDataStorage, _STORAGE>::value, "_STORAGE must inherit from IDataStorage");
public:

    virtual ~DataContainerBase() {}

    /**
     * \brief Load signature and content from file, using the given interfaces
     */
    void Load()
    {
        mStorage.ReadSignature(mSignature.Get(), mSignature.GetSize());
        mStorage.Read(GetData(), GetSize());
        mSignature.Verify(GetData(), GetSize());
    }

    /**
     * \brief Save signature and content to file, using the given interfaces
     */
    void Save()
    {
        this->mSignature.Calc(GetData(), GetSize());
        mStorage.WriteSignature(mSignature.Get(), mSignature.GetSize());
        mStorage.Write(GetData(), GetSize());
    }

    /**
     * \brief Get a reference to the internal content object.
     * \return Reference to content. (struct or class)
     */
    _DATA& Get() { return mData; }

    /**
     * \brief Get a const reference to the internal content object.
     * \return const reference to content.
     */
    const _DATA& Get() const { return mData; }

    /**
     * \brief Get the internal signature object.
     * \return Reference to signature object
     */
    _SIGNATURE& GetSignature() { return mSignature; }

    /**
     * \brief Get the internal storage object
     * \return Reference to storage object
     */
    _STORAGE& GetStorage() { return mStorage; }


    std::size_t GetSize() const override
    {
        return sizeof(mData);
    }

    std::uint8_t* GetData() override
    {
        return reinterpret_cast<std::uint8_t*>(&mData);
    }

    const std::uint8_t* GetData() const override
    {
        return reinterpret_cast<const std::uint8_t*>(&mData);
    }

protected:
    _SIGNATURE mSignature{};
    _STORAGE   mStorage{};
    _DATA mData{};
};


template<class T>
class DataContainer : public DataContainerBase<T, Crc32DataSignature, FileDataStorage>
{
public:
    DataContainer(std::string_view aFileName, std::string_view mSecret = "")
    {
        this->GetSignature().Init(mSecret);
        this->GetStorage().Init(aFileName);
    }
};

} /* namespace rsp::utils */


#endif /* INCLUDE_UTILS_DATACONTAINER_H_ */
