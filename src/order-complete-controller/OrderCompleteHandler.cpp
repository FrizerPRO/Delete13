#include "OrderCompleteHandler.h"

#include <fmt/format.h>

#include <userver/clients/dns/component.hpp>
#include <userver/formats/parse/time_of_day.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/formats/serialize/time_of_day.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>
#include <vector>
#include "../schemas/CompleteOrderRequestDto/CompleteOrderRequestDto.h"
namespace orders_complete_handler_template {

OrdersCompleteHandler::OrdersCompleteHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context)
    : HttpHandlerBase(config, component_context),
      pg_cluster_(
          component_context
              .FindComponent<userver::components::Postgres>("postgres-db-1")
              .GetCluster()) {
  using userver::storages::postgres::ClusterHostType;
  constexpr auto kCreateTable = R"~(
    CREATE TABLE IF NOT EXISTS orders_to_couriers (
        courier_id bigint,
        order_id bigint PRIMARY KEY,
        completed_time timestamp
      )
    )~";
  pg_cluster_->Execute(ClusterHostType::kMaster, kCreateTable);
}

std::string OrdersCompleteHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
  auto order_reqiest =
      userver::formats::json::FromString(request.RequestBody())
          .As<CompleteOrderRequestDto::CompleteOrderRequestDto>();
  const userver::storages::postgres::Query kGetCourier{
      "SELECT * FROM couriers_table "
      "WHERE courier_id = $1",
      userver::storages::postgres::Query::Name{"sample_get_value"},
  };
  const userver::storages::postgres::Query kGetOrder{
      "SELECT * FROM orders_table "
      "WHERE order_id = $1",
      userver::storages::postgres::Query::Name{"sample_get_value"},
  };
  const userver::storages::postgres::Query kGetCourierToOrder{
      "SELECT * FROM orders_to_couriers "
      "WHERE order_id = $1",
      userver::storages::postgres::Query::Name{"sample_get_value"},
  };
  const userver::storages::postgres::Query kInsertValue{
      "INSERT INTO orders_to_couriers "
      "(courier_id,order_id,completed_time) "
      "VALUES ($1, $2 , $3) "
      "ON CONFLICT DO NOTHING ",
      userver::storages::postgres::Query::Name{"sample_insert_value"},
  };
  const userver::storages::postgres::Query kUpdateValue{
      "UPDATE orders_table "
      "SET  completed_time = $2 "
      "WHERE order_id = $1 "
      "RETURNING *",
      userver::storages::postgres::Query::Name{"sample_insert_value"},
  };
  userver::storages::postgres::Transaction transaction = pg_cluster_->Begin(
      "find_couriers_and_orders",
      userver::storages::postgres::ClusterHostType::kMaster, {});
  std::vector<std::string> res_json;
  for (auto order : order_reqiest.complete_info) {
    auto found_courier = transaction.Execute(kGetCourier, order.courier_id);
    if (found_courier.IsEmpty()) {
      LOG_DEBUG()<<"Didnt find courier";
      transaction.Rollback();
      request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
      return "";
    }
    auto found_order = transaction.Execute(kGetOrder, order.order_id);
    if (found_order.IsEmpty()) {
            LOG_DEBUG()<<"Didnt find order";
      transaction.Rollback();
      request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
      return "";
    }
    auto found_order_to_courier =
        transaction.Execute(kGetCourierToOrder, order.order_id);
    if (!found_order_to_courier.IsEmpty()) {
      LOG_DEBUG()<<"Order already in use";
      transaction.Rollback();
      request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
      return "";
    }
    auto inserted_layer =
        transaction.Execute(kInsertValue, order.courier_id, order.order_id, order.complete_time);
    if (!inserted_layer.RowsAffected()) {
      
      transaction.Rollback();
      request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
      return "";
    }

    auto updated_layer =
        transaction.Execute(kUpdateValue, order.order_id, order.complete_time);
    if (!updated_layer.RowsAffected()) {
      transaction.Rollback();
      request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
      return "";
    }
    auto row = updated_layer.Front();
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
void AppendOrdersHandler(userver::components::ComponentList& component_list) {
  component_list.Append<OrdersCompleteHandler>();
}
}  // namespace orders_complete_handler_template
