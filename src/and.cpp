#include <systemc.h>


SC_MODULE(and){
sc_in<bool> a,b;
sc_out<bool> y;
void compute(){
y.write(a.read() & b.read());

}

SC_CTOR(and){
SC_METHOD(compute);
sensitive<< a << b; 
}
};
