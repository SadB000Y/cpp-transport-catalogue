#pragma once

#include "transport_catalogue.h"
#include "json.h"
#include "graph.h"
#include "numeric"
#include "optional"
#include "router.h"

// Я Я постарался поработать ещё над всеми замечаниями, убрал лишнюю функцию из каталога, 
// но с роутером ничего не могу придумать от слова совсем, по-другому не вижу решение этой задачи :(

class TransportRouter
{

public:
    TransportRouter(const transport_catalogue::TransportCatalogue& catalogue, double speed, double wait_time) : 
                    catalogue_(catalogue), speed_(speed), wait_time_(wait_time)
    {
        graph_ = BuildGraph();
    }

    const DirectedWeightedGraph<double>& GetGraph() const {
        return graph_;
    }

    template <typename Weight>
    std::optional<typename Router<Weight>::RouteInfo> GetRoute(VertexId from, VertexId to) const {
        Router<Weight> router(&graph_);
        return router.BuildRoute(from, to);
    }

private:
    DirectedWeightedGraph<double> BuildGraph();
    const transport_catalogue::TransportCatalogue& catalogue_;
    double speed_;
    double wait_time_;
    DirectedWeightedGraph<double> graph_;

};
