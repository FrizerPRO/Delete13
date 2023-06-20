#ifndef CouriersGroupOrders_H
#define CouriersGroupOrders_H

#include "../GroupOrders/GroupOrders.h"

namespace CouriersGroupOrders {
class CouriersGroupOrders {
 public:
  std::int64_t couriers_id;
  std::vector<GroupOrders::GroupOrders> orders;
};
}  // namespace CouriersGroupOrders

#endif  // CouriersGroupOrders_H
