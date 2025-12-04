#include "parser.hpp"
#include <fstream>
#include <sstream>
#include <regex>
#include <iostream>
#include <algorithm>

// Helper function to trim whitespace
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, last - first + 1);
}

Module parse_systemc_file(const std::string& path) {
    Module mod;
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Erreur: impossible d'ouvrir le fichier " << path << "\n";
        return mod;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    std::smatch match;

    // 1️⃣ Nom du module
    std::regex module_regex(R"(SC_MODULE\((\w+)\))");
    if (std::regex_search(content, match, module_regex)) {
        mod.name = match[1];
    }

    // 2️⃣ Ports (sc_in et sc_out) avec support des déclarations multiples
    // Regex améliorée : capture direction, type, et liste de noms
    // Support des types imbriqués comme sc_out<sc_uint<8>>
    std::regex port_regex(R"(sc_(in|out)<(.*?)>\s+([^;]+);)");
    
    for (auto i = std::sregex_iterator(content.begin(), content.end(), port_regex);
         i != std::sregex_iterator(); ++i) {
        std::smatch m = *i;
        std::string direction = m[1].str();  // "in" or "out"
        std::string full_match = m[2].str(); // Could be "bool" or "sc_uint<8>"
        std::string names_str = m[3].str();  // "a, b, c" or just "a"
        
        // Extract the type - handle nested <>
        std::string type;
        int angle_count = 0;
        for (char c : full_match) {
            type += c;
            if (c == '<') angle_count++;
            else if (c == '>') {
                angle_count--;
                if (angle_count == 0) break;
            }
        }
        
        // Parser les noms multiples séparés par des virgules
        std::istringstream iss(names_str);
        std::string name;
        while (std::getline(iss, name, ',')) {
            name = trim(name);
            if (!name.empty()) {
                mod.ports.push_back({name, type, direction});
            }
        }
    }

    // 3️⃣ Liste de sensibilité (sensitive << a << b << c;)
    std::regex sens_regex(R"(sensitive\s*<<\s*([^;]+);)");
    if (std::regex_search(content, match, sens_regex)) {
        std::string sens_str = match[1].str();
        
        // Parser les signaux : "a << b << c" ou "clk.pos() << reset"
        std::regex sig_regex(R"((\w+)(?:\.\w+\(\))?)");
        for (auto i = std::sregex_iterator(sens_str.begin(), sens_str.end(), sig_regex);
             i != std::sregex_iterator(); ++i) {
            std::string signal = (*i)[1].str();
            mod.sensitivity_list.push_back(signal);
        }
    }

    // 4️⃣ Corps de méthode - Extraire tout le contenu de compute()
    // Amélioration : supporte void compute() ET void compute(nom_module)
    // Et gère correctement les accolades imbriquées
    size_t compute_pos = content.find("void compute");
    if (compute_pos != std::string::npos) {
        size_t brace_start = content.find('{', compute_pos);
        if (brace_start != std::string::npos) {
            int brace_count = 1;
            size_t pos = brace_start + 1;
            while (pos < content.length() && brace_count > 0) {
                if (content[pos] == '{') brace_count++;
                else if (content[pos] == '}') brace_count--;
                pos++;
            }
            if (brace_count == 0) {
                mod.body = content.substr(brace_start + 1, pos - brace_start - 2);
            }
        }
    }

    return mod;
}
