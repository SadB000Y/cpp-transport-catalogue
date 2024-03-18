#pragma once

#include "domain.h"
#include "transport_catalogue.h"
#include "map_renderer.h"

namespace transport_catalogue
{

    class JSONreader{

    public:

        BusStops ParseBusInfo(const json::Dict& dict_bus_info);
        Stop ParseStopInfo(const json::Dict& dict_stop_info);

        void InputReader(json::Array base_req, TransportCatalogue& catal);
        void AnswerRequests(const json::Array& stat_req, std::ostream& output, TransportCatalogue& catal, SvgInfo& properties);

        void ProcessRequest(std::istream& input, std::ostream& output, TransportCatalogue& catal);

        json::Dict ExecuteBusRequest(json::Node request_node, TransportCatalogue& catal);
        json::Dict ExecuteStopRequest(json::Node request_node, TransportCatalogue& catal);
        json::Dict ExecuteMapRequest(json::Node request_node, const std::string& map_rend_string);

        /*
         * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
         * а также код обработки запросов к базе и формирование массива ответов в формате JSON
         */
    };
    

}