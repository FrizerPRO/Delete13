#include "CreateOrderRequest.h"
#include <userver/formats/json.hpp>

namespace CreateOrderRequest {
CreateOrderRequest Parse(const userver::formats::json::Value& json,
                         userver::formats::parse::To<CreateOrderRequest>) {
  CreateOrderRequest createOrderRequest;
  createOrderRequest.orders =
      json["orders"].As<std::vector<CreateOrderDto::CreateOrderDto>>();
  return createOrderRequest;
}

}  // namespace CreateOrderRequest
