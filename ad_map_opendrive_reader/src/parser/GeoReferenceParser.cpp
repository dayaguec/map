/*
 * ----------------- BEGIN LICENSE BLOCK ---------------------------------
 *
 * Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
 * de Barcelona (UAB).
 * Copyright (C) 2019-2021 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 * ----------------- END LICENSE BLOCK -----------------------------------
 */

#include "opendrive/parser/GeoReferenceParser.h"
#include "opendrive/types.hpp"

#include <boost/algorithm/string.hpp>
#include <iostream>
#include <limits>
#include <vector>

#include <proj.h>

namespace opendrive {
namespace parser {

::opendrive::geom::GeoLocation GeoReferenceParser::Parse(const std::string &geo_reference_string)
{
  ::opendrive::geom::GeoLocation result{
    std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(), 0.0, ""};
  result.projection = geo_reference_string;

  auto projPtr = proj_create(0, geo_reference_string.c_str());

  if (projPtr != nullptr)
  {
    PJ_COORD refPoint;
    refPoint.uv.u = 0;
    refPoint.uv.v = 0;
    const auto geoPoint = proj_trans(projPtr, PJ_INV, refPoint);
    result.longitude = geoPoint.uv.u * 57.295779513082321;
    result.latitude = geoPoint.uv.v * 57.295779513082321;
  }
  else
  {
    std::vector<std::string> geo_ref_splitted;
    boost::split(geo_ref_splitted, geo_reference_string, boost::is_any_of(" "));
    for (auto const &element : geo_ref_splitted)
    {
      std::vector<std::string> geo_ref_key_value;
      boost::split(geo_ref_key_value, element, boost::is_any_of("="));
      if (geo_ref_key_value.size() != 2u)
      {
        continue;
      }

      if (geo_ref_key_value[0] == "+lat_0")
      {
        result.latitude = std::stod(geo_ref_key_value[1]);
      }
      else if (geo_ref_key_value[0] == "+lon_0")
      {
        result.longitude = std::stod(geo_ref_key_value[1]);
      }
    }
  }

  return result;
}

} // namespace parser
} // namespace opendrive
