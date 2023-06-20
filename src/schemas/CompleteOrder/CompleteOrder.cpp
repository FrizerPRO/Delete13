#include "CompleteOrder.h"

namespace CompleteOrder {
CompleteOrder Parse(const userver::formats::json::Value& json,
                    userver::formats::parse::To<CompleteOrder>) {
  CompleteOrder order;
  order.order_id = json["order_id"].As<std::int64_t>();
  order.courier_id = json["courier_id"].As<std::int64_t>();
  order.complete_time = json["complete_time"].As<std::chrono::system_clock::time_point>();
  return order;
}
}  // namespace CompleteOrder