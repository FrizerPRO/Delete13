#ifndef CreateCourierRequest_H
#define CreateCourierRequest_H

#include "../CreateCourierDto/CreateCourierDto.h"
#include <userver/formats/json/value.hpp>

namespace CreateCourierRequest {
class CreateCourierRequest {
 public:
  std::vector<CreateCourierDto::CreateCourierDto> couriers;
};
CreateCourierRequest Parse(const userver::formats::json::Value& json,
                         userver::formats::parse::To<CreateCourierRequest>);
}  // namespace CreateCourierRequest

#endif  // CreateCourierRequest_H
