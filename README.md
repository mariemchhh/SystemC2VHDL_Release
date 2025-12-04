# 🚀 SystemC to VHDL Compiler

Un compilateur open-source performant capable de traduire des modules **SystemC** en code **VHDL synthétisable**.

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Language](https://img.shields.io/badge/language-C%2B%2B17-orange.svg)
![Build](https://img.shields.io/badge/build-CMake-green.svg)

## 🌟 Fonctionnalités

Ce compilateur analyse le code source C++ (SystemC) et génère automatiquement l'architecture VHDL correspondante.

- **Détection Intelligente des Ports** : Supporte les déclarations multiples (`sc_in<bool> a, b, c;`) et les types imbriqués.
- **Conversion de Types** :
  - `bool` → `std_logic`
  - `sc_uint<N>` → `std_logic_vector(N-1 downto 0)`
  - `sc_int<N>` → `signed(N-1 downto 0)`
- **Traduction de la Logique** :
  - Conversion des opérateurs C++ (`&&`, `||`, `^`, `!`) en VHDL (`and`, `or`, `xor`, `not`).
  - Transformation des structures de contrôle (`if`, `else if`, `else`) en équivalents VHDL (`if`, `elsif`, `else`).
  - Gestion automatique des listes de sensibilité.
- **Code Synthétisable** : Le VHDL généré est prêt pour la synthèse sur FPGA (Intel Quartus, Xilinx Vivado).

## 📦 Installation

### Prérequis
- Compilateur C++ (GCC/Clang)
- CMake (3.10+)
- Make

### Installation Rapide
Utilisez le script d'installation fourni :

```bash
./install_compiler.sh
```

Ou compilez manuellement :

```bash
mkdir -p compiler_src/build
cd compiler_src/build
cmake ..
make
```

## 🚀 Utilisation

### Conversion d'un fichier unique

```bash
./systemc2vhdl_new src/counter.cpp -o output/counter.vhdl
```

### Conversion en masse

Pour convertir tous les fichiers `.cpp` du dossier `src/` :

```bash
./convert_all_new.sh
```

## 📝 Exemples

### Entrée : SystemC (`counter.cpp`)

```cpp
SC_MODULE(counter){
  sc_in<bool> clk, reset, enable;
  sc_out<sc_uint<8>> count;
  sc_uint<8> count_value;
  
  void compute(){
    if(reset.read()){
      count_value = 0;
    }
    else if(enable.read()){
      count_value = count_value + 1;
    }
    count.write(count_value);
  }
  
  SC_CTOR(counter){
    SC_METHOD(compute);
    sensitive << clk.pos() << reset;
  }
};
```

### Sortie : VHDL (`counter.vhdl`)

```vhdl
entity counter is
  port (
    clk : in std_logic;
    reset : in std_logic;
    enable : in std_logic;
    count : out std_logic_vector(7 downto 0)
  );
end entity;

architecture rtl of counter is
begin
  process(clk, reset)
  begin
    if reset = '1' then
      count_value <= (others => '0');
    elsif enable = '1' then
      count_value <= count_value + 1;
    end if;
    count <= count_value;
  end process;
end architecture;
```

## 🤝 Contribution

Les contributions sont les bienvenues ! N'hésitez pas à ouvrir une issue ou une pull request pour :
- Ajouter le support de nouveaux types SystemC
- Améliorer la traduction des structures de contrôle
- Ajouter des tests unitaires

## 📄 Licence

Ce projet est sous licence MIT. Voir le fichier `LICENSE` pour plus de détails.
