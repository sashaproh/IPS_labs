#include <cstdint>
#include <chrono>
#include <iostream>
#include <string>
#include <cmath>

#include <cilk/cilk.h>
#include <cilk/reducer_opadd.h>

double fun(double x)
{
  return 4.0 / (1 + x * x);
}

double integrate(double a, double b, size_t N)
{
  const double h = (b - a) / N;
  double sum = 0.0f;
  for (size_t i = 0; i < N; ++i)
    sum += fun(a + i * h);

  return sum * h;
}

double parallel_integrate(double a, double b, size_t N)
{
  const double h = (b - a) / N;
  cilk::reducer_opadd<double> sum(0.0);
  cilk_for(size_t i = 0; i < N; ++i)
    sum += fun(a + i * h);

  return sum->get_value() * h;
}

using namespace std::chrono;

template<class F, class... Args>
auto measure_time(const std::string& name, F&& f, Args&&... args)
{
  auto begin_time = high_resolution_clock::now();
  const auto ans = f(std::forward<Args>(args)...);
  auto end_time = high_resolution_clock::now();
  duration<double> est_time = end_time - begin_time;

  std::cout << name << " ans: " << ans << '\n'
    << "Spended time: " << est_time.count() << '\n';

  return est_time.count();
}

int main()
{
  const double a = 0.0;
  const double b = 1.0;
  const size_t N = 10000000;

  for (size_t i = 0; i < 5; ++i)
  {
    const size_t n = std::pow(10, 5 + i);
    std::cout << "N = " << n << '\n';
    const auto non_parallel_t = measure_time("Serial", integrate, a, b, n);
    const auto parallel_t = measure_time("Parallel", parallel_integrate, a, b, n);

    const double boost_t = non_parallel_t / parallel_t;
    std::cout << "Boost: " << boost_t << '\n'
      << "--------------------------------------\n";
      
  }

  system("pause");

  return 0;
}