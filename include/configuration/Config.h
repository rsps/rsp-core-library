/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_CONFIGURATION_CONFIG_H_
#define INCLUDE_CONFIGURATION_CONFIG_H_

#include <memory>
#include <string>
#include <string_view>
#include <utils/json/Jsonable.h>
#include <utils/CoreException.h>

namespace rsp::config {

class EConfigSizeMismatch : public rsp::utils::CoreException
{
public:
    explicit EConfigSizeMismatch(std::size_t aExpected, std::size_t aActual)
      : CoreException(std::string("Config file size is ") + std::to_string(aActual) + ", it should be " + std::to_string(aExpected))
    {
    }
};

class EConfigValidation : public rsp::utils::CoreException
{
public:
    explicit EConfigValidation(std::string &arMessage)
      : CoreException(arMessage)
    {
    }
};

class EConfigWrite: public rsp::utils::CoreException
{
public:
    explicit EConfigWrite()
      : CoreException("Fatal error writing config data to file")
    {
    }
};

class ConfigBase : public rsp::utils::json::Jsonable
{
public:
    ConfigBase(std::uint8_t *apData, std::size_t aDataSize);
    virtual ~ConfigBase() {};

    void Load(const std::string &arFileName, std::string_view aSecret);
    void Save(const std::string &arFileName, std::string_view aSecret);

    virtual void validate() = 0;

protected:
    std::unique_ptr<std::uint8_t> mpData;
    std::size_t mDataSize;
};

template <typename T>
class ConfigType: public ConfigBase
{
public:
    ConfigType() : ConfigBase(&mData, sizeof(mData)) {}

    const T& Get() const { return mData; }
protected:
    T mData;
};


} // namespace rsp::config


#endif /* INCLUDE_CONFIGURATION_CONFIG_H_ */
