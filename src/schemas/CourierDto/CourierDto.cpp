#include "CourierDto.h"
#include <sstream>
#include <string>
#include <userver/formats/json.hpp>
#include <userver/formats/parse/time_of_day.hpp>
#include <userver/formats/serialize/time_of_day.hpp>
#include <userver/logging/log.hpp>

namespace CourierDto {
userver::formats::json::Value Serialize(
    const CourierDto& data,
    userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder builder;
  builder["courier_id"] = data.courier_id;  
  builder["courier_type"] = data.courier_type;
  builder["regions"] = data.regions;
  builder["working_hours"] = data.working_hours;
  return builder.ExtractValue();
}
}  // namespace CourierDto