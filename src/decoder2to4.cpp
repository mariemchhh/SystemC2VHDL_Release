#include <systemc.h>

SC_MODULE(decoder2to4){
  sc_in<bool> a, b;
  sc_in<bool> enable;
  sc_out<bool> y0, y1, y2, y3;
  
  void compute(){
    if(enable.read()){
      if(!a.read() && !b.read()){
        y0.write(true);
        y1.write(false);
        y2.write(false);
        y3.write(false);
      }
      else if(!a.read() && b.read()){
        y0.write(false);
        y1.write(true);
        y2.write(false);
        y3.write(false);
      }
      else if(a.read() && !b.read()){
        y0.write(false);
        y1.write(false);
        y2.write(true);
        y3.write(false);
      }
      else{
        y0.write(false);
        y1.write(false);
        y2.write(false);
        y3.write(true);
      }
    }
    else{
      y0.write(false);
      y1.write(false);
      y2.write(false);
      y3.write(false);
    }
  }
  
  SC_CTOR(decoder2to4){
    SC_METHOD(compute);
    sensitive << a << b << enable;
  }
};
