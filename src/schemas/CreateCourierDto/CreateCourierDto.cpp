#include "CreateCourierDto.h"
#include <userver/formats/json.hpp>

#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/formats/json/value.hpp>

namespace CreateCourierDto {
CreateCourierDto Parse(const userver::formats::json::Value& json,
                       userver::formats::parse::To<CreateCourierDto>) {
  CreateCourierDto res;

  res.courier_type = json["courier_type"].As<std::string>();

  res.regions = json["regions"].As<std::vector<int>>();
  res.working_hours = json["working_hours"].As<std::vector<std::string>>();
  return res;
}

}  // namespace CreateCourierDto
