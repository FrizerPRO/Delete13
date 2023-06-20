#include "OrderDto.h"
#include <userver/formats/json.hpp>
#include <userver/formats/parse/time_of_day.hpp>
#include <userver/formats/serialize/time_of_day.hpp>
#include <userver/formats/parse/time_of_day.hpp>
#include <sstream>
#include <string>
#include <userver/logging/log.hpp>

namespace OrderDto {


OrderDto Parse(const userver::formats::json::Value& json,
               userver::formats::parse::To<OrderDto>) {
  OrderDto orderDto;
  orderDto.order_id = json["order_id"].As<int64_t>();
  orderDto.weight = json["weight"].As<double>();
  orderDto.regions = json["regions"].As<int>();
  orderDto.delivery_hours = json["delivery_hours"].As<std::vector<std::string>>();
  orderDto.cost = json["cost"].As<int>();
  if (!json["completed_time"].IsEmpty()) {
    orderDto.completed_time =
        json["completed_time"].As<std::chrono::system_clock::time_point>();
  }
  return orderDto;
}

userver::formats::json::Value Serialize(const OrderDto& data,
                               userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder builder;
    builder["order_id"] = data.order_id;
    builder["weight"] = data.weight;
    builder["regions"] = data.regions;
    builder["delivery_hours"] = data.delivery_hours;
    builder["cost"] = data.cost;
    if(data.completed_time.has_value()){
          builder["completed_time"] = data.completed_time.value();
    }
  return builder.ExtractValue();
}
}  // namespace OrderDto
