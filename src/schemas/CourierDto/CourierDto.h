#ifndef CourierDto_H
#define CourierDto_H

#include <cstdint>
#include <userver/utils/time_of_day.hpp>
#include <userver/formats/serialize/time_of_day.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <vector>
#include <userver/formats/json/value.hpp>

namespace CourierDto {
class CourierDto {
 public:
  std::int64_t courier_id;
  std::string courier_type;
  std::vector<int> regions;
  std::vector<std::string>
      working_hours;
};
userver::formats::json::Value Serialize(const CourierDto& data,
                               userver::formats::serialize::To<userver::formats::json::Value>);
}  // namespace CourierDto

#endif  // CourierDto_H
