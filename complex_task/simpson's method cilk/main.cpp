#include <stdio.h>
#include <chrono>


#include <cilk/cilk.h>
#include <cilk/reducer_opadd.h>



double f(double);
double simpson_method_integrate(double, double, size_t);
double cilk_simpson_method_integrate(double, double, size_t);


int main()
{
  double a = 0.0, b = 1.0;
  

  auto array = { 100, 1000, 10000, 100000, 1000000, 10000000, 100000000 };

  for (size_t size : array)
  {

    printf("time for N = %d\n" , size);
    auto begin_time = std::chrono::high_resolution_clock::now();
    double S = simpson_method_integrate(a, b, size);
    auto end_time = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> time = end_time - begin_time;
    printf("integrate = %f, time for process = %f sec\n", S, time);



    auto begin_time2 = std::chrono::high_resolution_clock::now();
    double S2 = cilk_simpson_method_integrate(a, b, size);
    auto end_time2 = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> time2 = end_time2 - begin_time2;
    printf("cilk integrate = %f, time for process = %f sec\n", S2, time2);
  }


  system("pause");
  return 0;
}



double f(double x)
{
  return 8 / (1 + x * x);
}


// formula's simpson f(x)dx = (h/3) * ( y0 + yn + 4 * (y1 + y3 + ... + yn-1) + 2 * (y2 + y4 + ... yn-2) ) 
// or
// f(x)dx = (2h/6) * (y0 + 4*y1 + y2)
double simpson_method_integrate(double a, double b, size_t N)
{
  double S = 0.0;
  const double h = (b - a) / N;

  for (size_t i = 0; i < N; i++)
    S += f(a + h * i) + 4 * f((a + h * i) + h / 2) + f((a + h * i) + h);


  return (h / 6) * S;

}


double cilk_simpson_method_integrate(double a, double b, size_t N)
{
  const double h = (b - a) / N;

  cilk::reducer_opadd<double> S(0.0);
  cilk_for(size_t i = 0; i < N; i++)
    S += f(a + h * i) + 4 * f((a + h * i) + h / 2) + f((a + h * i) + h);


  return (h / 6) * S->get_value();
}