#include "json_reader.h"
#include <sstream>
namespace transport_catalogue
{

    BusStops ParseBusInfo(json::Dict dict_bus_info)
    {
        std::vector<std::string> stops;
        bool is_roundtrip = true;
        for (auto &memb : dict_bus_info.at("stops").AsArray())
        {
            stops.push_back(memb.AsString());
        }
        if (!dict_bus_info.at("is_roundtrip").AsBool())
        {
            is_roundtrip = false;
            std::vector<std::string> temp_vect = stops;
            temp_vect.pop_back();
            std::reverse(temp_vect.begin(), temp_vect.end());
            for (auto &memb : temp_vect)
            {
                stops.push_back(move(memb));
            }
        }

        return {dict_bus_info.at("name").AsString(), stops, is_roundtrip};
    }

    Stop ParseStopInfo(json::Dict dict_stop_info)
    {
        std::unordered_map<std::string, int64_t> stops_to_dists;

        if (dict_stop_info.count("road_distances"))
        {
            for (auto [stop, dist] : dict_stop_info.at("road_distances").AsMap())
            {
                stops_to_dists.insert({stop, dist.AsInt()});
            }
        }
        geo::Coordinates coords = {dict_stop_info.at("latitude").AsDouble(), dict_stop_info.at("longitude").AsDouble()};

        return {dict_stop_info.at("name").AsString(), coords, stops_to_dists};
    }

    void ProcessRequest(std::istream &input, std::ostream &output, TransportCatalogue &catal)
    {

        auto doc = json::Load(input);
        json::Dict result_dict = doc.GetRoot().AsMap();

        json::Array base_req = result_dict.at("base_requests").AsArray();
        json::Array stat_req = result_dict.at("stat_requests").AsArray();
        json::Node rander_sett = result_dict.at("render_settings");
        SvgInfo properties = ParsePropLine(rander_sett);
        InputReader(base_req, catal);
        output << "[" << std::endl;
        AnswerRequests(stat_req, output, catal, properties);
        output << std::endl
               << "]" << std::endl;
    }

    void InputReader(json::Array base_req, TransportCatalogue &catal)
    {
        Requests requests;
        for (auto &node : base_req)
        {
            if (node.AsMap().at("type").AsString() == "Stop")
            {
                requests.stops_requests.push_back(node);
                catal.AddStop(ParseStopInfo(node.AsMap()));
            }
            else
            {
                requests.bus_requests.push_back(std::move(node));
            }
        }

        for (auto &stop_node : requests.stops_requests)
        {
            auto stop_info = ParseStopInfo(std::move(stop_node).AsMap());

            if (stop_info.stops_to_dists.size() != 0)
            {
                for (auto &memb : stop_info.stops_to_dists)
                {
                    catal.SetDistance(memb.second, catal.FindStop(stop_info.name), catal.FindStop(memb.first));
                }
            }
        }

        for (auto &bus_node : requests.bus_requests)
        {
            std::vector<Stop *> stops;
            auto bus_info = ParseBusInfo(std::move(bus_node).AsMap());
            for (auto &memb : bus_info.stops)
            {
                stops.push_back(catal.FindStop(memb));
            }
            catal.AddBus({bus_info.name, stops, bus_info.is_roundtrip});
            for (auto stop : stops)
            {
                catal.GetStopsToBusesMap()[stop].insert(std::string_view{catal.GetBusesDeque().back().name});
            }
        }
    }

    void AnswerRequests(const json::Array &stat_req, std::ostream &output, TransportCatalogue &catal, SvgInfo &properties)
    {
        bool isfirstreq = true;
        for (auto &request_node : stat_req)
        {

            if (request_node.AsMap().at("type").AsString() == "Bus")
            {
                if (isfirstreq)
                {
                    json::Dict result = FormBusDataJSON(request_node, catal);
                    json::Print(json::Document{result}, output);
                    isfirstreq = false;
                }
                else
                {
                    output << "," << std::endl;
                    json::Dict result = FormBusDataJSON(request_node, catal);
                    json::Print(json::Document{result}, output);
                }
            }
            else if (request_node.AsMap().at("type").AsString() == "Stop")
            {
                if (isfirstreq)
                {
                    json::Dict result = FormStopDataJSON(request_node, catal);
                    json::Print(json::Document{result}, output);
                    isfirstreq = false;
                }
                else
                {
                    output << "," << std::endl;
                    json::Dict result = FormStopDataJSON(request_node, catal);
                    json::Print(json::Document{result}, output);
                }
            }
            else
            {

                svg::Document &result_doc = (FormSVGDocument(catal, properties));
                std::ostringstream out;
                result_doc.Render(out);
                std::string map_rend_string = out.str();
                json::Dict result = FormMapDataJson(request_node, map_rend_string);
                if (isfirstreq)
                {
                    json::Print(json::Document{result}, output);
                    isfirstreq = false;
                }
                else
                {
                    output << "," << std::endl;
                    json::Print(json::Document{result}, output);
                }
            }
        }
    }

    json::Dict FormMapDataJson(json::Node request_node, const std::string &map_rend_string)
    {
        json::Dict result;
        double request_id = request_node.AsMap().at("id").AsDouble();
        result["request_id"] = request_id;
        result["map"] = map_rend_string;
        return result;
    }

    json::Dict FormBusDataJSON(json::Node request_node, TransportCatalogue &catal)
    {
        json::Dict result;
        std::string name_of_the_bus = request_node.AsMap().at("name").AsString();
        double request_id = request_node.AsMap().at("id").AsDouble();
        result["request_id"] = request_id;

        if (catal.FindBus(name_of_the_bus))
        {
            auto data_of_bus = catal.GetBusInfo(name_of_the_bus);
            result["stop_count"] = static_cast<int>(data_of_bus.numb_of_stops);
            result["unique_stop_count"] = static_cast<int>(data_of_bus.numb_of_unique_stops);
            result["route_length"] = static_cast<int>(data_of_bus.true_length);
            result["curvature"] = static_cast<double>(data_of_bus.true_length / data_of_bus.length);

            return result;
        }
        else
        {
            result["error_message"] = "not found";
            return result;
        }
    }

    json::Dict FormStopDataJSON(json::Node request_node, TransportCatalogue &catal)
    {
        json::Dict result;
        std::string name_of_the_stop = request_node.AsMap().at("name").AsString();
        double request_id = request_node.AsMap().at("id").AsDouble();
        result["request_id"] = request_id;
        if (catal.FindStop(name_of_the_stop))
        {
            json::Array pas_bus;
            for (auto memb : catal.GetStopInfo(name_of_the_stop).passing_buses)
            {
                pas_bus.push_back(std::string{memb});
            }
            result["buses"] = pas_bus;
            return result;
        }
        else
        {
            result["error_message"] = "not found";
            return result;
        }
    }

}