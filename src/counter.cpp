#include <systemc.h>

SC_MODULE(counter){
  sc_in<bool> clk;
  sc_in<bool> reset;
  sc_in<bool> enable;
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
    count_value = 0;
    SC_METHOD(compute);
    sensitive << clk.pos() << reset;
  }
};
