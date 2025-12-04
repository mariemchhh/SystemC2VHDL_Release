#include <iostream>
#include <filesystem>
#include <fstream>
#include "parser.hpp"
#include "vhdl_gen.hpp"

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    if(argc < 2) {
        std::cerr << "Usage: systemc2vhdl <input_file.cpp> [-o <output_file.vhdl>]\n";
        return 1;
    }

    std::string input_file = argv[1];

    // Parser le fichier SystemC
    Module module = parse_systemc_file(input_file);

    // Générer le code VHDL
    std::string vhdl_code = generate_vhdl(module);

    // Déterminer le fichier de sortie
    std::string output_file;
    
    if (argc >= 4 && std::string(argv[2]) == "-o") {
        // Option -o fournie : utiliser le chemin spécifié
        output_file = argv[3];
    } else {
        // Comportement par défaut : vhdl/module_name.vhdl
        fs::path cpp_path(input_file);
        fs::path project_root = cpp_path.parent_path().parent_path();
        fs::path output_dir = project_root / "vhdl";
        fs::create_directories(output_dir);
        output_file = output_dir / (cpp_path.stem().string() + ".vhdl");
    }

    // Écrire le fichier
    std::ofstream ofs(output_file);
    if(!ofs) {
        std::cerr << "Erreur : impossible d'ouvrir " << output_file << " pour écriture\n";
        return 1;
    }
    ofs << vhdl_code;
    ofs.close();

    std::cout << "VHDL généré dans : " << output_file << std::endl;
    return 0;
}
