#include "transport_catalogue.h"
#include "geo.h"

template <typename Type>
std::set<Type> MakeSet(const std::vector<Type>& query_words)
{
    std::set<Type> s(query_words.begin(), query_words.end());
    return s;
}

void catalogue::TransportCatalogue::AddStop(const BusStop& stop, const std::vector<std::pair<std::string_view, int>> distances) {
	
	stops_.push_back(stop);
	name_to_stop_.insert({ stops_.back().name_of_stop, &stops_.back() });

	std::string_view from = stops_.back().name_of_stop;
	if (!distances.empty()) {
		for (auto& [to, dist] : distances) {
			distances_.push_back(std::string(to));
			stops_distance_[{ from, distances_.back() }] = dist;
		}

	}
}

void catalogue::TransportCatalogue::AddBus(const std::string& id, const std::vector<std::string_view>& routes) {
	std::vector<BusStop*> route;

	for (size_t i = 0; i < routes.size(); ++i)
	{
		auto stop = routes[i];
		if (name_to_stop_.find(stop) != name_to_stop_.end())
		{
			route.push_back(name_to_stop_[stop]);
			name_to_stop_[stop]->buses_to_stop_.insert(id);
		}
		else
		{
			route.push_back(nullptr);
		}
	}

	buses_.push_back({ std::string(id), route });
	name_to_bus_.insert({ buses_.back().name_of_bus, &buses_.back() });
}

size_t catalogue::TransportCatalogue::UniqueStopsCount(std::string_view bus_number) const {
	std::unordered_set<std::string_view> unique_stops;
	for (const auto& stop : name_to_bus_.at(std::string(bus_number))->bus_stops) {
		unique_stops.insert(stop->name_of_stop);
	}
	return unique_stops.size();
}

double catalogue::TransportCatalogue::LengthCount(std::string_view bus_number) const {
	double result = 0;
	auto stops_vect = name_to_bus_.at(std::string(bus_number))->bus_stops;
	for (size_t count = 0; count < stops_vect.size() - 1; count++)
	{
		result += ComputeDistance(stops_vect[count]->coords, stops_vect[count + 1]->coords);
	}
	return result;
}

catalogue::TransportCatalogue::BusesInfo catalogue::TransportCatalogue::GetBusInfo(std::string_view bus_name) const {
	
	if (name_to_bus_.find(std::string(bus_name)) != name_to_bus_.end()) {
		
		return { name_to_bus_.at(std::string(bus_name))->bus_stops.size(), UniqueStopsCount(bus_name), std::get<0>(ComputeCurvature(bus_name)), std::get<1>(ComputeCurvature(bus_name)) };

	}
	else
		return { 0, 0, 0, 0};
}

catalogue::TransportCatalogue::StopsInfo catalogue::TransportCatalogue::GetStopInfo(std::string_view req_stop) const {

	std::vector<std::string> result;

	if (name_to_stop_.find(std::string(req_stop)) != name_to_stop_.end()) {

		if (name_to_stop_.at(req_stop)->buses_to_stop_.empty())
			return { "", {"no buses"}};
		else
		{
			for (auto& bus : name_to_stop_.at(req_stop)->buses_to_stop_)
			{
				result.push_back(std::string(bus));
			}
		}

	}
	else
		return { "", {"stop not found"}};
	return { req_stop, result };
}

std::tuple<double, double> catalogue::TransportCatalogue::ComputeCurvature(std::string_view name_of_bus) const {

	double curvature = 0.0;
	double total_metres_distance = 0;

	if (name_to_bus_.find(std::string(name_of_bus)) != name_to_bus_.end()) {

		double total_geo_distance = 0.0;
		geo::Coordinates from = { 0.0, 0.0 };
		geo::Coordinates to = { 0.0, 0.0 };

		for (size_t i = 0; i < name_to_bus_.at(name_of_bus)->bus_stops.size(); ++i)
		{
			from = name_to_bus_.at(name_of_bus)->bus_stops[i]->coords;
			if (i + 1 < name_to_bus_.at(name_of_bus)->bus_stops.size())
			{
				to = name_to_bus_.at(name_of_bus)->bus_stops[i + 1]->coords;
				total_geo_distance += ComputeDistance(from, to);

				auto it = stops_distance_.find({ name_to_bus_.at(name_of_bus)->bus_stops[i]->name_of_stop, name_to_bus_.at(name_of_bus)->bus_stops[i + 1]->name_of_stop });
				if (it != stops_distance_.end())
					total_metres_distance += it->second;
				else
				{
					it = stops_distance_.find({ name_to_bus_.at(name_of_bus)->bus_stops[i + 1]->name_of_stop, name_to_bus_.at(name_of_bus)->bus_stops[i]->name_of_stop });
					if (it != stops_distance_.end())
						total_metres_distance += it->second;
				}

			}
		}

		curvature = total_metres_distance / total_geo_distance;

	}

	return { total_metres_distance, curvature };

}
