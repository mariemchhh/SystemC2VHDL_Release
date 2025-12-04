#include <systemc.h>
SC_MODULE(or){
sc_in<bool> a,b;
sc_out<bool> y;

void compute(){
y.write(a.read() || b.read());
}
SC_CTOR(or){
SC_METHOD(compute);
sensitive << a << b;
}

};
