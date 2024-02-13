#pragma once

#include <iostream>
#include <deque>
#include <string>
#include <vector>
#include <set>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include "geo.h"

namespace catalogue {

	class TransportCatalogue {

	public:
		struct BusStop {
			std::string name_of_stop;
			geo::Coordinates coords;
			std::set<std::string> buses_to_stop_ = {};
		};

		struct Buses {
			std::string name_of_bus;
			std::vector<BusStop*> bus_stops;
		};

		struct BusesInfo {
			size_t amount_stops;
			size_t amount_unique_stops;
			double length;
		};

		struct StopsInfo {
			std::string_view stop;
			std::vector<std::string> buses;
		};

		void AddStop(const BusStop& stop);

		void AddBus(const std::string& id, const std::vector<std::string_view>& route);

		BusesInfo GetBusInfo(std::string_view name_of_bus) const;

		StopsInfo GetStopInfo(std::string_view name_of_bus) const;

		/*size_t UniqueStopsCount(std::string_view bus_number) const;

		double LengthCount(std::string_view bus_number) const;*/

	private:
		std::deque<BusStop> stops_;
		std::unordered_map<std::string_view, BusStop*> name_to_stop_;

		std::deque<Buses> buses_;
		std::unordered_map<std::string_view, Buses*> name_to_bus_;

		size_t UniqueStopsCount(std::string_view bus_number) const;
		double LengthCount(std::string_view bus_number) const;
		// Реализуйте класс самостоятельно
	};
}