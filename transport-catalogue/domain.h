#pragma once
#include <string>
#include <set>
#include <vector>
#include <unordered_map>
#include "geo.h"
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
        std::unordered_map<std::string, int64_t> stops_to_dists;
    };

    struct Buses
    {
        std::string name;
        std::vector<Stop *> stops;
        bool is_roundtrip;
    };

    struct BusesInfo
    {
        size_t numb_of_stops;
        size_t numb_of_unique_stops;
        double length;
        int64_t true_length;
    };

    struct StopInfo
    {
        std::set<std::string_view> passing_buses;
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