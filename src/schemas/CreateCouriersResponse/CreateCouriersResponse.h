#ifndef CreateCouriersResponse_H
#define CreateCouriersResponse_H

#include "../CourierDto/CourierDto.h"
#include <userver/formats/json/value.hpp>

namespace CreateCouriersResponse {
class CreateCouriersResponse {
 public:
  std::vector<CourierDto::CourierDto> couriers;
};
}  // namespace CreateCouriersResponse
#endif  // CreateCouriersResponse_H
