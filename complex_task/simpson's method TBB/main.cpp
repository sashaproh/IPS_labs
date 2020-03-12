#include <stdio.h>
#include <chrono>


#include <tbb/tbb.h>


size_t N = 100000000;

double f(double);
double simpson_method_integrate(double, double, size_t);
double tbb_simpson_method_integrate(double, double, size_t);


class SimpsonMethod
{
  private:
    double a_;

  public:
    double S;

    SimpsonMethod(double a, const size_t N) : a_(a), S(0.0) {}

    void operator() (const tbb::blocked_range<size_t>& range)
    {
      double h = 1.0 / N;
      double sum = S;
      for( size_t i = range.begin(); i != range.end(); i++ )
        sum += f(a_ + h * i) + 4 * f((a_ + h * i) + h / 2) + f((a_ + h * i) + h);

      S = sum;
    }

    SimpsonMethod(SimpsonMethod &s, tbb::split) : a_(s.a_), S(0.0) {}

    void join(const SimpsonMethod &s)
    {
      S += s.S;
    }

};



int main()
{
  double a = 0.0, b = 1.0;

  printf("time for N = %d\n", N);


  auto begin_time = std::chrono::high_resolution_clock::now();
  double S = simpson_method_integrate(a, b, N);
  auto end_time = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double> time = end_time - begin_time;
  printf("integrate = %f, time for process = %f sec\n", S, time);



  auto begin_time2 = std::chrono::high_resolution_clock::now();
  double S2 = tbb_simpson_method_integrate(a, b, N);
  auto end_time2 = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double> time2 = end_time2 - begin_time2;
  printf("TBB integrate = %f, time for process = %f sec\n", S2, time2);


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



double tbb_simpson_method_integrate(double a, double b, size_t N)
{
  double S = 0.0;
  const double h = (b - a) / N;
  tbb::task_scheduler_init init;

  SimpsonMethod simp(a, N);

  tbb::parallel_reduce(tbb::blocked_range<size_t>(0, N), simp, tbb::auto_partitioner());

  return (h / 6) * simp.S;
  
}
