/*

Copyright John Maddock 2021.

Derived in part from machine translation of "tanh-sinh integrator version 5"
Copyright ? 2010, 2020  Graeme Dennes
https://newtonexcelbach.com/2020/10/29/numerical-integration-with-tanh-sinh-quadrature-v-5-0/

This program is free software : you can redistribute itand /or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see < https://www.gnu.org/licenses/>.

*/

#include "tanh_sinh_test_cases.hpp"
#include <boost/math/quadrature/tanh_sinh.hpp>
#include <boost/math/special_functions/relative_difference.hpp>

#include <iostream>
#include <iomanip>

//#define TEST_QTHSH

unsigned calls = 0;

template <>
void log_test_call(const double&)
{
   ++calls;
}

template <class F, class Real>
Real qthsh(F f, Real a, Real b, int n, Real eps, Real* perror) 
{
   BOOST_MATH_STD_USING
   Real c = (a + b) / 2; // center (mean) 
   Real d = (b - a) / 2; // half distance 
   Real s = f(c);
   Real v, h = 2;
   int k = 0;
   if (n <= 0) // use default levels n=6 
      n = 6; // 6 is optimal, 7 just as good taking longer 
   if (eps <= 0) // use default eps=1E-9 
      eps = 1E-9;
   do {
      Real p = 0, q, fp = 0, fm = 0, t, eh;
      h /= 2;
      t = eh = exp(h);
      if (k > 0)
         eh *= eh;
      do {
         Real u = exp(1 / t - t);      // = exp(-2*sinh(j*h)) = 1/exp(sinh(j*h))^2 
         Real r = 2 * u / (1 + u);       // = 1 - tanh(sinh(j*h)) 
         Real w = (t + 1 / t) * r / (1 + u); // = cosh(j*h)/cosh(sinh(j*h))^2 
         Real x = d * r;
         if (a + x > a) {              // if too close to a then reuse previous fp 
            Real y = f(a + x);
            if (boost::math::isfinite(y))
               fp = y;                 // if f(x) is finite, add to the local sum 
         }
         if (b - x < b) {              // if too close to b then reuse previous fm 
            Real y = f(b - x);
            if (boost::math::isfinite(y))
               fm = y;                 // if f(x) is finite, add to the local sum 
         }
         q = w * (fp + fm);
         p += q;
         t *= eh;
      } while (fabs(q) > eps * fabs(p));
      v = 2 * s;
      s += p;
      ++k;
   } while (fabs(v - s) > 10 * eps * fabs(s) && k <= n);
   *perror = fabs(v - s) / (fabs(s) + eps);
   return d * s * h; // result with estimated relative error e 
}


int main()
{
   std::pair<const test_entry*, const test_entry*> p = get_tests();

   boost::math::quadrature::tanh_sinh<double> integrator;

   double tolerance = 1e-9;
   double error = 0;
   int index = 1;

   std::cout << "   N                   Result    #Calls  ErrorEstimate     ErrorFound\n";

   for (const test_entry* pos = p.first; pos != p.second; ++pos, ++index)
   {
      try {
         double result = integrator.integrate(pos->proc, pos->a, pos->b, tolerance, &error);
         std::cout << std::setw(4) << std::right << index
            << std::setw(25) << std::scientific << std::right << std::setprecision(17) << result
            << std::setw(10) << std::right << calls
            << std::setw(15) << std::right << std::setprecision(4) << error
            << std::setw(15) << std::right << std::setprecision(4) << boost::math::relative_difference(result, pos->exact_result) << std::endl;
      }
      catch (const boost::math::evaluation_error&)
      {
         std::cout << std::setw(4) << std::right << index 
            << std::right << std::setw(20) << "EXCEPTION!!" << std::endl;
      }
      calls = 0;
   }
#ifdef TEST_QTHSH

   std::cout << "\n\nTesting qthsh at " << tolerance << std::endl << std::endl;
   std::cout << "\n\n   N                   Result    #Calls  ErrorEstimate     ErrorFound\n";
   index = 1;

   for (const test_entry* pos = p.first; pos != p.second; ++pos, ++index)
   {
      try {
         double result = qthsh(pos->proc, pos->a, pos->b, 15, tolerance, &error);
         std::cout << std::setw(4) << std::right << index
            << std::setw(25) << std::scientific << std::right << std::setprecision(17) << result
            << std::setw(10) << std::right << calls
            << std::setw(15) << std::right << std::setprecision(4) << error
            << std::setw(15) << std::right << std::setprecision(4) << boost::math::relative_difference(result, pos->exact_result) << std::endl;
      }
      catch (const boost::math::evaluation_error&)
      {
         std::cout << std::setw(4) << std::right << index
            << std::right << std::setw(20) << "EXCEPTION!!" << std::endl;
      }
      calls = 0;
   }
   //
   // Internally, qthsh multiplies the tolerance by 10 to obtain a termination condition.
   // Boost just uses tolerance directly as the termination condition.
   // So add this comparison to level up the playing field and compare like with like:
   //
   std::cout << "\n\nTesting qthsh at " << (tolerance / 10) << std::endl << std::endl;
   std::cout << "\n\n   N                   Result    #Calls  ErrorEstimate     ErrorFound\n";
   index = 1;

   for (const test_entry* pos = p.first; pos != p.second; ++pos, ++index)
   {
      try {
         double result = qthsh(pos->proc, pos->a, pos->b, 15, tolerance / 10, &error);
         std::cout << std::setw(4) << std::right << index
            << std::setw(25) << std::scientific << std::right << std::setprecision(17) << result
            << std::setw(10) << std::right << calls
            << std::setw(15) << std::right << std::setprecision(4) << error
            << std::setw(15) << std::right << std::setprecision(4) << boost::math::relative_difference(result, pos->exact_result) << std::endl;
      }
      catch (const boost::math::evaluation_error&)
      {
         std::cout << std::setw(4) << std::right << index
            << std::right << std::setw(20) << "EXCEPTION!!" << std::endl;
      }
      calls = 0;
   }
#endif
}
