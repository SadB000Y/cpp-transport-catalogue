#pragma once

#include <variant>
#include <utility>
#include <vector>

#include "transport_catalogue.h"
#include "json.h"
#include "graph.h"
#include "numeric"
#include "optional"
#include "router.h"

template <typename Weight>
class TransportRouter
{
public:

    struct stop_item {
        std::string stop_name;
        double time;
    };

    struct bus_item {
        std::string bus_name;
        int span_count;
        double time;
    };

    using route = std::variant<stop_item, bus_item>;
    
    TransportRouter(const transport_catalogue::TransportCatalogue& catalogue, double speed, double wait_time) : 
                    catalogue_(catalogue), speed_(speed), wait_time_(wait_time)
                    , graph_(BuildGraph()), router_(graph_)
    { }
    
    std::optional<std::pair<std::vector<route>, double>> GetRoute(std::string_view from, std::string_view to) const {
        auto route_info =router_.BuildRoute(catalogue_.GetStopId(from), catalogue_.GetStopId(to));
        
        if (!route_info.has_value()) {
            return { };
        }
        std::vector<route> result;
        Weight weight = route_info.value().weight;
        for (auto& item : route_info.value().edges)

        {
            auto curr_edge = graph_.GetEdge(item);
            stop_item stop = { std::move(curr_edge.stop_name), wait_time_ };
            result.push_back(std::move(stop));
            bus_item bus = { std::move(curr_edge.bus_name), curr_edge.span_count, curr_edge.weight - wait_time_};
            result.push_back(std::move(bus));

        }
        return {{result, weight}};
    }
    
private:
    DirectedWeightedGraph<double> BuildGraph();
    const transport_catalogue::TransportCatalogue& catalogue_;
    double speed_;
    double wait_time_;
    DirectedWeightedGraph<double> graph_;

    Router<Weight> router_;
};
