#include <iostream>
#include <string>

#include "input_reader.h"
#include "stat_reader.h"

using namespace std;

int main() {
    catalogue::TransportCatalogue catalogue;

    int base_request_count;
    cin >> base_request_count >> ws;
    LoopForInput(catalogue, base_request_count);

    int stat_request_count;
    cin >> stat_request_count >> ws;
    LoopForPrint(catalogue, stat_request_count);
   
}