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

#include <exceptions/CoreException.h>
#include <memory>
#include <type_traits>
#include <posix/FileIO.h>

namespace rsp::utils {

/**
 * \brief Base class for DataContainer exceptions.
 */
struct DataContainerException : public exceptions::CoreException
{
    explicit DataContainerException(const std::string &arMsg)
      : CoreException(arMsg)
    {
    }
};

/**
 * \brief Predefined exeption thrown on invalid signatures on stored data.
 */
struct EInvalidSignature : public DataContainerException
{
    explicit EInvalidSignature()
        : DataContainerException("Invalid signature")
    {
    }
};


struct IDataContent;

/**
 * \brief Interface for data signature objects
 */
struct IDataSignature
{
    virtual ~IDataSignature() {}

    /**
     * \brief Get size of signature buffer
     * \return size_t
     */
    virtual std::size_t GetSize() const = 0;

    /**
     * \brief Get start address of signature buffer
     * \return uint8_t*
     */
    virtual std::uint8_t* GetData() = 0;

    /**
     * \brief Get const start address of signature buffer
     * \return const uint8_t*
     */
    virtual const std::uint8_t* GetData() const = 0;

    /**
     * \brief Calculate the signature on the given data. Store the signature in internal buffer.
     * \param arContent Reference to IDataContent instance
     */
    virtual void Calc(const IDataContent &arContent) = 0;

    /**
     * \brief Verify if the internal signature mathes the one calulcated for the given data.
     * \param arContent const reference to IDataContent instance
     */
    virtual void Verify(const IDataContent &arContent) = 0;
};

/**
 * \brief Interface for data storage objects.
 *
 * All data storage is a two step operation:
 *   1. One to read/write the signature part.
 *   2. Another to read/write the actual data content.
 */
struct IDataStorage
{
    virtual ~IDataStorage() {}

    /**
     * \brief Read the signature from data store into given buffer
     * \param arContent Reference to IDataSignature instance
     */
    virtual void ReadSignature(IDataSignature &arSignature) = 0;

    /**
     * \brief Write the signature from given buffer to data store
     * \param arContent const reference to IDataSignature instance
     */
    virtual void WriteSignature(const IDataSignature &arSignature) = 0;

    /**
     * \brief Read the data content from data store into given buffer
     * \param arContent Reference to IDataContent instance
     */
    virtual void Read(IDataContent &arContent) = 0;

    /**
     * \brief Write the data content from given buffer to the data store
     * \param arContent const reference to IDataContent instance
     */
    virtual void Write(const IDataContent &arContent) = 0;

    /**
     * \brief Get the total size of the storage, used for dynamic containers.
     * \return size_t
     */
    virtual std::size_t GetSize() = 0;
};

/**
 * \brief Interface for data content objects
 */
struct IDataContent
{
    virtual ~IDataContent() {}

    /**
     * \brief Get the size of the data content buffer
     * \return size_t
     */
    virtual std::size_t GetSize() const = 0;

    /**
     * \brief Get a pointer to the start address of the content buffer
     * \return uint8_t*
     */
    virtual std::uint8_t* GetData() = 0;

    /**
     * \brief Get a const pointer to the start address of the content buffer
     * \return const uint8_t*
     */
    virtual const std::uint8_t* GetData() const = 0;
};

/**
 * \brief Streaming operator to hex dump the signature
 * \param os Output stream
 * \param arSignature Signature to print
 * \return output stream
 */
std::ostream& operator<<(std::ostream& os, const IDataSignature &arSignature);

/**
 * \brief Streaming operator to hex dump the content
 * \param os Output stream
 * \param arContent Content to print
 * \return output stream
 */
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

    std::uint8_t* GetData() override { return reinterpret_cast<std::uint8_t*>(&mCRC); }
    const std::uint8_t* GetData() const override { return reinterpret_cast<const std::uint8_t*>(&mCRC); }
    std::size_t GetSize() const override { return sizeof(std::uint32_t); }
    void Calc(const rsp::utils::IDataContent &arContent) override;
    void Verify(const rsp::utils::IDataContent &arContent) override;

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

    void Read(rsp::utils::IDataContent &arContent) override;
    void Write(const rsp::utils::IDataContent &arContent) override;
    void ReadSignature(rsp::utils::IDataSignature &arSignature) override;
    void WriteSignature(const rsp::utils::IDataSignature &arSignature) override;
    std::size_t GetSize() override;

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

    static constexpr bool has_size = requires(const _DATA& t) { t.size(); };
    static constexpr bool has_data = requires(const _DATA& t) { t.data(); };

public:

    virtual ~DataContainerBase() {}

    /**
     * \brief Load signature and content from file, using the given interfaces
     */
    void Load()
    {
        mStorage.ReadSignature(mSignature);
        if constexpr (has_size) {
            mData.resize(mStorage.GetSize() - mSignature.GetSize());
        }
        mStorage.Read(*this);
        mSignature.Verify(*this);
    }

    /**
     * \brief Save signature and content to file, using the given interfaces
     */
    void Save()
    {
        mSignature.Calc(*this);
        mStorage.WriteSignature(mSignature);
        mStorage.Write(*this);
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


    /*
     * IDataContent interface implementation
     */
    std::size_t GetSize() const override
    {
        if constexpr (has_size) {
            return mData.size();
        }
        else {
            return sizeof(mData);
        }
    }
    std::uint8_t* GetData() override
    {
        if constexpr (has_data) {
            return reinterpret_cast<std::uint8_t*>(mData.data());
        }
        else {
            return reinterpret_cast<std::uint8_t*>(&mData);
        }
    }
    const std::uint8_t* GetData() const override
    {
        if constexpr (has_data) {
            return reinterpret_cast<const std::uint8_t*>(mData.data());
        }
        else {
            return reinterpret_cast<const std::uint8_t*>(&mData);
        }
    }

protected:
    _SIGNATURE mSignature{};
    _STORAGE   mStorage{};
#pragma pack(1)
    _DATA mData{};
#pragma pack()
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
