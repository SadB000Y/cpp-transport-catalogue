#pragma once
#include <string>
#include <set>
#include <vector>
#include <unordered_map>
#include "geo.h"
#include "json.h"

namespace transport_catalogue
{

    struct BusStops
    {
        std::string name;
        std::vector<std::string> stops;
        bool is_roundtrip;
    };

    struct Stop
    {
        std::string name;
        geo::Coordinates coordinates;
        std::set<std::string> stop_to_dist;
    };

    struct Buses
    {
        std::string name;
        std::vector<const Stop*> stops;
        bool is_roundtrip;
    };

    struct BusesInfo
    {
        size_t numb_of_stops;
        size_t numb_of_unique_stops;
        double length;
        double true_length;
    };

    struct StopInfo
    {
        std::set<std::string_view> passing_buses;
    };

    enum ReqType {
        BUS,
        STOP
    };

    struct RouteOutput
    {
        std::string name;// bus or stop_name
        double time;
        int span_count;
        ReqType req_type;
    };

    struct StopDistancesHasher {
        size_t operator()(const std::pair<const Stop*, const Stop*>& points) const {
            size_t hash_first = std::hash<const void*>{}(points.first);
            size_t hash_second = std::hash<const void*>{}(points.second);
            return hash_first + hash_second * 37;
        }
    };

    /*
     * В этом файле вы можете разместить классы/структуры, которые являются частью предметной области (domain)
     * вашего приложения и не зависят от транспортного справочника. Например Автобусные маршруты и Остановки.
     *
     * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный
     * заголовочный файл может оказаться полезным, когда дело дойдёт до визуализации карты маршрутов:
     * визуализатор карты (map_renderer) можно будет сделать независящим от транспортного справочника.
     *
     * Если структура вашего приложения не позволяет так сделать, просто оставьте этот файл пустым.
     *
     */
}

struct Requests
{
    json::Array stops_requests;
    json::Array bus_requests;
};