#include "CourierNumberHandler.h"

#include <fmt/format.h>

#include <userver/clients/dns/component.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/formats/parse/time_of_day.hpp>
#include <userver/formats/serialize/time_of_day.hpp>
#include <vector>

namespace couriers_naumber_handler_template {

  CouriersNumberHandler::CouriersNumberHandler(const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context)
      : HttpHandlerBase(config, component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()){}

  std::string CouriersNumberHandler::HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const {
        const userver::storages::postgres::Query kGetValues{
        "SELECT * FROM couriers_table "
        "WHERE courier_id = $1",
        userver::storages::postgres::Query::Name{"sample_get_value"},
    };
    auto order_id = userver::formats::json::FromString(request.GetPathArg("id")).As<int64_t>();
      auto res = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave,kGetValues,order_id);
    if (res.IsEmpty()) {
      request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
      return "";
    }
    auto row = res.Front();
    request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
    userver::formats::json::ValueBuilder builder;
    builder["courier_id"] = row["courier_id"].As<int64_t>();
    builder["courier_type"] =
        row["courier_type"].As<std::string>();
    builder["regions"] = row["regions"].As<std::vector<int>>();
    builder["working_hours"] =
        row["working_hours"].As<std::vector<std::string>>();
    return ToString(builder.ExtractValue());
  }
void AppendOrdersHandler(userver::components::ComponentList& component_list) {
  component_list.Append<CouriersNumberHandler>();
}
}  // namespace orders_handler_template
