#pragma once

#include <iostream>
#include <iosfwd>
#include <string_view>
#include <string>

#include "transport_catalogue.h"

void ParseAndPrintStat(const catalogue::TransportCatalogue& tansport_catalogue, std::string_view request,
                       std::ostream& output);

void LoopForPrint(const catalogue::TransportCatalogue& tansport_catalogue, const int stat_request_count, std::istream& output);