#include "CourierInfoHandler.h"

#include <fmt/format.h>

#include <chrono>
#include <userver/clients/dns/component.hpp>
#include <userver/formats/parse/time_of_day.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/formats/serialize/time_of_day.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>
#include <userver/utils/datetime.hpp>
#include <userver/utils/datetime/date.hpp>
#include <vector>

namespace couriers_info_handler_template {

CourierInfoHandler::CourierInfoHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context)
    : HttpHandlerBase(config, component_context),
      pg_cluster_(
          component_context
              .FindComponent<userver::components::Postgres>("postgres-db-1")
              .GetCluster()) {}

std::string CourierInfoHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
  const userver::storages::postgres::Query kGetCourier{
      "SELECT * FROM couriers_table "
      "WHERE courier_id = $1",
      userver::storages::postgres::Query::Name{"sample_get_value"},
  };
  const userver::storages::postgres::Query kGetEarnings{
      "SELECT SUM(orders_table.cost) AS total_cost "
      "FROM orders_to_couriers "
      "JOIN orders_table ON orders_to_couriers.order_id = orders_table.order_id "
      "WHERE orders_to_couriers.courier_id = $1 "
      "AND orders_to_couriers.completed_time >= $2 "
      "AND orders_to_couriers.completed_time <= $3;",
      userver::storages::postgres::Query::Name{"sample_get_value"},
  };

  const userver::storages::postgres::Query kGetRating{
      "SELECT CAST(COUNT(*) / EXTRACT(EPOCH FROM ($3 - $2)) AS float) "
      "FROM orders_to_couriers "
      "WHERE courier_id = $1 "
      "AND completed_time >= $2 "
      "AND completed_time <= $3;",
      userver::storages::postgres::Query::Name{"sample_get_value"},
  };

  auto courier_id = userver::formats::json::FromString(request.GetPathArg("id"))
                        .As<int64_t>();
  auto courier_res =
      pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                           kGetCourier, courier_id);
  if (courier_res.IsEmpty()) {
    request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
    return "";
  }

  std::chrono::time_point start_date =
      userver::utils::datetime::Date(
          userver::utils::datetime::DateFromRFC3339String(
              request.GetArg("startDate")))
          .GetSysDays();
  std::chrono::time_point end_date =
      userver::utils::datetime::Date(
          userver::utils::datetime::DateFromRFC3339String(
              request.GetArg("endDate")))
          .GetSysDays();
  auto earnings_res =
      pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                           kGetEarnings, courier_id, start_date, end_date);
  if (earnings_res.IsEmpty()) {
    request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
    return "";
  }
  auto raiting_res =
      pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                           kGetRating, courier_id, start_date, end_date);
  if (raiting_res.IsEmpty()) {
    request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
    return "";
  }

  auto courier = courier_res.Front();
  request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
  userver::formats::json::ValueBuilder courier_builder;
  courier_builder["courier_id"] = courier["courier_id"].As<int64_t>();
  courier_builder["courier_type"] = courier["courier_type"].As<std::string>();
  courier_builder["regions"] = courier["regions"].As<std::vector<int>>();
  courier_builder["working_hours"] =
      courier["working_hours"].As<std::vector<std::string>>();
  int earn_multiplyer = 4;
  int raiting_multiplyer = 1;

  if (courier["courier_type"].As<std::string>() == "FOOT") {
    earn_multiplyer = 2;
    raiting_multiplyer = 3;
  } else if (courier["courier_type"].As<std::string>() == "BIKE") {
    earn_multiplyer = 3;
    raiting_multiplyer = 2;
  }
  courier_builder["earnings"] =
      earnings_res.Front()["total_cost"].As<int64_t>() * earn_multiplyer;
  courier_builder["rating"] =
      earnings_res.AsSingleRow<float>() * raiting_multiplyer;
  return ToString(courier_builder.ExtractValue());
}
void AppendOrdersHandler(userver::components::ComponentList& component_list) {
  component_list.Append<CourierInfoHandler>();
}
}  // namespace couriers_info_handler_template
