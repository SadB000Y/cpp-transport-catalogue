#pragma once

#include "json.h"
#include "domain.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "json_builder.h"
#include "transport_router.h"
#include "graph.h"
#include "router.h"
namespace transport_catalogue
{
    BusStops ParseBusInfo(json::Dict dict_bus_info);
    Stop ParseStopInfo(json::Dict dict_stop_info);

    struct Requests
    {
        json::Array stops_requests;
        json::Array bus_requests;
    };

    void InputReader(json::Array base_req, TransportCatalogue& catalogue);
    void AnswerRequests(const json::Array& stat_req, std::ostream& output, TransportCatalogue& catalogue, SvgInfo& properties, json::Node route_prop);

    void ProcessRequest(std::istream& input, std::ostream& output, TransportCatalogue& catalogue);

    json::Dict FormBusDataJSON(json::Node request_node, TransportCatalogue& catalogue);
    json::Dict FormStopDataJSON(json::Node request_node, TransportCatalogue& catalogue);
    json::Dict FormMapDataJson(json::Node request_node, const std::string& map_rend_string);
    json::Dict FormRouteDataJSON(json::Node request_node, TransportRouter& transport_router);

    /*
     * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
     * а также код обработки запросов к базе и формирование массива ответов в формате JSON
     */

}