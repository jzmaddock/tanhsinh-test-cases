# tanhsinh-test-cases

This repository contains a long list of integrals expressed as C++ functions for
testing automatic integrators: see tanh_sinh_test_cases.hpp.

Integration ranges and expected values are listed in tanh_sinh_test_cases.cpp.

Sample test driver is in tanh_sinh_test_errors.cpp and demonstrates testing
the tanhsinh integrator from Boost.Math along side the `qthsh` routine from 
https://www.genivia.com/files/qthsh.pdf

These test cases are based on the work of Graeme Dennes: https://newtonexcelbach.com/2020/10/29/numerical-integration-with-tanh-sinh-quadrature-v-5-0/
and are GPL licenced.

## TODO

Integration endpoints should all be exact binary values, expected results should be expressed 
in terms of symbolic constants or at least have more digits precision to enable multiprecision testing.

We should also add more test cases to cover integrating various special functions.
