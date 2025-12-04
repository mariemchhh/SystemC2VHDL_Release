#include <systemc.h>

SC_MODULE(freq_divider){
  sc_in<bool> clk_in;
  sc_in<bool> reset;
  sc_out<bool> clk_out;
  
  bool toggle;
  
  void compute(){
    if(reset.read()){
      toggle = false;
    }
    else{
      toggle = !toggle;
    }
    clk_out.write(toggle);
  }
  
  SC_CTOR(freq_divider){
    toggle = false;
    SC_METHOD(compute);
    sensitive << clk_in.pos() << reset;
  }
};
