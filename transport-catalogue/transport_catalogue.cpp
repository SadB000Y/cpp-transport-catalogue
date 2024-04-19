#include "transport_catalogue.h"
#include "geo.h"
#include <set>
#include <iostream>

namespace transport_catalogue
{
    void TransportCatalogue::AddStop(const Stop& stop)
    {
        stops_.push_back(stop);
        Stop* stop_ptr = &stops_.back();
        stopname_to_stop_.insert({ std::string_view{stops_.back().name}, stop_ptr });
        stop_name_to_id[std::string_view{ stops_.back().name }] = stop_count++;

        id_to_stopname[stop_name_to_id[stop.name]] = stop.name;
    }

    Stop* TransportCatalogue::FindStop(const std::string& name_of_stop)
    {
        if (stopname_to_stop_.count(name_of_stop))
        {
            return stopname_to_stop_.at(name_of_stop);
        }
        else
        {
            return nullptr;
        }
    }

    void TransportCatalogue::AddBus(const Buses& bus)
    {
        buses_.push_back(bus);
        Buses* bus_ptr = &buses_.back();
        busname_to_bus_.insert({ std::string_view{buses_.back().name}, bus_ptr });
        roundtrip_of_buses.insert({ std::string_view{buses_.back().name}, bus.is_roundtrip });

        for (auto& stop : bus_ptr->stops)
        {
            unique_stops.insert(std::string_view{ stop->name });
        }
    }

    Buses* TransportCatalogue::FindBus(const std::string& name_of_bus)
    {
        if (busname_to_bus_.count(name_of_bus))
        {
            return busname_to_bus_.at(name_of_bus);
        }
        else
            return nullptr;
    }

    template <typename Type>
    std::set<Type> MakeSet(std::vector<Type>& query_words)
    {
        std::set<Type> s(query_words.begin(), query_words.end());
        return s;
    }

    BusesInfo TransportCatalogue::GetBusInfo(const std::string& name_of_bus)
    {
        auto bus_address = busname_to_bus_.at(name_of_bus);
        size_t numb_of_stops = bus_address->stops.size();
        size_t numb_of_unique_stops = MakeSet(bus_address->stops).size();
        auto& stops_vect = bus_address->stops;
        double result_dist = 0;
        int64_t real_dist = 0;
        for (size_t count = 0; count < stops_vect.size() - 1; count++)
        {
            result_dist += geo::ComputeDistance(stops_vect[count]->coordinates, stops_vect[count + 1]->coordinates);
            real_dist += GetDistance(stops_vect[count], stops_vect[count + 1]);
        }
        return { numb_of_stops, numb_of_unique_stops, result_dist, real_dist };
    }

    StopInfo TransportCatalogue::GetStopInfo(const std::string& name_of_stop)
    {
        if (stopname_to_buses.count(stopname_to_stop_.at(std::string_view{ name_of_stop })))
        {
            return { stopname_to_buses.at(stopname_to_stop_.at(std::string_view{name_of_stop})) };
        }
        else
        {
            return StopInfo{};
        }
    }

    std::unordered_map<Stop*, std::set<std::string_view>>& TransportCatalogue::GetStopsToBusesMap()
    {
        return stopname_to_buses;
    }

    std::deque<Buses>& TransportCatalogue::GetBusesDeque()
    {
        return buses_;
    }

    const std::deque<Buses>& TransportCatalogue::GetBusesDequeConst() const
    {
        return buses_;
    }

    void TransportCatalogue::SetDistance(int64_t dist, Stop* from, Stop* to)
    {
        dist_betw_stops_[{from, to}] = dist;
    }

    int64_t TransportCatalogue::GetDistance(Stop* from, Stop* to) const
    {
        if ((from == to) && (!dist_betw_stops_.count({ from, to })))
        {
            return 0;
        }
        if (dist_betw_stops_.count({ from, to }))
        {
            return dist_betw_stops_.at({ from, to });
        }
        else
        {
            return dist_betw_stops_.at({ to, from });
        }
    }

    const std::deque<Buses>& TransportCatalogue::GetAllBuses()
    {
        return buses_;
    }

    std::set<std::string_view>& TransportCatalogue::GetUniqueStops()
    {
        return unique_stops;
    }

    size_t TransportCatalogue::GetStopId(std::string_view stop_name) const
    {
        return stop_name_to_id.at(stop_name);
    }

    std::string TransportCatalogue::GetStopFromId(size_t stop_id) const
    {
        return id_to_stopname.at(stop_id);
    }

    size_t TransportCatalogue::GetStopsCount() const
    {
        return stops_.size();
    }

}