#ifndef OrderAssignResponse_H
#define OrderAssignResponse_H

#include "../CouriersGroupOrders/CouriersGroupOrders.h"

#include <userver/utils/datetime/date.hpp>

namespace OrderAssignResponse {
class OrderAssignResponse {
 public:
  userver::utils::datetime::Date date;
  std::vector<CouriersGroupOrders::CouriersGroupOrders> couriers;
};
}  // namespace OrderAssignResponse
#endif  // OrderAssignResponse_H
