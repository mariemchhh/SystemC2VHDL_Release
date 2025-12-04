#include "vhdl_gen.hpp"
#include <sstream>
#include <regex>
#include <algorithm>

// Convert SystemC types to VHDL types
std::string convert_type(const std::string& systemc_type) {
    if (systemc_type == "bool") {
        return "std_logic";
    }
    
    // sc_uint<N> → std_logic_vector(N-1 downto 0)
    std::regex uint_regex(R"(sc_uint<(\d+)>)");
    std::smatch match;
    if (std::regex_search(systemc_type, match, uint_regex)) {
        int bits = std::stoi(match[1].str());
        if (bits == 1) {
            return "std_logic";
        }
        return "std_logic_vector(" + std::to_string(bits-1) + " downto 0)";
    }
    
    // sc_int<N> → signed(N-1 downto 0)
    std::regex int_regex(R"(sc_int<(\d+)>)");
    if (std::regex_search(systemc_type, match, int_regex)) {
        int bits = std::stoi(match[1].str());
        if (bits == 1) {
            return "std_logic";
        }
        return "signed(" + std::to_string(bits-1) + " downto 0)";
    }
    
    // Par défaut
    return "std_logic";
}

// Translate SystemC code to VHDL
std::string translate_systemc_to_vhdl(const std::string& code) {
    std::string result = code;
    
    // 1. Supprimer .read()
    result = std::regex_replace(result, std::regex(R"(\.read\(\))"), "");
    
    // 2. Convertir .write(x) en <= x;
    result = std::regex_replace(result, 
                                std::regex(R"((\w+)\.write\(([^)]+)\))"),
                                "$1 <= $2");
    
    // 3. Convertir les opérateurs booléens
    result = std::regex_replace(result, std::regex(R"(\s+&&\s+)"), " and ");
    result = std::regex_replace(result, std::regex(R"(\|\|)"), " or ");
    
    // 4. Convertir XOR avec espaces autour
    result = std::regex_replace(result, std::regex(R"(\s+\^\s+)"), " xor ");
    
    // 5. Convertir NOT (gérer les espaces)
    result = std::regex_replace(result, std::regex(R"(\s+!\s*)"), " not ");
    
    // 6. Convertir les comparaisons
    result = std::regex_replace(result, std::regex(R"(\s+==\s+)"), " = ");
    result = std::regex_replace(result, std::regex(R"(\s+!=\s+)"), " /= ");
    result = std::regex_replace(result, std::regex(R"(\s+>=\s+)"), " >= ");
    result = std::regex_replace(result, std::regex(R"(\s+<=\s+)"), " <= ");
    result = std::regex_replace(result, std::regex(R"(\s+>\s+)"), " > ");
    result = std::regex_replace(result, std::regex(R"(\s+<\s+)"), " < ");
    
    // 7. Traiter } else if { en premier
    result = std::regex_replace(result, std::regex(R"(\}\s*else\s+if\s*\(([^)]+)\)\s*\{)"), 
                                "elsif $1 then");
    
    // 8. Traiter } else {
    result = std::regex_replace(result, std::regex(R"(\}\s*else\s*\{)"), 
                                "else");
    
    // 9. Convertir les if restants
    result = std::regex_replace(result, std::regex(R"(if\s*\(([^)]+)\)\s*\{)"), 
                                "if $1 then");
    
    // 10. Gérer les accolades fermantes - compter les structures pour ajouter end if
    std::string final_result;
    std::istringstream iss(result);
    std::string line;
    std::vector<std::string> lines;
    
    // Lire toutes les lignes
    while (std::getline(iss, line)) {
        lines.push_back(line);
    }
    
    // Traiter ligne par ligne
    for (size_t i = 0; i < lines.size(); i++) {
        std::string line = lines[i];
        
        // Remplacer les } par end if; si approprié
        size_t brace_pos = line.find('}');
        if (brace_pos != std::string::npos) {
            // Vérifier si on est dans un contexte de if
            // (on cherche if/elsif/else dans les lignes précédentes)
            bool in_if_context = false;
            for (int j = i - 1; j >= 0 && j >= (int)i - 10; j--) {
                if (lines[j].find(" then") != std::string::npos ||
                    lines[j].find("else") != std::string::npos) {
                    in_if_context = true;
                    break;
                }
            }
            
            if (in_if_context) {
                line = line.substr(0, brace_pos) + "end if;" + line.substr(brace_pos + 1);
            }
        }
        
        final_result += line + "\n";
    }
    
    // 11. Convertir les assignations = en <=
    // Mais seulement pour les assignations de signaux, pas dans les conditions
    final_result = std::regex_replace(final_result, 
                                      std::regex(R"((\s+)(\w+)\s*=\s*([^;=]+);)"),
                                      "$1$2 <= $3;");
    
    // 12. Ajouter indentation propre
    std::string indented;
    std::istringstream iss2(final_result);
    int indent_level = 0;
    while (std::getline(iss2, line)) {
        std::string trimmed = line;
        trimmed.erase(0, trimmed.find_first_not_of(" \t"));
        
        // Décrémenter avant les end if
        if (trimmed.find("end if") == 0 || trimmed.find("elsif") == 0 || 
            trimmed.find("else") == 0) {
            indent_level = std::max(0, indent_level - 1);
        }
        
        // Ajouter l'indentation
        for (int i = 0; i < indent_level; i++) {
            indented += "  ";
        }
        indented += trimmed + "\n";
        
        // Incrémenter après les then et else
        if (trimmed.find(" then") != std::string::npos || 
            (trimmed.find("else") == 0 && trimmed.find("elsif") == std::string::npos)) {
            indent_level++;
        }
    }
    
    return indented;
}

std::string generate_vhdl(const Module& m) {
    std::ostringstream vhdl;
    
    // Entity declaration
    vhdl << "entity " << m.name << " is\n";
    
    if (!m.ports.empty()) {
        vhdl << "  port (\n";
        
        for (size_t i = 0; i < m.ports.size(); ++i) {
            const Port& p = m.ports[i];
            std::string vhdl_type = convert_type(p.type);
            std::string direction = p.direction;  // "in" or "out"
            
            vhdl << "    " << p.name << " : " << direction << " " << vhdl_type;
            
            // Add semicolon if not the last port
            if (i + 1 < m.ports.size()) {
                vhdl << ";";
            }
            vhdl << "\n";
        }
        
        vhdl << "  );\n";
    } else {
        vhdl << "  port ();\n";
    }
    
    vhdl << "end entity;\n\n";
    
    // Architecture
    vhdl << "architecture rtl of " << m.name << " is\n";
    vhdl << "begin\n";
    
    // Process with sensitivity list
    vhdl << "  process(";
    for (size_t i = 0; i < m.sensitivity_list.size(); ++i) {
        vhdl << m.sensitivity_list[i];
        if (i + 1 < m.sensitivity_list.size()) {
            vhdl << ", ";
        }
    }
    vhdl << ")\n";
    vhdl << "  begin\n";
    
    // Translate and add body
    std::string translated_body = translate_systemc_to_vhdl(m.body);
    
    // Add indentation to each line of the body
    std::istringstream iss(translated_body);
    std::string line;
    while (std::getline(iss, line)) {
        if (!line.empty() && line.find_first_not_of(" \t\n\r") != std::string::npos) {
            vhdl << "    " << line << "\n";
        }
    }
    
    vhdl << "  end process;\n";
    vhdl << "end architecture;\n";
    
    return vhdl.str();
}
