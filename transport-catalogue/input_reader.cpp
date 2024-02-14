#include "input_reader.h"

#include <algorithm>
#include <cassert>
#include <iterator>

/**
 * Парсит строку вида "10.123,  -30.1837" и возвращает пару координат (широта, долгота)
 */
geo::Coordinates ParseCoordinates(std::string_view str) {
    static const double nan = std::nan("");

    auto not_space = str.find_first_not_of(' ');
    auto comma = str.find(',');

    if (comma == str.npos) {
        return {nan, nan};
    }

    auto not_space2 = str.find_first_not_of(' ', comma + 1);

    double lat = std::stod(std::string(str.substr(not_space, comma - not_space)));
    double lng = std::stod(std::string(str.substr(not_space2)));

    return {lat, lng};
}

/**
 * Удаляет пробелы в начале и конце строки
 */
std::string_view Trim(std::string_view string) {
    const auto start = string.find_first_not_of(' ');
    if (start == string.npos) {
        return {};
    }
    return string.substr(start, string.find_last_not_of(' ') + 1 - start);
}

/**
 * Разбивает строку string на n строк, с помощью указанного символа-разделителя delim
 */
std::vector<std::string_view> Split(std::string_view string, char delim) {
    std::vector<std::string_view> result;

    size_t pos = 0;
    while ((pos = string.find_first_not_of(' ', pos)) < string.length()) {
        auto delim_pos = string.find(delim, pos);
        if (delim_pos == string.npos) {
            delim_pos = string.size();
        }
        if (auto substr = Trim(string.substr(pos, delim_pos - pos)); !substr.empty()) {
            result.push_back(substr);
        }
        pos = delim_pos + 1;
    }

    return result;
}

/**
 * Парсит маршрут.
 * Для кольцевого маршрута (A>B>C>A) возвращает массив названий остановок [A,B,C,A]
 * Для некольцевого маршрута (A-B-C-D) возвращает массив названий остановок [A,B,C,D,C,B,A]
 */
std::vector<std::string_view> ParseRoute(std::string_view route) {
    if (route.find('>') != route.npos) {
        return Split(route, '>');
    }

    auto stops = Split(route, '-');
    std::vector<std::string_view> results(stops.begin(), stops.end());
    results.insert(results.end(), std::next(stops.rbegin()), stops.rend());

    return results;
}

CommandDescription ParseCommandDescription(std::string_view line) {
    auto colon_pos = line.find(':');
    if (colon_pos == line.npos) {
        return {};
    }

    auto space_pos = line.find(' ');
    if (space_pos >= colon_pos) {
        return {};
    }

    auto not_space = line.find_first_not_of(' ', space_pos);
    if (not_space >= colon_pos) {
        return {};
    }

    return {std::string(line.substr(0, space_pos)),
            std::string(line.substr(not_space, colon_pos - not_space)),
            std::string(line.substr(colon_pos + 1))};
}

void InputReader::ParseLine(std::string_view line) {
    auto command_description = ParseCommandDescription(line);
    if (command_description) {
        commands_.push_back(std::move(command_description));
    }
}

std::vector<std::pair<std::string_view, int>> ParseDistances(std::string_view str)
{
    std::vector<std::pair<std::string_view, int>> result;
    size_t pos = 0;
    size_t end_this_values = 0;
    pos = str.find_first_of(',', pos + 1); pos = str.find_first_of(',', pos + 1); pos += 1;
    std::string_view description_distances = Trim(str.substr(pos));
    pos = 0;

    while (end_this_values != description_distances.npos)
    {
        int distance = 0;
        std::string_view stopname;

        pos = description_distances.find_first_not_of(' ', pos);
        distance = std::stoi(std::string(description_distances.substr(pos, description_distances.find_first_of('m', pos))));
        pos = description_distances.find_first_of(' ', pos); pos = description_distances.find_first_of(' ', pos + 1);
        end_this_values = description_distances.find_first_of(',', pos);
        if (pos == description_distances.npos && end_this_values == description_distances.npos)
            return {};
        stopname = Trim(description_distances.substr(pos, (end_this_values - pos)));

        pos = end_this_values + 2;
        result.push_back({ std::move(stopname), std::move(distance) });
    }

    return result;
}

void InputReader::ApplyCommands([[maybe_unused]] catalogue::TransportCatalogue & catalogue) const {
    
    for (const auto& commands : commands_) 
        if (commands.command == "Stop") {
            catalogue::TransportCatalogue::BusStop new_stop = { commands.id, ParseCoordinates(commands.description) };
            auto distances = ParseDistances(commands.description);
            catalogue.AddStop(new_stop, std::move(distances));
        }
   
    for (const auto& commands : commands_)
        if (commands.command == "Bus") {
            //std::vector<std::string_view> route = ParseRoute(commands.description);
            catalogue.AddBus(commands.id, std::move(ParseRoute(commands.description)));
        }
        
}

void LoopForInput(catalogue::TransportCatalogue& catalogue, const int base_request_count, std::istream& input) {
    InputReader reader;
    for (int i = 0; i < base_request_count; ++i) {
        std::string line;
        std::getline(input, line);
        reader.ParseLine(line);
    }
    reader.ApplyCommands(catalogue);
}