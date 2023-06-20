#ifndef GetCourierMetaInfoResponse_H
#define GetCourierMetaInfoResponse_H

#include <cstdint>
#include <userver/utils/time_of_day.hpp>
#include <vector>
namespace GetCourierMetaInfoResponse {
class GetCourierMetaInfoResponse {
 public:
  std::int64_t courier_id;
  enum CourierType { FOOT, BIKE, AUTO };
  CourierType courier_type;
  std::vector<int> regions;
  std::vector<userver::utils::datetime::TimeOfDay<std::chrono::minutes>>
      working_hours;
  int rating;
  int earnings;
};
}  // namespace GetCourierMetaInfoResponse

#endif  // GetCourierMetaInfoResponse_H
