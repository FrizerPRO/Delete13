#ifndef CreateCourierDto_H
#define CreateCourierDto_H
#include <userver/utils/time_of_day.hpp>
#include <vector>
#include <userver/formats/parse/common_containers.hpp>
#include <userver/formats/parse/time_of_day.hpp>
#include <userver/formats/serialize/time_of_day.hpp>
#include <userver/formats/json/value.hpp>

namespace CreateCourierDto {
class CreateCourierDto {
 public:
  std::string courier_type;
  std::vector<int> regions;
  std::vector<std::string> working_hours;
};
CreateCourierDto Parse(const userver::formats::json::Value& json,
                           userver::formats::parse::To<CreateCourierDto>);
}  // namespace CreateCourierDto

#endif  // CreateCourierDto_H
