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

struct BusStop;
struct Buses;

namespace catalogue {

	namespace hasher{

		class StopsDistancePair{
		public:
			size_t operator()(const std::pair<std::string_view, std::string_view> stops) const{
				size_t hash_first = hasher_(stops.first);
				size_t hash_second = hasher_(stops.second);
				return hash_first + hash_second * 37;
			}
			bool operator==(const std::pair<BusStop*, BusStop*> other) const{
				return *this == other;
			}
		private:
			std::hash<std::string_view> hasher_;
		};

		class StrView {
		public:
			size_t operator()(const std::string_view str) const
			{
				return hasher_(str);
			}
			bool operator==(const std::string_view other) const
			{
				return *this == other;
			}
		private:
			std::hash<std::string_view> hasher_;
		};
	}

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
			double curvature;
		};

		struct StopsInfo {
			std::string_view stop;
			std::vector<std::string> buses;
		};

		void AddStop(const BusStop& stop, const std::vector<std::pair<std::string_view, int>> distances);

		void AddBus(const std::string& id, const std::vector<std::string_view>& route);

		BusesInfo GetBusInfo(std::string_view name_of_bus) const;

		StopsInfo GetStopInfo(std::string_view name_of_bus) const;

	private:
		std::deque<BusStop> stops_;
		std::unordered_map<std::string_view, BusStop*, hasher::StrView> name_to_stop_;
		std::unordered_map<std::pair<const std::string_view, const std::string_view>, int, hasher::StopsDistancePair> stops_distance_;

		std::deque<Buses> buses_;
		std::unordered_map<std::string_view, Buses*, hasher::StrView> name_to_bus_;

		std::deque<std::string> distances_;

		size_t UniqueStopsCount(std::string_view bus_number) const;
		double LengthCount(std::string_view bus_number) const;
		std::tuple<double, double> ComputeCurvature(std::string_view name_of_bus) const;
		// Реализуйте класс самостоятельно
	};
}