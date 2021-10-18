// uniform_real_distribution
#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>
#include <string>
#include <vector>
#include <climits>
#include <sstream>
#include <chrono>


// split a string into tokens delimited by delim, return vector of strings with tokens
// std::vector<std::string> split(const std::string &s, char delim) {
//     std::vector<std::string> elems;
// 	std::stringstream ss;
//     ss.str(s);
//     std::string item;
//     while (std::getline(ss, item, delim)) {
//         elems.push_back(item);
//     }
//     return elems;
// }

// This program takes two input arguments, the first is the input trace file path, and the second is the output path for the processed traced
// The input trace has a list of pairs, each with a cycle number and a core id. The program takes that list and changes the core id for a randomly
// obtained one, from a normal distribution with standard deviation sigma
int main(int argc, char* argv[])
{
    const int ncores=64;
    const float sigma=100;

    // Initialize the random generator used by rand()
    srand (time(NULL));
    std::default_random_engine generator;
	generator.seed( std::chrono::system_clock::now().time_since_epoch().count() );
    std::normal_distribution<double> distribution(ncores/2, sigma);

    // if there were no input arguments, return error
    if (argc < 3) {
        std::cout << "Please introduce input and output trace file paths." << '\n';
        return 1;
    }
    // else {
    //     // TODO: Do we want to save the parameter values for something??
	// 	std::vector<std::string> in_parameters = split(argv[1], '_');
	// 	std::string parameter_name = in_parameter.at(0);
	// 	std::string parameter_value = in_parameter.at(1);
    // }

    std::string input_file = argv[1];
    std::string output_file = argv[2];
    std::cout << "Reading trace file from " << input_file << " and saving randomized output to " << output_file << std::endl;
    std::ifstream injection_trace_file(input_file);
    std::ofstream randomized_injection_trace_file(output_file);

    if (injection_trace_file.is_open() && randomized_injection_trace_file.is_open()) {
        std::string line;
        long int cycle_number;
        int node_id;
        // we read each line in the trace file
        while (getline(injection_trace_file, line)) {
            std::istringstream iss(line);
            // each line of the trace should have a cycle number and the node ID separated by a space
            if (!(iss >> cycle_number >> node_id)) {
                std::cout << "ERROR: injection_trace_file contains a non-valid line" << std::endl;
                abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
            }
            if (node_id >= ncores) {
                // node IDs in the trace should go from 0 to ncores-1 (maybe we can make it such that ncores is read from the trace as well)
                std::cout << "ERROR: injection_trace_file contains a node ID bigger than the total number of nodes" << std::endl;
                abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
            }

            // We only allow cycle numbers below the limit for a simply 4 byte INT. If we have reached the limit value for a simple 4 byte int, we cut the injection
            // WARNING: do we want to cut the trace when it reaches cycle INT_MAX? Or we want to allow to proceed? If so, we need to convert all INTs to LONG INTs
            if (cycle_number < INT_MAX) {
                // change node id in file for a new one randomly chosen according to hotspotness normal distribution
                double number;
                do {
                    // we keep getting random numbers until one of them is inside the range of core IDs (0 to 63)
                    number = distribution(generator);
                } while (!((number>=0.0) && (number<ncores)));

                randomized_injection_trace_file << cycle_number << " " << int(number) << std::endl;
            }
            else {
                std::cout << "ERROR: cycle number too big" << '\n';
                return 1;
            }
        }
        injection_trace_file.close();
        std::cout << "FINISHED READING TRACE FILE" << std::endl;
    }
    else {
        std::cout << "ERROR: injection_trace_file or randomized_injection_trace_file couldn't be opened" << std::endl;
        return 1;
    }

    injection_trace_file.close();
    randomized_injection_trace_file.close();


    // const int nrolls=10000000;  // number of experiments
    // const int nstars=10000;    // maximum number of stars to distribute
    //
    // int p[ncores]={};
    //
    // for (int i=0; i<nrolls; ++i) {
    //     double number = distribution(generator);
    //     if ((number>=0.0)&&(number<ncores)) ++p[int(number)];
    // }
    //
    // std::cout << "normal_distribution (32,100):" << std::endl;
    //
    // for (int i=0; i<ncores; ++i) {
    //     std::cout << i << "-" << (i+1) << ": ( " << p[i]*100.0/nrolls << " )\t ";
    //     std::cout << std::string(p[i]*nstars/nrolls,'*') << std::endl;
    // }

    return 0;
}
