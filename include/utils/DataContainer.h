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

class EInvalidSignature : public CoreException
{
public:
    explicit EInvalidSignature(const std::string &arFileName)
      : CoreException("Invalid signature in file " + arFileName)
    {
    }
};

class EInvalidHeader : public CoreException
{
public:
    explicit EInvalidHeader(const std::string &arFileName)
      : CoreException("Invalid header in file " + arFileName)
    {
    }
};

/**
 * \brief Flag definitions for data storage containers.
 */
enum class ContainerFlags : std::uint8_t {
    None              = 0x00,
    ExtendedContainer = 0x01,
    Signed            = 0x02,
    Encrypted         = 0x04
};


/**
 * Placeholder type for any signature up to 512 bits.
 */
using Signature_t = std::uint8_t[64];

/**
 * \brief Header definition for simple data containers.
 *
 * All values are stored in the CPU's native format, so containers might not be portable.
 */
struct ContainerHeader
{
    const std::uint8_t HeaderSize;
    EnumFlags<ContainerFlags> Flags;
    const std::uint8_t  ContainerVersion;
    std::uint8_t PayloadVersion = 0;
    std::uint32_t PayloadSize = 0;

    ContainerHeader(std::uint8_t aSize = sizeof(ContainerHeader), ContainerFlags aFlags = ContainerFlags::None, std::uint8_t aVersion = 1)
        : HeaderSize(aSize),
          Flags(aFlags),
          ContainerVersion(aVersion)
    {
        if (Flags & ContainerFlags::Encrypted) {
            std::cout << "Encrypted";
        }
    }
} __attribute__((packed)); // 8 bytes

/**
 * \brief Header definition for signed data containers.
 *
 * Values are stored in the CPU's native format, so containers might not be portable.
 */
struct ContainerHeaderExtended : public ContainerHeader
{
    Signature_t  Signature{};

    ContainerHeaderExtended()
        : ContainerHeader(sizeof(ContainerHeaderExtended), ContainerFlags::ExtendedContainer)
    {
    }
} __attribute__((packed)); // 72 bytes


/**
 * \brief Generic base class for containers with basic load and save functionality.
 */
class DataContainerBase
{
public:
    DataContainerBase(ContainerHeader *apHeader, std::uint8_t *apData, std::size_t aDataSize);
    virtual ~DataContainerBase() {};

    void Load(const std::string &arFileName, std::string_view aSecret = "");
    void Save(const std::string &arFileName, std::string_view aSecret = "");

protected:
    std::unique_ptr<ContainerHeader> mpHeader;
    std::unique_ptr<std::uint8_t> mpData;
    std::size_t mDataSize;

    template <class T>
    T* getHeaderAs() { return static_cast<T*>(mpHeader.get()); }

private:
    virtual bool checkSignature(std::string_view aSecret) { return true; }
    virtual bool getSignature(Signature_t &arSignature, std::string_view aSecret) { return true; }
    virtual void readFrom(rsp::posix::FileIO &arFile);
    virtual void writeTo(rsp::posix::FileIO &arFile);
};

/**
 * \brief Template for data containers. Usage: DataContainer<MyStruct> my_data;
 * \tparam D Data structure to put in container
 * \tparam H
 */
template <class D, class H = ContainerHeader, class B = DataContainerBase>
class DataContainer: public B
{
public:
    DataContainer() : B(&mHeader, &mData, sizeof(mData)) {}

    const H& GetHeader() const { return mHeader; }
    const D& Get() const { return mData; }
protected:
    H mHeader{};
    D mData{};
};

} /* namespace rsp::utils */


#endif /* INCLUDE_UTILS_DATACONTAINER_H_ */
