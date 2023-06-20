#include <string>
#include <string_view>

#include <userver/components/component_list.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/formats/json/value.hpp>

namespace couriers_handler_template {

class CouriersHandler final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-couriers";
  CouriersHandler(const userver::components::ComponentConfig& config,
                const userver::components::ComponentContext& component_context);
      std::string HandleRequestThrow(
          const userver::server::http::HttpRequest& request,
          userver::server::request::RequestContext&) const override;

  std::string PostCouriers(const userver::server::http::HttpRequest& request) const;
  std::string GetCouriers(const userver::server::http::HttpRequest& request) const;
  userver::storages::postgres::ClusterPtr pg_cluster_;
};
void AppendOrdersHandler(userver::components::ComponentList& component_list);
}  // namespace orders_handler_template
