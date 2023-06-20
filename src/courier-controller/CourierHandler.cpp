#include "CourierHandler.h"

#include <fmt/format.h>

#include <userver/clients/dns/component.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>

#include <userver/formats/parse/time_of_day.hpp>
#include <userver/formats/serialize/time_of_day.hpp>
#include <vector>

#include "../schemas/CourierDto/CourierDto.h"
#include "../schemas/CreateCourierRequest/CreateCourierRequest.h"

namespace couriers_handler_template {
CouriersHandler::CouriersHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context)
    : HttpHandlerBase(config, component_context),
      pg_cluster_(
          component_context
              .FindComponent<userver::components::Postgres>("postgres-db-1")
              .GetCluster()) {
  using userver::storages::postgres::ClusterHostType;
  constexpr auto kCreateTable = R"~(
    CREATE TABLE IF NOT EXISTS couriers_table (
        courier_id bigserial PRIMARY KEY,
        courier_type text,
        regions integer[],
        working_hours text[]
      )
    )~";

  pg_cluster_->Execute(ClusterHostType::kMaster, kCreateTable);
}

std::string CouriersHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
  switch (request.GetMethod()) {
    case userver::server::http::HttpMethod::kPost:
      return CouriersHandler::PostCouriers(request);
      break;
    case userver::server::http::HttpMethod::kGet:
      return CouriersHandler::GetCouriers(request);
      break;
    default:
      break;
  }
  return "";
}
std::string CouriersHandler::PostCouriers(
    const userver::server::http::HttpRequest& request) const {
  const userver::storages::postgres::Query kInsertValue{
      "INSERT INTO couriers_table "
      "(courier_type,regions,working_hours) "
      "VALUES ($1, $2, $3) "
      "ON CONFLICT DO NOTHING "
      "RETURNING *",
      userver::storages::postgres::Query::Name{"sample_insert_value"},
  };
  const userver::storages::postgres::Query kGetValues{
      "SELECT * FROM couriers_table ",
      userver::storages::postgres::Query::Name{"sample_get_value"},
  };

  auto request_data = request.RequestBody();
  auto courierRequest = userver::formats::json::FromString(request_data)
                            .As<CreateCourierRequest::CreateCourierRequest>();
  userver::storages::postgres::Transaction transaction = pg_cluster_->Begin(
      "sample_transaction_insert_key_value",
      userver::storages::postgres::ClusterHostType::kMaster, {});
  std::vector<std::string> aarray_of_prop;
    request.SetResponseStatus(userver::server::http::HttpStatus::kOk);

  for (auto courierDto : courierRequest.couriers) {
    auto res =
        transaction.Execute(kInsertValue, courierDto.courier_type,
                            courierDto.regions, courierDto.working_hours);
    if (!res.RowsAffected()) {
      transaction.Rollback();
      request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
      return "";
    }
    auto row = res.Front();
    userver::formats::json::ValueBuilder builder;
    builder["courier_id"] = row["courier_id"].As<int64_t>();
    builder["courier_type"] =
        row["courier_type"].As<std::string>();
    builder["regions"] = row["regions"].As<std::vector<int>>();
    builder["working_hours"] =
        row["working_hours"].As<std::vector<std::string>>();
    aarray_of_prop.push_back(ToString(builder.ExtractValue()));
  }
  transaction.Commit();
  userver::formats::json::ValueBuilder res_json;
  res_json["couriers"] = aarray_of_prop;
  return ToString(res_json.ExtractValue());
}
std::string CouriersHandler::GetCouriers(
    const userver::server::http::HttpRequest& request) const {
  const userver::storages::postgres::Query kGetValues{
      "SELECT * FROM couriers_table LIMIT $1 OFFSET $2",
      userver::storages::postgres::Query::Name{"sample_get_value"},
  };
  auto limit = 1;
  auto offset = 0;
  if (request.HasArg("offset")) {
    try {
      offset = std::stoi(request.GetArg("offset"));
    } catch (const std::exception&) {
      request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
      return "";
    }
  }
  if (request.HasArg("limit")) {
    try {
      limit = std::stoi(request.GetArg("limit"));
    } catch (const std::exception&) {
      request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
      return "";
    }
  }

  auto res =
      pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                           kGetValues, limit, offset);
  if (res.IsEmpty()) {
    request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
    return "";
  }
  std::vector<std::string> aarray_of_prop;

  for (auto row : res) {
    userver::formats::json::ValueBuilder builder;
    builder["courier_id"] = row["courier_id"].As<int64_t>();
    builder["courier_type"] =
        row["courier_type"].As<std::string>();
    builder["regions"] = row["regions"].As<std::vector<int>>();
    builder["working_hours"] =
        row["working_hours"].As<std::vector<std::string>>();
    aarray_of_prop.push_back(ToString(builder.ExtractValue()));
  }
  userver::formats::json::ValueBuilder res_json;
  res_json["couriers"] = aarray_of_prop;
  res_json["limit"] = limit;
  res_json["offset"] = offset;
  return ToString(res_json.ExtractValue());
}
void AppendOrdersHandler(userver::components::ComponentList& component_list) {
  component_list.Append<CouriersHandler>();
}

}  // namespace couriers_handler_template
