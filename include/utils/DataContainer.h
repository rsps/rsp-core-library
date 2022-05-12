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
    explicit EInvalidSignature(const std::string &arFileName)
      : DataContainerException("Invalid signature in file " + arFileName)
    {
    }
};

struct EInvalidCRC : public DataContainerException
{
    explicit EInvalidCRC(const std::string &arFileName)
      : DataContainerException("Invalid CRC in file " + arFileName)
    {
    }
};

struct EInvalidHeader : public DataContainerException
{
    explicit EInvalidHeader(const std::string &arFileName)
      : DataContainerException("Invalid header in file " + arFileName)
    {
    }
};

struct ESizeDiffersInAssignment : public DataContainerException
{
    explicit ESizeDiffersInAssignment()
      : DataContainerException("Copy assignment of DataContainers is not same size")
    {
    }
};


/**
 * \brief Flag definitions for data storage containers.
 */
enum class ContainerFlags : std::uint8_t {
    None        = 0x00,
    Extended    = 0x01,
    Encrypted   = 0x02
};


/**
 * Placeholder type for any signature up to 512 bits.
 */
using Signature_t = std::uint8_t[64];

/**
 * \brief Header definition for simple data containers.
 *
 * All values are stored in the CPU's native format, so containers might not be portable.
 *
 * The simple header holds a CRC32 value of the payload content, used to ensure data integrity.
 */
struct ContainerHeader
{
    std::uint8_t Size;
    EnumFlags<ContainerFlags> Flags;
    std::uint8_t  Version;
    std::uint8_t PayloadVersion = 0;
    std::uint32_t PayloadCRC = 0;
    std::uint32_t PayloadSize = 0;

    ContainerHeader(std::uint8_t aSize = sizeof(ContainerHeader), ContainerFlags aFlags = ContainerFlags::None, std::uint8_t aVersion = 1);
    ContainerHeader& operator=(const ContainerHeader&) = default;
} __attribute__((packed)); // 12 bytes

/**
 * \brief Header definition for signed data containers.
 *
 * Values are stored in the CPU's native format, so containers might not be portable.
 *
 * The extended header adds space for a HMAC signature of the container contents. The HMAC calculation
 * can be seeded with a secret, to make the data container very resistant to tampering.
 */
struct ContainerHeaderExtended : public ContainerHeader
{
    Signature_t  Signature{};

    ContainerHeaderExtended()
        : ContainerHeader(sizeof(ContainerHeaderExtended), ContainerFlags::Extended)
    {
    }
    ContainerHeaderExtended& operator=(const ContainerHeaderExtended&) = default;
} __attribute__((packed)); // 76 bytes

std::ostream& operator<<(std::ostream& os, const ContainerHeader &arHeader);
std::ostream& operator<<(std::ostream& os, const ContainerHeaderExtended &arHeader);
std::ostream& operator<<(std::ostream& os, const Signature_t &arBuffer);


/**
 * \brief Generic base class for high integrity data containers with basic load and save functionality.
 */
class DataContainerBase
{
public:
    DataContainerBase(ContainerHeader *apHeader, std::uint8_t *apData, std::size_t aDataSize);
    DataContainerBase(const DataContainerBase &arOther) = delete;
    DataContainerBase(const DataContainerBase &&arOther) = delete;
    virtual ~DataContainerBase() {};

    DataContainerBase& operator=(const DataContainerBase &arOther);
    DataContainerBase& operator=(const DataContainerBase &&arOther) = delete;

    /**
     * \brief Load the container contents from the given file
     *
     * Several validation checks are automatically performed on the content.
     *
     * \param arFileName path to existing container file
     * \param aSecret Optional secret used for HMAC signature
     */
    void Load(const std::string &arFileName, std::string_view aSecret = "");

    /**
     * \brief Save the container contents to the given file. If the file exists it is overwritten.
     *
     * CRC of payload is automatically calculated and put into file header.
     * If the container is of extended type, a signing signature is also calculated on the payload.
     *
     * \param arFileName Path to file where content is stored.
     * \param aSecret Optional secret used for HMAC signature.
     */
    void Save(const std::string &arFileName, std::string_view aSecret = "");

protected:
    ContainerHeader* mpHeader;
    std::uint8_t* mpData;
    std::uint32_t mDataSize;

    ContainerHeader& getHeader() { return *mpHeader; }
    ContainerHeaderExtended& getExtHeader() { return *reinterpret_cast<ContainerHeaderExtended*>(mpHeader); }

private:
    /**
     * \brief Interface function for verifying the contents signature.
     *
     * \param aSecret String with seed for hash calculation.
     * \return bool True if signature is valid.
     */
    virtual bool checkSignature(std::string_view aSecret) { return true; }

    /**
     * \brief Interface function to calculate the content hash value, and fill it into the signature buffer.
     *
     * \param arSignature Reference to buffer for signature.
     * \param aSecret Seed to be used in hash generation.
     * \return bool True if a signature was calculated.
     */
    virtual bool getSignature(Signature_t &arSignature, std::string_view aSecret) { return false; }

    /**
     * \brief Interface function to read the container content from a file.
     *
     * \param arFile Open FileIO object, ready to read content from.
     */
    virtual void readPayloadFrom(rsp::posix::FileIO &arFile);

    /**
     * \brief Interface function to write container content to a file.
     *
     * \param arFile An open FileIO object, ready to write the content to.
     * \return bool True if any header data was changed during write. Used if encryption changes the payload size.
     */
    virtual bool writePayloadTo(rsp::posix::FileIO &arFile);

    /**
     * \brief Interface function to calculate the CRC32 value of the content.
     *
     * The default implementation calculated the CRC32 on the plain data content.
     *
     * \return uint32_t CRC32 result
     */
    virtual std::uint32_t calcCRC() const;
};

/**
 * \brief Template for data containers. Usage: DataContainer<MyStruct> my_data;
 * \tparam D Data structure to put in container
 * \tparam H Header to be used in container file.
 */
template <class D, class H = ContainerHeader, class B = DataContainerBase>
class DataContainer: public B
{
public:
    /**
     * \brief Specialized constructor, transfers memory addresses to base class for generic handling.
     */
    DataContainer() : B(&mHeader, reinterpret_cast<std::uint8_t*>(&mData), sizeof(mData)) {}

    /**
     * \brief Get a reference to the internal header object.
     * \return Reference to header.
     */
    H& GetHeader() { return mHeader; }
    /**
     * \brief Get a const reference to the internal header object.
     * \return const reference to header
     */
    const H& GetHeader() const { return mHeader; }

    /**
     * \brief Get a reference to the internal content object.
     * \return Reference to content. (struct or class)
     */
    D& Get() { return mData; }

    /**
     * \brief Get a const reference to the internal content object.
     * \return const reference to content.
     */
    const D& Get() const { return mData; }

    /**
     * \brief Operator to return copy of content object
     */
    operator D() const { return mData; }
protected:
    H mHeader{};
    D mData{};
};

} /* namespace rsp::utils */


#endif /* INCLUDE_UTILS_DATACONTAINER_H_ */
