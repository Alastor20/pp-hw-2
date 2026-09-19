#include <cstddef>
#include <cstring>
#include <exception>
#include <iostream>
#include <random>

struct MultiThreadStruct
{
  double r;
  size_t tests, seed;
};

bool isInside(double x, double y, double r)
{
  return (x - r) * (x - r) + (y - r) * (y - r) <= r * r;
}

size_t calc(double r, size_t tests, size_t seed)
{
  std::default_random_engine gen(seed);
  std::uniform_real_distribution< double > dist(0, 2 * r);
  size_t result = 0;
  for (size_t i = 0; i < tests; ++i) {
    double x = dist(gen);
    double y = dist(gen);
    if (isInside(x, y, r)) {
      ++result;
    }
  }
  return result;
}

void *multiThreader(void *data)
{
  MultiThreadStruct *task = static_cast< MultiThreadStruct * >(data);
  size_t result = calc(task->r, task->tests, task->seed);
  return reinterpret_cast< void * >(result);
}

double area(double r, size_t threads, size_t tests)
{
  if (r <= 0 || !threads || !tests) {
    throw std::invalid_argument("Invalid input data");
  }
  std::vector< pthread_t > pthreads(threads);
  std::vector< MultiThreadStruct > structs;
  structs.reserve(threads);
  size_t testPerThread = tests / threads;
  size_t tail = tests % threads;

  for (size_t i = 0; i < threads; i++) {
    size_t testsForThread = i < tail ? testPerThread + 1 : testPerThread;
    structs.push_back({r, testsForThread, i});

    int err = pthread_create(&pthreads[i], nullptr, multiThreader, &structs[i]);
    if (err) {
      std::cerr << "pthread_create error: " << strerror(err) << '\n';
      std::terminate();
    }
  }
  int err = 0;
  size_t result[1] = {};
  size_t hits = 0;
  for (auto &thread : pthreads) {
    err = pthread_join(thread, reinterpret_cast< void ** >(&result));
    if (err) {
      std::cerr << "pthread_join error: " << strerror(err) << '\n';
      std::terminate();
    }
    hits += *result;
  }
  return 4 * r * r * hits / tests;
}

int main()
{
  constexpr size_t threads = 16;
  double r = 0;
  size_t tests = 0;
  std::cin >> r >> tests;
  try {
    std::cout << area(r, threads, tests) << '\n';
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return 1;
  }
}
