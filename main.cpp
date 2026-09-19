#include <random>

bool isInside(double x, double y, double r)
{
  return (x - r) * (x - r) + (y - r) * (y - r) <= r * r;
}

size_t calc(double r, size_t testsCount, size_t seed)
{
  std::default_random_engine gen(seed);
  std::uniform_real_distribution< double > dist(0, 2 * r);
  size_t result = 0;
  for (size_t i = 0; i < testsCount; ++i) {
    if (isInside(dist(gen), dist(gen), r)) {
      ++result;
    }
  }
  return result;
}

int main()
{}
