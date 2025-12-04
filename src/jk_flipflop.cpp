#include <systemc.h>

SC_MODULE(jk_flipflop){
  sc_in<bool> clk;
  sc_in<bool> j, k;
  sc_out<bool> q, qn;
  
  bool q_state;
  
  void compute(){
    if(!j.read() && !k.read()){
      q_state = q_state;
    }
    else if(j.read() && !k.read()){
      q_state = true;
    }
    else if(!j.read() && k.read()){
      q_state = false;
    }
    else{
      q_state = !q_state;
    }
    q.write(q_state);
    qn.write(!q_state);
  }
  
  SC_CTOR(jk_flipflop){
    q_state = false;
    SC_METHOD(compute);
    sensitive << clk.pos();
  }
};
