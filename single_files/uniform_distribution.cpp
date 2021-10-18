// uniform_real_distribution
#include <iostream>
#include <random>

int main()
{
  const int nrolls=10000;  // number of experiments
  const int nstars=95;     // maximum number of stars to distribute
  const int fuzzy_size=8; // number of intervals

  std::default_random_engine generator;
  std::uniform_real_distribution<double> distribution(0.0,1.0);

  int p[fuzzy_size]={};

    for (int iter=0; iter < nrolls; iter++) {
        for (int curr_node=0; curr_node < fuzzy_size; curr_node++) {
            double number = distribution(generator);
            if (number <= (1.0/fuzzy_size)) {
                ++p[curr_node];
            }
        }

        for (int i=0; i<fuzzy_size; ++i) {
          std::cout << i << " (" << p[i] << "): ";
          std::cout << std::string(p[i]*nstars/nrolls,'*') << std::endl;
        }
        std::cout << "----------------------" << '\n';
    }

  std::cout << "uniform_real_distribution (0.0,1.0):" << std::endl;
  std::cout << std::fixed; std::cout.precision(1);

  for (int i=0; i<fuzzy_size; ++i) {
    std::cout << i << " (" << p[i] << "): ";
    std::cout << std::string(p[i]*nstars/nrolls,'*') << std::endl;
  }

  return 0;
}
