// uniform_real_distribution
#include <iostream>
#include <random>
#include <algorithm>

// normal distribution PDF
float normal_pdf(float x, float m, float s) {
    static const float inv_sqrt_2pi = 0.3989422804014327;
    float a = (x - m) / s;

    return inv_sqrt_2pi / s * exp(-0.5f * a * a);
}

int main()
{
  const int nrolls=10000;  // number of experiments
  const int nstars=95;     // maximum number of stars to distribute
  const int ncores=64;
  const int fuzzy_size=4; // number of intervals
  const float sigma=0.1*sqrt(fuzzy_size);
  const int mean=1;

  std::default_random_engine generator;
  std::uniform_real_distribution<double> distribution(0.0,1.0);

  std::vector<float> tx_weights;
  float sum_pdf_values = 0;
  float delta = 2.0/(ncores-1);
  float x = 0;
  float max_pdf_value = 0;
  for (int id=0; id < ncores; id++) {
      float y = normal_pdf(x, mean, sigma);
      tx_weights.push_back(y);
      sum_pdf_values += y;
      if (y > max_pdf_value) {
          max_pdf_value = y;
      }
      x += delta;
  }

  for (float& value: tx_weights) {
  	//value *= fuzzy_size / sum_pdf_values;
    value /= max_pdf_value;
  }


    // for (int iter=0; iter < nrolls; iter++) {
    //     for (int curr_node=0; curr_node < fuzzy_size; curr_node++) {
    //         double number = distribution(generator);
    //         if (number <= (1.0/fuzzy_size)) {
    //             ++p[curr_node];
    //         }
    //     }
    // }

    // std::cout << "uniform_real_distribution (0.0,1.0):" << std::endl;
    // std::cout << std::fixed; std::cout.precision(1);

    for (int i=0; i<ncores; ++i) {
    std::cout << i << " (" << tx_weights[i] << "): ";
    std::cout << std::string(tx_weights[i]*nstars/nrolls,'*') << std::endl;
    }

    std::cout << "-------------------" << '\n';

    int n = tx_weights.size();
    int shift = 4;
    int algo_shift = ncores - shift;

    // Create an auxiliary array of twice size.
    std::vector<float> temp_tx_weights(tx_weights.size()*2, 0);

    // Copy tx_weights[] to temp_tx_weights[] two times
    for (int i = 0; i < n; i++) {
        temp_tx_weights[i] = temp_tx_weights[n + i] = tx_weights[i];
    }

    for (int i=0; i<ncores; ++i) {
        std::cout << i << " (" << temp_tx_weights[i] << "): ";
        std::cout << std::string(tx_weights[i]*nstars/nrolls,'*') << std::endl;
    }

    std::cout << "----------" << '\n';

    // save from ind-th index to (n+i)th index
    int i_original = 0;
    for (int i = algo_shift; i < n + algo_shift; i++) {
        tx_weights[i_original] = temp_tx_weights[i];
        i_original++;
    }

    std::cout << "++++++++++" << '\n';

    for (int i=0; i<ncores; ++i) {
        std::cout << i << " (" << tx_weights[i] << "): ";
        std::cout << std::string(tx_weights[i]*nstars/nrolls,'*') << std::endl;
    }

    return 0;
}
