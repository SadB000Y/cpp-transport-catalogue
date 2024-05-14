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

class TransportRouter
{
public:

    struct StopItem {
        std::string stop_name;
        double time;
    };

    struct BusItem {
        std::string bus_name;
        int span_count;
        double time;
    };

    using route = std::variant<StopItem, BusItem>;

    TransportRouter(const transport_catalogue::TransportCatalogue& catalogue, double speed, double wait_time) :
        catalogue_(catalogue), speed_(speed), wait_time_(wait_time)
        , graph_(BuildGraph()), router_(graph_)
    {  }



    std::optional<std::pair<std::vector<route>, double>> GetRoute(std::string_view from, std::string_view to) const {
        auto route_info = router_.BuildRoute(vertex_ids_.at(from), vertex_ids_.at(to));

        if (!route_info.has_value()) {
            return { };
        }
        std::vector<route> result;
        double weight = route_info.value().weight;
        for (auto& item : route_info.value().edges)

        {
            auto curr_edge = graph_.GetEdge(item);
            StopItem stop = { std::move(curr_edge.stop_name), wait_time_ };
            result.push_back(std::move(stop));
            BusItem bus = { std::move(curr_edge.bus_name), curr_edge.span_count, curr_edge.weight - wait_time_ };
            result.push_back(std::move(bus));

        }
        return { {result, weight} };
    }

private:
    DirectedWeightedGraph<double> BuildGraph();
    const transport_catalogue::TransportCatalogue& catalogue_;
    double speed_;
    double wait_time_;
    std::map<std::string_view, unsigned long> vertex_ids_;
    DirectedWeightedGraph<double> graph_;

    Router<double> router_;
};
