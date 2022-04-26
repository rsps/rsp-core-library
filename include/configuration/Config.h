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

#include <utils/Variant.h>
#include <utils/json/Json.h>
#include <utils/json/Jsonable.h>
#include <utils/StructElement.h>

namespace rsp::config {

template<typename T>
class NamedElement : public rsp::utils::StructElement<T> {
public:
    NamedElement(const std::string &arName, const T &arValue) : StructElement(arValue), mElementName(arName) {}

    void ToJson(JsonObject &arJson) const override
    {
        arJson[mElementName] = (IsNull()) ? nullptr : Get();
    }

    void FromJson(const JsonObject &arJson)
    {
        Set(T(arJson[mElementName]));
    }

protected:
    std::string mElementName;
};

#define CFG_ITEM(type, name, value) NamedElement<type> name{#name, value}

struct MyConfig {

    CFG_ITEM(std::string, api_proxy, "https://aretaeus.rspsystems.com/api/v1"),
    CFG_ITEM(double, center_wavelength, 830.1726),
    CFG_ITEM(int, spec_cooling_timeout, 20),

    "poly_coefficients": {
      "poly_c0": "8.23181299e+02",
      "poly_c1": "1.91456080e-01",
      "poly_c2": "-2.9970E-04",
      "poly_c3": "-5.39880322e-12"
    },

};


/*
{
  "api_proxy": "https://development-aretaeus.rspamw.dk/api/v1",
  "center_wavelength": 830.3547,
  "meta": {
    "serial_number": "TEST-20211027-0825-B",
    "manufacturer_code": "TEST-JJ78053CBA",
    "batch": "0025",
    "model": {
      "slug": "ut-01",
      "name": "NewModel"
    },
    "brand": {
      "slug": "gluco-beam",
      "name": "Gluco Beam"
    },
    "configuration": {
      "id": 2,
      "name": "TEST-20211027-0825-B | NewModel",
      "template": {
        "id": 2,
        "name": "My New Template"
      }
    }
  },
  "poly_coefficients": {
    "poly_c0": "8.23181299e+02",
    "poly_c1": "1.91456080e-01",
    "poly_c2": "-2.9970E-04",
    "poly_c3": "-5.39880322e-12"
  },
  "MyItem": "yes"
}
 */

class ConfigBase
{
public:
    Config();
    Config(const std::string &arJson);

    void Load(const std::string &arFileName, const std::string &arSignSeed);
    void Save(const std::string &arFileName, const std::string &arSignSeed);

    using rsp::utils::json::Json::Decode;
    using rsp::utils::json::Json::Encode;

    template<typename T>
    const T& Get(const std::string &arName, const T &arDefault) const
    {
        if (!Get().AsObject().MemberExists(arName)) {
            return arDefault;
        }
        return Get().AsObject()[arName];
    }

    template<typename T>
    const T& Get(const std::string &arName) const
    {
        return Get().AsObject()[arName];
    }

protected:
    using rsp::utils::json::Json::Get;
};


} // namespace rsp::config

#endif /* INCLUDE_CONFIGURATION_CONFIG_H_ */
