#include <stdio.h>
#include <thread>
#include <chrono>


static const int THREADS_NUMBER = 4;


double f(double);
double simpson_method_integrate(double , double , size_t );
void thread_simpson_method_integrate(double & , const int ,double ,double , size_t );
void thread_simpson_execution(double & , double ,double , size_t );



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

    double S2 = 0.0;
    auto begin_time2 = std::chrono::high_resolution_clock::now();
    thread_simpson_execution(S2, a, b, size);
    auto end_time2 = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> time2 = end_time2 - begin_time2;
    printf("thread integrate = %f, time for process = %f sec\n", S2, time2);

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

 // #pragma loop(hint_parallel(4));
  for (size_t i = 0; i < N; i++)
    S += f(a + h * i) + 4 * f((a + h * i) + h / 2) + f((a + h * i) + h);


  return (h / 6) * S;

}


void thread_simpson_method_integrate(double & result , const int number_thread, double a, double b, size_t N)
{
  double S = 0.0;
  const double h = (b - a) / N;



  int dist = 0, last_dist = 0;


  dist = N / THREADS_NUMBER;

  if(number_thread + 1 == THREADS_NUMBER)
    dist = N - ( dist * (THREADS_NUMBER - 1) );
 

  if (number_thread + 1 == THREADS_NUMBER)
  {
    for (size_t i = N - dist; i < N; i++)
      S += f(a + h * i) + 4 * f((a + h * i) + h / 2) + f((a + h * i) + h);
  }
  else
    for (size_t i = number_thread * dist; i < dist * (number_thread + 1); i++)
      S += f(a + h * i) + 4 * f((a + h * i) + h / 2) + f((a + h * i) + h);
      


  result += (h/6) * S;

}


void thread_simpson_execution(double & result, double a, const double b, size_t N)
{
  std::thread threads[THREADS_NUMBER];
 

  for (int i = 0; i < THREADS_NUMBER; i++)
    threads[i] = std::thread(thread_simpson_method_integrate, std::ref(result), i, a, b, N);


  for (int i = 0; i < THREADS_NUMBER; i++)
    threads[i].join();

}