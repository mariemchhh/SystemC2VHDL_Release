#!/bin/bash

##############################################################################
# Script de Conversion en Masse - Compilateur SystemC → VHDL v2.0
##############################################################################

PROJECT_ROOT="/media/mariem/D1/systemc_project"
COMPILER="$PROJECT_ROOT/compiler_src/build/systemc2vhdl"
SRC_DIR="$PROJECT_ROOT/src"
OUTPUT_DIR="$PROJECT_ROOT/vhdl_new"

# Couleurs
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo ""
echo "=========================================="
echo "   Conversion SystemC → VHDL en Masse"
echo "   Version 2.0 (Amélioré)"
echo "=========================================="
echo ""

# Vérifier que le compilateur existe
if [ ! -f "$COMPILER" ]; then
    echo -e "${RED}❌ Erreur : Compilateur non trouvé${NC}"
    echo "   Veuillez exécuter : ./install_compiler.sh"
    exit 1
fi

# Créer le répertoire de sortie
mkdir -p "$OUTPUT_DIR"

# Compteurs
total=0
success=0
failed=0

echo -e "${BLUE}📁 Sources   :${NC} $SRC_DIR"
echo -e "${BLUE}📁 Destination :${NC} $OUTPUT_DIR"
echo -e "${BLUE}🔧 Compilateur :${NC} systemc2vhdl v2.0"
echo ""

# Parcourir tous les fichiers .cpp
for cpp_file in "$SRC_DIR"/*.cpp; do
    if [ -f "$cpp_file" ]; then
        module=$(basename "$cpp_file" .cpp)
        vhdl_file="$OUTPUT_DIR/${module}.vhdl"
        ((total++))
        
        printf "  [$total] %-20s ... " "$module"
        
        # Exécuter la conversion
        output=$("$COMPILER" "$cpp_file" -o "$vhdl_file" 2>&1)
        
        if [ $? -eq 0 ] && [ -f "$vhdl_file" ]; then
            # Vérifier que le fichier contient du VHDL valide
            if grep -q "entity" "$vhdl_file" && grep -q "architecture" "$vhdl_file"; then
                echo -e "${GREEN}✓ OK${NC}"
                ((success++))
            else
                echo -e "${RED}✗ INVALID${NC}"
                ((failed++))
            fi
        else
            echo -e "${RED}✗ FAILED${NC}"
            echo "      Error: $output"
            ((failed++))
        fi
    fi
done

echo ""
echo "=========================================="
if [ $failed -eq 0 ]; then
    echo -e "${GREEN}✅ Succès : $success/$total fichiers convertis${NC}"
else
    echo -e "Résultat : ${GREEN}$success succès${NC}, ${RED}$failed échecs${NC} sur $total fichiers"
fi
echo "=========================================="
echo ""

# Afficher le contenu du répertoire de sortie
echo "📂 Fichiers générés dans $OUTPUT_DIR/ :"
ls -lh "$OUTPUT_DIR"/*.vhdl 2>/dev/null | awk '{printf "   %s %10s  %s\n", $9, $5, $9}' | sed "s|$OUTPUT_DIR/||g"

echo ""
echo "💡 Conseil : Vous pouvez vérifier la syntaxe VHDL avec :"
echo "   ghdl -a vhdl_new/nom_fichier.vhdl"
echo ""
