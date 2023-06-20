#include "OrdersNumberHandler.h"

#include <fmt/format.h>

#include <userver/clients/dns/component.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>

#include <userver/formats/parse/time_of_day.hpp>
#include <userver/formats/serialize/time_of_day.hpp>
#include <vector>

#include "../schemas/CreateOrderRequest/CreateOrderRequest.h"
#include "../schemas/OrderDto/OrderDto.h"


namespace orders_number_handler_template {

  OrdersNumberHandler::OrdersNumberHandler(const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context)
      : HttpHandlerBase(config, component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()){}

  std::string OrdersNumberHandler::HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const {
        const userver::storages::postgres::Query kGetValues{
        "SELECT * FROM orders_table "
        "WHERE order_id = $1",
        userver::storages::postgres::Query::Name{"sample_get_value"},
    };
    auto order_id = userver::formats::json::FromString(request.GetPathArg("id")).As<int64_t>();
      auto res = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave,kGetValues,order_id);
    if (res.IsEmpty()) {
      request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
      return "";
    }
    auto row = res.Front();
    request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);
    userver::formats::json::ValueBuilder builder;
    builder["order_id"] = row["order_id"].As<int64_t>();
    builder["weight"] = row["weight"].As<double>();
    builder["regions"] = row["regions"].As<int>();
    builder["delivery_hours"] = row["delivery_hours"].As<std::vector<std::string>>();
    builder["cost"] = row["cost"].As<int>();
    if(!row["completed_time"].IsNull()){
          builder["completed_time"] = row["completed_time"].As<std::chrono::system_clock::time_point>();
    }
    return ToString(builder.ExtractValue());
  }
void AppendOrdersHandler(userver::components::ComponentList& component_list) {
  component_list.Append<OrdersNumberHandler>();
}
}  // namespace orders_handler_template
