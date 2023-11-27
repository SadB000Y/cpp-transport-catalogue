#include "transport_catalogue.h"
#include "geo.h"

template <typename Type>
std::set<Type> MakeSet(std::vector<Type>& query_words)
{
    std::set<Type> s(query_words.begin(), query_words.end());
    return s;
}

void catalogue::TransportCatalogue::AddStop(BusStop& stops) {
    stops_.push_back(stops); 
	name_to_stop_.insert({ stops_.back().name_of_stop, &stops_.back() });
}

void catalogue::TransportCatalogue::AddBus(std::string id, std::vector<std::string_view>&& routes) {
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
		
		return { name_to_bus_.at(std::string(bus_name))->bus_stops.size(), UniqueStopsCount(bus_name), LengthCount(bus_name)};

	}
	else
		return { 0, 0, 0};
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
