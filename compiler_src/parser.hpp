#pragma once 
#include <string>
#include <vector>

// Port structure with type information
struct Port {
    std::string name;
    std::string type;        // "bool", "sc_uint<8>", etc.
    std::string direction;   // "in" or "out"
};

struct Module {
    std::string name;
    std::vector<Port> ports;          // All ports with type info
    std::string body;
    std::vector<std::string> sensitivity_list;  // Signals in sensitivity list
};

Module parse_systemc_file(const std::string& path);
