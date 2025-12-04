#include <systemc.h>

SC_MODULE(pulse_generator){
  sc_in<bool> clk;
  sc_in<bool> trigger;
  sc_out<bool> pulse;
  
  bool pulse_active;
  
  void compute(){
    if(trigger.read() && !pulse_active){
      pulse_active = true;
    }
    else{
      pulse_active = false;
    }
    pulse.write(pulse_active);
  }
  
  SC_CTOR(pulse_generator){
    pulse_active = false;
    SC_METHOD(compute);
    sensitive << clk.pos();
  }
};
