#pragma once

#include <iosfwd>
#include <string_view>

#include "transport_catalogue.h"

void ParseAndPrintStat(const catalogue::TransportCatalogue& tansport_catalogue, std::string_view request,
                       std::ostream& output);