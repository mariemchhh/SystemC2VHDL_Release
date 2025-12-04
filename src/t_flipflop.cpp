#include <systemc.h>

SC_MODULE(t_flipflop){
  sc_in<bool> clk;
  sc_in<bool> t;
  sc_out<bool> q;
  
  bool q_state;
  
  void compute(){
    if(t.read()){
      q_state = !q_state;
    }
    q.write(q_state);
  }
  
  SC_CTOR(t_flipflop){
    q_state = false;
    SC_METHOD(compute);
    sensitive << clk.pos();
  }
};
