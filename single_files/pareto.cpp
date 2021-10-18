#include <iostream> // input, output
#include <random> // default_random_engine
#include <chrono> // now
#include <math.h> // ceil, pow
#include <vector> // vector of nodes
#include <queue> // in_buffer

int main()
{
  	int seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed); // construct a trivial random generator engine from a time-based seed
	std::uniform_real_distribution<double> distribution(0.0,1.0);
	int plot[10] = {};
	float H=1;
	float a=3-2*H;
	float bon=1;
	float avg_inj_rate = 0.1;
	float boff=bon*(1.0/avg_inj_rate-1);
	float b=boff;
	int i =0;
	int ncycles = 100000;
	int temp;
	while(i<ncycles) {
		double number = distribution(generator);
		temp = int(round(b/(pow(1-number,1.0/a))));
		if(temp < 10) {	++plot[temp]; }
		i++;
	}
	
	for (i=0; i<8; ++i) {
	    std::cout << i << " ("<< plot[i]*100.0/ncycles <<")\t\t ";
	    std::cout << std::string(plot[i]*100.0/ncycles,'*') << std::endl;
	  }
	return 0;
}

