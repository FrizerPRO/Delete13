#include "CreateOrderDto.h"
#include <sstream>
#include <string>
#include <userver/formats/json.hpp>

#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

namespace CreateOrderDto {
CreateOrderDto Parse(const userver::formats::json::Value& json,
                     userver::formats::parse::To<CreateOrderDto>) {
  CreateOrderDto orderDto;
  orderDto.weight = json["weight"].As<double>();
  orderDto.regions = json["regions"].As<int>();
  orderDto.delivery_hours = json["delivery_hours"].As<std::vector<std::string>>();
  orderDto.cost = json["cost"].As<int>();
  return orderDto;
}
}  // namespace CreateOrderDto

