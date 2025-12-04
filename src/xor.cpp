#include <systemc.h>
SC_MODULE(xor){
  sc_in<bool> a, b;
  sc_out<bool> y;
 
void compute(xor){
  y.write(a.read() ^ b.read());
}
 SC_CTOR(xor){
  SC_METHOD(compute);
  sensitive << a << b;
}
};
