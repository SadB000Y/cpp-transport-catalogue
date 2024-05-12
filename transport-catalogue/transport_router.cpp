#include "transport_router.h"

template <>
DirectedWeightedGraph<double> TransportRouter<double>::BuildGraph()
{
    DirectedWeightedGraph<double> result(catalogue_.GetStopsCount());
    const auto curr_buses = catalogue_.GetBusesDequeConst();
    for (auto& bus : curr_buses)
    {
        const auto curr_stops = bus.stops;

        if (bus.is_roundtrip)
        {
            for (int count_first = 0; count_first < curr_stops.size() - 1; count_first++)
            {
                for (int count_second = 1 + count_first; count_second <= curr_stops.size() - 1; count_second++)
                {
                    double total_dist = 0;
                    if (count_second - count_first == 1)
                    {
                        total_dist = catalogue_.GetDistance(curr_stops[count_first], curr_stops[count_second]);
                    }
                    else
                    {
                        std::vector<transport_catalogue::Stop*> temp_cont = { curr_stops.begin() + count_first, curr_stops.begin() + count_second + 1 };
                        for (auto it = temp_cont.begin(); it != temp_cont.end() - 1; it++)
                        {
                            total_dist += catalogue_.GetDistance(*it, *(it + 1));
                        }
                    }
                    result.AddEdge({ catalogue_.GetStopId(curr_stops[count_first]->name),
                                    catalogue_.GetStopId(curr_stops[count_second]->name),
                                    wait_time_ + total_dist / speed_ * 1.0,
                                    bus.name,
                                    curr_stops[count_first]->name,
                                    std::abs(count_second - count_first) });
                }
            }
        }

        else
        {
            for (int count_first = 0; count_first <= curr_stops.size() / 2; count_first++)
            {
                for (int count_second = 0; count_second <= curr_stops.size() / 2; count_second++)
                {
                    double total_dist = 0;
                    if (std::abs(count_second - count_first) <= 1)
                    {
                        total_dist = catalogue_.GetDistance(curr_stops[count_first], curr_stops[count_second]);
                    }

                    else
                    {
                        if (count_first > count_second)
                        {
                            for (auto it = curr_stops.begin() + count_first; it > curr_stops.begin() + count_second; it--)
                            {
                                total_dist += catalogue_.GetDistance(*it, *(it - 1));
                            }
                        }

                        else
                        {
                            std::vector<transport_catalogue::Stop*> temp_cont = { curr_stops.begin() + count_first, curr_stops.begin() + count_second + 1 };
                            for (auto it = temp_cont.begin(); it != temp_cont.end() - 1; it++)
                            {
                                total_dist += catalogue_.GetDistance(*it, *(it + 1));
                            }
                        }
                    }

                    result.AddEdge({ catalogue_.GetStopId(curr_stops[count_first]->name),
                                    catalogue_.GetStopId(curr_stops[count_second]->name),
                                    wait_time_ + total_dist / speed_ * 1.0,
                                    bus.name,
                                    curr_stops[count_first]->name,
                                    std::abs(count_second - count_first) });
                }
            }
        }
    }
    return result;
}