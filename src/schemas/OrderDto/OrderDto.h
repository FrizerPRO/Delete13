#ifndef OrderDto_H
#define OrderDto_H

#include <chrono>
#include <cstdint>
#include <optional>
#include <userver/formats/json/value.hpp>
#include <userver/formats/parse/common_containers.hpp>
#include <userver/formats/parse/time_of_day.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/formats/serialize/time_of_day.hpp>
#include <userver/utils/time_of_day.hpp>
#include <vector>

namespace OrderDto {
class OrderDto {
 public:
  std::int64_t order_id;
  double weight;
  int regions;
  std::vector<std::string> delivery_hours;
  int cost;
  std::optional<std::chrono::system_clock::time_point> completed_time;
};
OrderDto Parse(const userver::formats::json::Value& json,
               userver::formats::parse::To<OrderDto>);
userver::formats::json::Value Serialize(
    const OrderDto& data,
    userver::formats::serialize::To<userver::formats::json::Value>);

}  // namespace OrderDto

#endif  // OrderDto_H
