#ifndef CompleteOrder_H
#define CompleteOrder_H

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

namespace CompleteOrder {
class CompleteOrder {
 public:
  std::int64_t courier_id;
  std::int64_t order_id;
  std::chrono::system_clock::time_point complete_time;
};
CompleteOrder Parse(const userver::formats::json::Value& json,
               userver::formats::parse::To<CompleteOrder>);
}  // namespace CompleteOrder
#endif  // CompleteOrder_H
