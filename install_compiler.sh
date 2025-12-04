#!/bin/bash

##############################################################################
# Script d'installation du Compilateur SystemC → VHDL Amélioré
# Version: 2.0 (Décembre 2025)
##############################################################################

set -e  # Arrêter en cas d'erreur

PROJECT_ROOT="/media/mariem/D1/systemc_project"
COMPILER_SRC="$PROJECT_ROOT/compiler_src"
BUILD_DIR="$COMPILER_SRC/build"
EXECUTABLE="$BUILD_DIR/systemc2vhdl"

echo "=========================================="
echo "   Installation du Compilateur Amélioré"
echo "=========================================="
echo ""

# Vérifier que nous sommes dans le bon répertoire
if [ ! -d "$COMPILER_SRC" ]; then
    echo "❌ Erreur : Répertoire $COMPILER_SRC non trouvé"
    exit 1
fi

# Étape 1 : Nettoyer le build précédent
echo "📁 Nettoyage du build précédent..."
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

# Étape 2 : Configuration avec CMake
echo "⚙️  Configuration avec CMake..."
cd "$BUILD_DIR"
CC=gcc CXX=g++ cmake .. > /dev/null 2>&1

if [ $? -ne 0 ]; then
    echo "❌ Erreur lors de la configuration CMake"
    exit 1
fi

# Étape 3 : Compilation
echo "🔨 Compilation en cours..."
make -j$(nproc) > /dev/null 2>&1

if [ $? -ne 0 ]; then
    echo "❌ Erreur lors de la compilation"
    exit 1
fi

# Étape 4 : Vérifier l'exécutable
if [ ! -f "$EXECUTABLE" ]; then
    echo "❌ Erreur : Exécutable non créé"
    exit 1
fi

# Étape 5 : Créer un lien symbolique dans le projet
cd "$PROJECT_ROOT"
ln -sf "$EXECUTABLE" systemc2vhdl_new

# Étape 6 : Tests rapides
echo ""
echo "🧪 Tests de validation..."

# Test 1 : XOR
TEST_RESULT=$("$EXECUTABLE" "$PROJECT_ROOT/src/xor.cpp" -o /tmp/test_xor.vhdl 2>&1)
if grep -q "a : in std_logic" /tmp/test_xor.vhdl && \
   grep -q "b : in std_logic" /tmp/test_xor.vhdl && \
   grep -q "y : out std_logic" /tmp/test_xor.vhdl; then
    echo "  ✅ Test XOR : PASS (3/3 ports détectés)"
else
    echo "  ❌ Test XOR : FAIL"
    exit 1
fi

# Test 2 : Counter (types vectoriels)
"$EXECUTABLE" "$PROJECT_ROOT/src/counter.cpp" -o /tmp/test_counter.vhdl 2>&1 > /dev/null
if grep -q "std_logic_vector(7 downto 0)" /tmp/test_counter.vhdl; then
    echo "  ✅ Test Counter : PASS (sc_uint<8> converti)"
else
    echo "  ❌ Test Counter : FAIL"
    exit 1
fi

# Test 3 : Half Adder (opérateurs)
"$EXECUTABLE" "$PROJECT_ROOT/src/half_adder.cpp" -o /tmp/test_half_adder.vhdl 2>&1 > /dev/null
if grep -q "xor" /tmp/test_half_adder.vhdl && \
   grep -q "and" /tmp/test_half_adder.vhdl; then
    echo "  ✅ Test Half Adder : PASS (opérateurs traduits)"
else
    echo "  ❌ Test Half Adder : FAIL"
    exit 1
fi

# Nettoyage
rm -f /tmp/test_*.vhdl

echo ""
echo "=========================================="
echo "   ✅ Installation Réussie !"
echo "=========================================="
echo ""
echo "📍 Emplacement de l'exécutable :"
echo "   $EXECUTABLE"
echo ""
echo "🔗 Lien symbolique créé :"
echo "   $PROJECT_ROOT/systemc2vhdl_new"
echo ""
echo "📝 Utilisation :"
echo "   ./systemc2vhdl_new <input.cpp> -o <output.vhdl>"
echo ""
echo "📚 Exemple :"
echo "   ./systemc2vhdl_new src/xor.cpp -o output/xor.vhdl"
echo ""
echo "🎯 Pour convertir tous les fichiers :"
echo "   ./convert_all_new.sh"
echo ""
