#ifndef GetCouriersResponse_H
#define GetCouriersResponse_H

#include "../CourierDto/CourierDto.h"

namespace GetCouriersResponse {
class GetCouriersResponse {
 public:
  std::vector<CourierDto::CourierDto> couriers;
  int limit;
  int offset;
};
}  // namespace GetCouriersResponse

#endif  // GetCouriersResponse_H
