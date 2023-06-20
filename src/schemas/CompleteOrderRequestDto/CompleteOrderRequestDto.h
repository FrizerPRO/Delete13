#ifndef CompleteOrderRequestDto_H
#define CompleteOrderRequestDto_H

#include <vector>
#include "../CompleteOrder/CompleteOrder.h"

namespace CompleteOrderRequestDto {
class CompleteOrderRequestDto {
 public:
  std::vector<CompleteOrder::CompleteOrder> complete_info;
};
CompleteOrderRequestDto Parse(const userver::formats::json::Value& json,
               userver::formats::parse::To<CompleteOrderRequestDto>);
}  // namespace CompleteOrderRequestDto

#endif  // CompleteOrderRequestDto_H
