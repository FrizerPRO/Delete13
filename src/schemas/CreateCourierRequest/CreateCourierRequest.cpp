#include "CreateCourierRequest.h"

namespace CreateCourierRequest {
CreateCourierRequest Parse(const userver::formats::json::Value& json,
                           userver::formats::parse::To<CreateCourierRequest>) {
  CreateCourierRequest res;
  res.couriers =
      json["couriers"].As<std::vector<CreateCourierDto::CreateCourierDto>>();
      return res;
}

}  // namespace CreateCourierRequest
