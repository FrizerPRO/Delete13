#include "hello.hpp"

#include <fmt/format.h>

#include <userver/clients/dns/component.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>

namespace pg_service_template {

namespace {
class Hello final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-hello";

  Hello(const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context)
      : HttpHandlerBase(config, component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()) {
    constexpr auto kCreateTable = R"~(
      CREATE TABLE IF NOT EXISTS key_value_table (
        key VARCHAR PRIMARY KEY,
        value INTEGER DEFAULT(1)
      )
    )~";

    using userver::storages::postgres::ClusterHostType;
    pg_cluster_->Execute(ClusterHostType::kMaster, kCreateTable);
  }

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const override {
    const auto& name = request.GetArg("name");

    int user_count = 0;
    if (!name.empty()) {
      auto result = pg_cluster_->Execute(
          userver::storages::postgres::ClusterHostType::kMaster,
          "INSERT INTO key_value_table VALUES($1, 1) "
          "ON CONFLICT (key) "
          "DO UPDATE SET value = key_value_table.value + 1 "
          "RETURNING value",
          name);

      user_count = result.AsSingleRow<int>();
    }
    return pg_service_template::SayHelloTo(name, user_count);
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

std::string SayHelloTo(std::string_view name, int user_count) {
  if (name.empty()) {
    name = "unknown user";
  }

  return fmt::format("Hello, {},your count is:{} \n",name,user_count);

  UASSERT(false);
}

void AppendHello(userver::components::ComponentList& component_list) {
  component_list.Append<Hello>();
  //component_list.Append<userver::components::Postgres>("postgres-db-1");
  //component_list.Append<userver::clients::dns::Component>();
}

}  // namespace pg_service_template
