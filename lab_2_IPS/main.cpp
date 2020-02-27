#include <stdio.h>
#include <ctime>
#include <cilk/cilk.h>
#include <cilk/reducer_opadd.h>
#include <chrono>

using namespace std::chrono;

// ���������� ����� � �������� ���������� �������
const int MATRIX_SIZE = 100;

/// ������� InitMatrix() ��������� ���������� � �������� 
/// ��������� ���������� ������� ���������� ����������
/// matrix - �������� ������� ����
void InitMatrix(double** matrix)
{
  for (int i = 0; i < MATRIX_SIZE; ++i)
  {
    matrix[i] = new double[MATRIX_SIZE + 1];
  }

  for (int i = 0; i < MATRIX_SIZE; ++i)
  {
    for (int j = 0; j <= MATRIX_SIZE; ++j)
    {
      matrix[i][j] = rand() % 2500 + 1;
    }
  }
}

/// ������� SerialGaussMethod() ������ ���� ������� ������ 
/// matrix - �������� ������� �������������� ���������, �������� � ����,
/// ��������� ������� ������� - �������� ������ ������ ���������
/// rows - ���������� ����� � �������� �������
/// result - ������ ������� ����
void SerialGaussMethod(double **matrix, const int rows, double* result)
{
  int k;
  double koef;


  auto begin_time = high_resolution_clock::now();

  // ������ ��� ������ ������
  for (k = 0; k < rows; ++k)
  {
    //
    for (int i = k + 1; i < rows; ++i)
    {
      koef = -matrix[i][k] / matrix[k][k];

      for (int j = k; j <= rows; ++j)
      {
        matrix[i][j] += koef * matrix[k][j];
      }
    }
  }

  auto end_time = high_resolution_clock::now();

  // �������� ��� ������ ������
  result[rows - 1] = matrix[rows - 1][rows] / matrix[rows - 1][rows - 1];

  for (k = rows - 2; k >= 0; --k)
  {
    result[k] = matrix[k][rows];

    //
    for (int j = k + 1; j < rows; ++j)
    {
      result[k] -= matrix[k][j] * result[j];
    }

    result[k] /= matrix[k][k];
  }

  duration<double> time = end_time - begin_time;
  printf("Solution version. Forward elimitation time = %f sec\n", time.count());
}


/// ������� ParallelSerialGaussMethod() ������ ���� ������� ������ 
/// matrix - �������� ������� �������������� ���������, �������� � ����,
/// ��������� ������� ������� - �������� ������ ������ ���������
/// rows - ���������� ����� � �������� �������
/// result - ������ ������� ����
void ParallelSerialGaussMethod(double **matrix, const int rows, double* result)
{
  int k;

  auto begin_time = high_resolution_clock::now();

  // ������ ��� ������ ������
  for (k = 0; k < rows; ++k)
  {
    //
    cilk_for(int i = k + 1; i < rows; ++i)
    {
      double koef = -matrix[i][k] / matrix[k][k];

      for (int j = k; j <= rows; ++j)
        matrix[i][j] += koef * matrix[k][j];
    }
  }
  auto end_time = high_resolution_clock::now();

  // �������� ��� ������ ������
  result[rows - 1] = matrix[rows - 1][rows] / matrix[rows - 1][rows - 1];

  for (k = rows - 2; k >= 0; --k)
  {

    cilk::reducer_opadd<double> result_for_k(matrix[k][rows]);

   // result[k] = matrix[k][rows];

    cilk_for(int j = k + 1; j < rows; ++j)
    {
      result_for_k -= matrix[k][j] * result[j];
      //result[k] -= matrix[k][j] * result[j];
    }

    result[k] = result_for_k->get_value() / matrix[k][k];
  }

  duration<double> time = end_time - begin_time;
  printf("Parallel version. Forward elimination time: %f sec\n", time.count());
}

int main()
{
  srand((unsigned)time(0));

  int i;

  // ���-�� ����� � �������, ���������� � �������� �������
  const int test_matrix_lines = MATRIX_SIZE;

  double **test_matrix = new double*[test_matrix_lines];

  // ���� �� �������
  for (i = 0; i < test_matrix_lines; ++i)
  {
    // (test_matrix_lines + 1)- ���������� �������� � �������� �������,
    // ��������� ������� ������� ������� ��� ������ ����� ���������, �������� � ����
    test_matrix[i] = new double[test_matrix_lines + 1];
  }

  // ������ ������� ����
  double *result = new double[test_matrix_lines];

  /*
  // ������������� �������� �������
  test_matrix[0][0] = 2; test_matrix[0][1] = 5;  test_matrix[0][2] = 4;  test_matrix[0][3] = 1;  test_matrix[0][4] = 20;
  test_matrix[1][0] = 1; test_matrix[1][1] = 3;  test_matrix[1][2] = 2;  test_matrix[1][3] = 1;  test_matrix[1][4] = 11;
  test_matrix[2][0] = 2; test_matrix[2][1] = 10; test_matrix[2][2] = 9;  test_matrix[2][3] = 7;  test_matrix[2][4] = 40;
  test_matrix[3][0] = 3; test_matrix[3][1] = 8;  test_matrix[3][2] = 9;  test_matrix[3][3] = 2;  test_matrix[3][4] = 37;
  */
  

  InitMatrix(test_matrix);

  SerialGaussMethod(test_matrix, test_matrix_lines, result);
  ParallelSerialGaussMethod(test_matrix, test_matrix_lines, result);

  
  printf("Solution:\n");

  for (i = 0; i < test_matrix_lines; ++i)
  {
    printf("x(%d) = %lf\n", i, result[i]);
  }

   system("pause");

  for (i = 0; i < test_matrix_lines; ++i)
  {
    delete[]test_matrix[i];
  }

  delete[] result;

  return 0;
}