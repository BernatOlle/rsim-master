#include <iostream> // input, output
#include <random> // default_random_engine
#include <chrono> // now
#include <math.h> // ceil, pow
#include <vector> // vector of nodes
#include <queue> // in_buffer

int main()
{
	const double inj_rate = 0.01; // mean of the poisson process. How many packets we tx per node per cycle in average
	const int avg_distance_between_arrivals = ceil(1/inj_rate);

  	int seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed); // construct a trivial random generator engine from a time-based seed

	std::exponential_distribution<double> inj_distribution(inj_rate);
	int plot[avg_distance_between_arrivals*10] = {};
	int i = 0;
	int ncycles = 10000;
	int temp;
	while(i<ncycles) {
		int number = ceil(inj_distribution(generator));
		if(number < avg_distance_between_arrivals*10) { ++plot[number]; }
		i++;
	}

	for (i=0; i<avg_distance_between_arrivals*10; ++i) {
	    // std::cout << i << " ("<< plot[i]*1000.0/ncycles <<")\t\t ";
	    std::cout << i << "\t\t" << std::string(plot[i]*1000.0/ncycles,'*') << std::endl;
	  }

    int occurr = 0;
	float avg = 0;
  	for (i=0; i<avg_distance_between_arrivals*10; ++i) {
		avg += plot[i]*i;
		occurr += plot[i];
	}

	std::cout << "Theoretical average (cycles between packets): " << 1.0/inj_rate << '\n';
	std::cout << "Real average: " << avg/occurr << '\n';

	return 0;
}
