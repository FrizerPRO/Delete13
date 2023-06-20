#include "OrdersHandler.h"

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

namespace orders_handler_template {
OrdersHandler::OrdersHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context)
    : HttpHandlerBase(config, component_context),
      pg_cluster_(
          component_context
              .FindComponent<userver::components::Postgres>("postgres-db-1")
              .GetCluster()) {
  constexpr auto kCreateTable = R"~(
      CREATE TABLE IF NOT EXISTS orders_table (
        order_id bigserial PRIMARY KEY,
        weight double precision,
        regions integer,
        delivery_hours text[],
        cost integer,
        completed_time timestamp
      )
    )~";

  using userver::storages::postgres::ClusterHostType;
  pg_cluster_->Execute(ClusterHostType::kMaster, kCreateTable);
}

std::string OrdersHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
  switch (request.GetMethod()) {
    case userver::server::http::HttpMethod::kPost:
      return OrdersHandler::PostOrders(request);
      break;
    case userver::server::http::HttpMethod::kGet:
      return OrdersHandler::GetOrders(request);
      break;
    default:
      break;
  }
  return "";
}
std::string OrdersHandler::PostOrders(
    const userver::server::http::HttpRequest& request) const {
  const userver::storages::postgres::Query kInsertValue{
      "INSERT INTO orders_table "
      "(weight,regions,delivery_hours,cost,completed_time) "
      "VALUES ($1, $2, $3, $4, NULL) "
      "ON CONFLICT DO NOTHING "
      "RETURNING *",
      userver::storages::postgres::Query::Name{"sample_insert_value"},
  };
  const userver::storages::postgres::Query kGetValues{
      "SELECT * FROM orders_table ",
      userver::storages::postgres::Query::Name{"sample_get_value"},
  };

  auto request_data = request.RequestBody();
  auto orderRequest = userver::formats::json::FromString(request_data)
                          .As<CreateOrderRequest::CreateOrderRequest>();
  userver::storages::postgres::Transaction transaction = pg_cluster_->Begin(
      "sample_transaction_insert_key_value",
      userver::storages::postgres::ClusterHostType::kMaster, {});
  std::vector<std::string> res_json;

  for (auto orderDto : orderRequest.orders) {
    auto res =
        transaction.Execute(kInsertValue, orderDto.weight, orderDto.regions,
                            orderDto.delivery_hours, orderDto.cost);
    if (!res.RowsAffected()) {
      request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
      return "";
    }
    auto row = res.Front();
    userver::formats::json::ValueBuilder builder;
    builder["order_id"] = row["order_id"].As<int64_t>();
    builder["weight"] = row["weight"].As<double>();
    builder["regions"] = row["regions"].As<int>();
    builder["delivery_hours"] =
        row["delivery_hours"].As<std::vector<std::string>>();
    builder["cost"] = row["cost"].As<int>();
    if (!row["completed_time"].IsNull()) {
      builder["completed_time"] =
          row["completed_time"].As<std::chrono::system_clock::time_point>();
    }
    res_json.push_back(ToString(builder.ExtractValue()));
  }
  transaction.Commit();
  
  return ToString(
      userver::formats::json::ValueBuilder(res_json).ExtractValue());
}
std::string OrdersHandler::GetOrders(
    const userver::server::http::HttpRequest& request) const {
  const userver::storages::postgres::Query kGetValues{
      "SELECT * FROM orders_table LIMIT $1 OFFSET $2",
      userver::storages::postgres::Query::Name{"sample_get_value"},
  };
  auto limit = 1;
  auto offset = 0;
  if (request.HasArg("offset")) {
    try {
      offset = std::stoi(request.GetArg("offset"));
    } catch (const std::exception&) {
    }
  }
  if (request.HasArg("limit")) {
    try {
      limit = std::stoi(request.GetArg("limit"));
    } catch (const std::exception&) {
    }
  }

  auto res =
      pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                           kGetValues, limit, offset);
  if (res.IsEmpty()) {
    request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
    return "";
  }
  std::vector<std::string> res_json;
  for (auto row : res) {
    userver::formats::json::ValueBuilder builder;
    builder["order_id"] = row["order_id"].As<int64_t>();
    builder["weight"] = row["weight"].As<double>();
    builder["regions"] = row["regions"].As<int>();
    builder["delivery_hours"] =
        row["delivery_hours"].As<std::vector<std::string>>();
    builder["cost"] = row["cost"].As<int>();
    if (!row["completed_time"].IsNull()) {
      builder["completed_time"] =
          row["completed_time"].As<std::chrono::system_clock::time_point>();
    }
    res_json.push_back(ToString(builder.ExtractValue()));
  }
  return ToString(
      userver::formats::json::ValueBuilder(res_json).ExtractValue());
}
void AppendOrdersHandler(userver::components::ComponentList& component_list) {
  component_list.Append<OrdersHandler>();
  component_list.Append<userver::components::Postgres>("postgres-db-1");
  component_list.Append<userver::clients::dns::Component>();
}

}  // namespace orders_handler_template
