#include "stat_reader.h"

void ParseAndPrintStat(const catalogue::TransportCatalogue& tansport_catalogue, std::string_view request,
    std::ostream& output) {
    // Реализуйте самостоятельно
    auto space_pos = request.find(' ');
    std::string_view command_out = request.substr(0, space_pos);

    if (command_out == "Bus") {
        auto not_space = request.find_first_not_of(' ', space_pos);
        std::string_view num_of_bus = request.substr(not_space);

        auto information = std::move(tansport_catalogue.GetBusInfo(num_of_bus));
        if (information.amount_stops != 0) {
            output << "Bus " << num_of_bus << ": " << information.amount_stops << " stops on route, " <<
                information.amount_unique_stops << " unique stops, " << information.length << " route length"
                << std::endl;
        }
        else {
            output << "Bus " << num_of_bus << ": " << "not found" << std::endl;
        }
    }
    else {
        auto not_space = request.find_first_not_of(' ', space_pos);
        std::string_view stop_name = request.substr(not_space);
        
        auto information = std::move(tansport_catalogue.GetStopInfo(stop_name));
        if (information.buses[0] == "no buses")
            output << "Stop " << stop_name << ": no buses" << std::endl;
        else 
            if (information.buses[0] == "stop not found")
                output << "Stop " << stop_name << ": not found" << std::endl;
            else {
                output << "Stop " << stop_name << ": buses";
                for (int i = 0; i < information.buses.size(); ++i)
                    output << " " << std::string(information.buses[i]);
                output << std::endl;
            }

    }
}