#pragma once
#include "transport_catalogue.h"
#include "json.h"
#include "graph.h"
#include "numeric"
class TransportRouter
{

public:
    TransportRouter(const transport_catalogue::TransportCatalogue& catalogue, double speed, double wait_time) : catalogue_(catalogue), speed_(speed), wait_time_(wait_time)
    {
        graph_ = BuildGraph();
    }

    const DirectedWeightedGraph<double>& GetGraph()
    {
        return graph_;
    }

private:
    DirectedWeightedGraph<double> BuildGraph();
    const transport_catalogue::TransportCatalogue& catalogue_;
    double speed_;
    double wait_time_;
    DirectedWeightedGraph<double> graph_;
};