#ifndef CreateOrderRequest_H
#define CreateOrderRequest_H

#include <vector>
#include "../CreateOrderDto/CreateOrderDto.h"
#include <userver/formats/json/value.hpp>
#include <userver/formats/parse/common_containers.hpp>

  namespace CreateOrderRequest {
class CreateOrderRequest {
 public:
  std::vector<CreateOrderDto::CreateOrderDto> orders;
};
CreateOrderRequest Parse(const userver::formats::json::Value& json,
                         userver::formats::parse::To<CreateOrderRequest>);
}  // namespace CreateOrderRequest
#endif  // CreateOrderRequest_H
