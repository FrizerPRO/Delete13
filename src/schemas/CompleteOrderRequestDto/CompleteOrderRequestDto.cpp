#include "CompleteOrderRequestDto.h"

namespace CompleteOrderRequestDto {
    CompleteOrderRequestDto Parse(const userver::formats::json::Value& json,
               userver::formats::parse::To<CompleteOrderRequestDto>){
                CompleteOrderRequestDto res;
                res.complete_info = json["complete_info"].As<std::vector<CompleteOrder::CompleteOrder>>();
                return res;
               }
}  // namespace CompleteOrderRequestDto
