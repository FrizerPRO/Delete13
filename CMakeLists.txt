cmake_minimum_required(VERSION 3.12)
project(pg_service_template CXX)

include(third_party/userver/cmake/SetupEnvironment.cmake)
include(GNUInstallDirs)

add_subdirectory(third_party/userver)


# Common sources
add_library(${PROJECT_NAME}_objs OBJECT
    src/hello.hpp
    src/hello.cpp
	src/courier-controller/CourierHandler.h
	src/courier-controller/CourierHandler.cpp
	src/courier-number-controller/CourierNumberHandler.h
	src/courier-number-controller/CourierNumberHandler.cpp
	src/couriers-info/CourierInfoHandler.h
	src/couriers-info/CourierInfoHandler.cpp
	src/order-complete-controller/OrderCompleteHandler.h
	src/order-complete-controller/OrderCompleteHandler.cpp
	src/order-number-controller/OrdersNumberHandler.h
	src/order-number-controller/OrdersNumberHandler.cpp
	src/order-controller/OrdersHandler.h
	src/order-controller/OrdersHandler.cpp
	src/schemas/CreateOrderDto/CreateOrderDto.cpp
	src/schemas/CreateOrderDto/CreateOrderDto.h
	src/schemas/CreateOrderRequest/CreateOrderRequest.cpp
	src/schemas/CreateOrderRequest/CreateOrderRequest.h
	src/schemas/OrderDto/OrderDto.cpp
	src/schemas/OrderDto/OrderDto.h
	src/schemas/GroupOrders/GroupOrders.cpp
	src/schemas/GroupOrders/GroupOrders.h
	src/schemas/CouriersGroupOrders/CouriersGroupOrders.cpp
	src/schemas/CouriersGroupOrders/CouriersGroupOrders.h
	src/schemas/OrderAssignResponse/OrderAssignResponse.cpp
	src/schemas/OrderAssignResponse/OrderAssignResponse.h
	src/schemas/CompleteOrder/CompleteOrder.cpp
	src/schemas/CompleteOrder/CompleteOrder.h
	src/schemas/CompleteOrderRequestDto/CompleteOrderRequestDto.cpp
	src/schemas/CompleteOrderRequestDto/CompleteOrderRequestDto.h
	src/schemas/CreateCourierDto/CreateCourierDto.cpp
	src/schemas/CreateCourierDto/CreateCourierDto.h
	src/schemas/CreateCourierRequest/CreateCourierRequest.cpp
	src/schemas/CreateCourierRequest/CreateCourierRequest.h
	src/schemas/CourierDto/CourierDto.cpp
	src/schemas/CourierDto/CourierDto.h
	src/schemas/CreateCouriersResponse/CreateCouriersResponse.cpp
	src/schemas/CreateCouriersResponse/CreateCouriersResponse.h
	src/schemas/GetCouriersResponse/GetCouriersResponse.cpp
	src/schemas/GetCouriersResponse/GetCouriersResponse.h
	src/schemas/GetCourierMetaInfoResponse/GetCourierMetaInfoResponse.cpp
	src/schemas/GetCourierMetaInfoResponse/GetCourierMetaInfoResponse.h
)
target_link_libraries(${PROJECT_NAME}_objs PUBLIC userver-core userver-postgresql)


# The Service
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_objs)


# Unit Tests
add_executable(${PROJECT_NAME}_unittest
    src/hello_test.cpp
)
target_link_libraries(${PROJECT_NAME}_unittest PRIVATE ${PROJECT_NAME}_objs userver-utest)
add_google_tests(${PROJECT_NAME}_unittest)


# Benchmarks
add_executable(${PROJECT_NAME}_benchmark
	src/hello_benchmark.cpp
)
target_link_libraries(${PROJECT_NAME}_benchmark PRIVATE ${PROJECT_NAME}_objs userver-ubench)
add_google_benchmark_tests(${PROJECT_NAME}_benchmark)

# Functional Tests
add_subdirectory(tests)

if(DEFINED ENV{PREFIX})
	message(STATUS "Set install prefix: $ENV{PREFIX}")
	file(TO_CMAKE_PATH "$ENV{PREFIX}" PREFIX_PATH)
	set(CMAKE_INSTALL_PREFIX ${PREFIX_PATH})
endif()

set(CONFIG_VAR_PATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME}/config_vars.yaml)
set(CONFIG_FALLBACK_PATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME}/dynamic_config_fallback.json)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configs/static_config.yaml.in ${CMAKE_CURRENT_SOURCE_DIR}/configs/static_config.yaml)

FILE(GLOB CONFIGS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.yaml ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.json)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${PROJECT_NAME})
install(FILES ${CONFIGS_FILES} DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME} COMPONENT ${PROJECT_NAME})
