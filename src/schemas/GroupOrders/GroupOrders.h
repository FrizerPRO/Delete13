#ifndef GroupOrders_H
#define GroupOrders_H

#include <cstdint>
#include <vector>
#include "../OrderDto/OrderDto.h"
#include "GroupOrders.h"
namespace GroupOrders {
class GroupOrders {
 public:
  std::int64_t group_order_id;
  std::vector<OrderDto::OrderDto> orders;
};
}  // namespace GroupOrders

#endif  // GroupOrders_H
