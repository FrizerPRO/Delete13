#ifndef CreateOrderDto_H
#define CreateOrderDto_H

#include <userver/utils/time_of_day.hpp>
#include <vector>
#include <userver/formats/json/value.hpp>
#include <userver/formats/parse/common_containers.hpp>
#include <userver/formats/parse/time_of_day.hpp>
#include <userver/formats/serialize/time_of_day.hpp>

namespace CreateOrderDto {
class CreateOrderDto {
 public:
  int weight;
  int regions;
  std::vector<std::string> delivery_hours;
  int cost;
};
    CreateOrderDto Parse(const userver::formats::json::Value& json,
               userver::formats::parse::To<CreateOrderDto>);
    // formats::json::Value Serialize(const MyKeyValue& data,
    //                   formats::serialize::To<formats::json::Value>)
}  // namespace CreateOrderDto
#endif  // CreateOrderDto_H
