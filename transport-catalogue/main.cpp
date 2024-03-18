#include "json_reader.h"
#include <iostream>
int main()
{
    using namespace transport_catalogue;
    TransportCatalogue catal;
    JSONreader reader;
    reader.ProcessRequest(std::cin, std::cout, catal);
}