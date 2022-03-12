#include "utilities.hpp"
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <limits> // numeric_limits<double>::max()

// print some statistics for each node in the chip
void print_statistics_per_node(const std::vector<Node*>& chip) {
	int total_injected_packets_chip = 0;
	std::vector<double> pckt_latencies_chip; // here we store the avg packet latency of every node
	int total_injected_packets_node;
	float theoretical_node_avg_inj_rate;
	float real_node_avg_inj_rate;
	double total_theoretical_chip_avg_inj_rate = 0;
	double total_real_chip_avg_inj_rate = 0;
	double avg_pckt_latency_node; // geometric mean of total_latencies_radicand_node^(1/total_served_packets_node)
	double total_weighted_sum_stationary_latencies_chip = 0;
	int total_sum_stationary_packets_chip = 0;

	//std::cout << "--------------------------------------------------" << std::endl;
	for (std::vector<Node*>::const_iterator curr_node = chip.begin(); curr_node != chip.end(); ++curr_node) {
		total_injected_packets_node = (*curr_node)->get_total_injected_packets_node();
		total_injected_packets_chip += total_injected_packets_node;

		// we calculate the theoretical injection rate unless we're reading from a trace
		if (Global_params::Instance()->get_chosen_distrib() != Inj_distribs::trace) {
			theoretical_node_avg_inj_rate = (*curr_node)->get_node_inj_rate();
			total_theoretical_chip_avg_inj_rate += theoretical_node_avg_inj_rate;
		}

		real_node_avg_inj_rate = total_injected_packets_node/(float)Global_params::Instance()->get_cycle_injection_stopped();
		total_real_chip_avg_inj_rate += real_node_avg_inj_rate;

		avg_pckt_latency_node = (*curr_node)->get_mean_stationary_pckt_latencies_node(); // mean of all stationary latencies of a node
		pckt_latencies_chip.push_back(avg_pckt_latency_node); // we store the avg packet latency for the current node

		if ((total_injected_packets_node - (*curr_node)->get_total_served_packets_node()) > 0) {
			std::cout << "!!!!!!!!! We finished the program and we still had packets to serve";
		}
		/*
		std::cout << (*curr_node)->get_id() << ":";
		std::cout << " Inj_pckts(all): " << total_injected_packets_node; // total number of injected packets including warm up and cool down
		std::cout << "\t Avg_latency_pckt(stationary): " << avg_pckt_latency_node << " cycles. "; // avg latency per packet OF ONLY WARM UP AND COOL DOWN
		if (Global_params::Instance()->get_chosen_distrib() == Inj_distribs::trace) {
			std::cout << " Theoretical Avg_inj_rate: N/A";
		}
		else {
			std::cout << " Theoretical Avg_inj_rate: " << theoretical_node_avg_inj_rate << " pckts/cycle ";
		}
		std::cout << " Real Avg_inj_rate: "	<< real_node_avg_inj_rate << " pckts/cycle" << std::endl;*/
		//(*curr_node)->plot_pckt_latencies_node(); // this shows the distribution of latencies for this node
	} // End of for-each node

	std::ofstream outdata;
	std::ofstream outdata2;
    std::ofstream outdata3;
    std::ofstream outdata4;
    std::ofstream outdata5;


	std::string path_results, path_results2, path_results3, path_results4, path_results5;

	// if we received the results folder path from command line, we use that one
	//std::cout << "--------------------------------------------------" << std::endl;
	if (Global_params::Instance()->get_save_results_path().compare("") != 0) {
		std::cout << "Saving results in " << Global_params::Instance()->get_save_results_path() << std::endl;
		path_results = Global_params::Instance()->get_save_results_path() + "/data_throughput/";
		path_results2 = Global_params::Instance()->get_save_results_path() + "/data_latencies/";
        path_results3 = Global_params::Instance()->get_save_results_path() + "/data_energy/";
        path_results4 = Global_params::Instance()->get_save_results_path() + "/data_latencies_histogram/";
        path_results5 = Global_params::Instance()->get_save_results_path() + "/data_latencies_evolution/";

	}
	// otherwise we use the default path
	else {
		//std::cout << "Saving results in defaultAA folder ./results" << Global_params::Instance()->get_save_results_path() << std::endl;
		path_results = "./results/data_throughput/";
		path_results2 = "./results/data_latencies/";
        path_results3 = "./results/data_energy/";
        path_results4 = "./results/data_latencies_histogram/";
        path_results5 = "./results/data_latencies_evolution/";

	}

	// if we're dealing with a trace we try to read the parameters from the file name, just for the sake of saving the results in the same fashion as non-trace executions
	if (Global_params::Instance()->get_chosen_distrib() == Inj_distribs::trace) {
		std::string trace_file_full_path (Global_params::Instance()->get_load_trace_path());

        std::string file_extension (".txt");
        std::string::size_type pos;
		// if we find the extension at the end of the filename, we remove it
        if ((pos = trace_file_full_path.find(file_extension)) != std::string::npos) {
            // std::cout << "extension found at " << pos << '\n';
            trace_file_full_path.erase(pos, file_extension.length());
            // std::cout << "Extension removed. New file name: " << trace_file_full_path << '\n';
        }

		// first divide the path into each of the parts of it, delimited by '/'
		std::vector<std::string> tracefile_name_parts = split(trace_file_full_path, '/');
		std::string tracefile_name = tracefile_name_parts.back(); // then we get the filename, which is always at the end of the path
		std::map<std::string, std::string> map_of_tracefile_name = mappify(tracefile_name, '_', '-'); // we create a map of parameter-value pairs

		std::map<std::string, std::string>::iterator it;
		it = map_of_tracefile_name.find("H");
		if ( it != map_of_tracefile_name.end() ) {
			Global_params::Instance()->set_H( stof(it->second) );
			std::cout << "Value read from trace file name, H=" << Global_params::Instance()->get_H() << '\n';
		}
		it = map_of_tracefile_name.find("sigma");
		if ( it != map_of_tracefile_name.end() ) {
			Global_params::Instance()->set_sigma( stof(it->second) );
			std::cout << "Value read from trace file name, sigma=" << Global_params::Instance()->get_sigma() << '\n';
		}
		it = map_of_tracefile_name.find("injrate");
		if ( it != map_of_tracefile_name.end() ) {
			Global_params::Instance()->set_inj_rate( stof(it->second) );
			std::cout << "Value read from trace file name, inj_rate=" << Global_params::Instance()->get_inj_rate();
			std::cout << " (global inj_rate per chip " << Global_params::Instance()->get_inj_rate()*Global_params::Instance()->get_ncores() << ")\n";
		}
	}
	path_results += "C" + std::to_string(Global_params::Instance()->get_nchannels()) + "/";
	path_results2 += "C" + std::to_string(Global_params::Instance()->get_nchannels()) + "/";
		path_results3 += "C" + std::to_string(Global_params::Instance()->get_nchannels()) + "/";
		path_results4 += "C" + std::to_string(Global_params::Instance()->get_nchannels()) + "/";
		path_results5 += "C" + std::to_string(Global_params::Instance()->get_nchannels()) + "/";

		path_results += "AS" + std::to_string(Global_params::Instance()->get_chosen_assig()) + "/";
		path_results2 += "AS" + std::to_string(Global_params::Instance()->get_chosen_assig()) + "/";
			path_results3 += "AS" + std::to_string(Global_params::Instance()->get_chosen_assig()) + "/";
			path_results4 += "AS" + std::to_string(Global_params::Instance()->get_chosen_assig()) + "/";
			path_results5 += "AS" + std::to_string(Global_params::Instance()->get_chosen_assig()) + "/";

	path_results += "N" + std::to_string(Global_params::Instance()->get_ncores()) + "/";
	path_results2 += "N" + std::to_string(Global_params::Instance()->get_ncores()) + "/";
    path_results3 += "N" + std::to_string(Global_params::Instance()->get_ncores()) + "/";
    path_results4 += "N" + std::to_string(Global_params::Instance()->get_ncores()) + "/";
    path_results5 += "N" + std::to_string(Global_params::Instance()->get_ncores()) + "/";

	path_results += "S" + std::to_string(Global_params::Instance()->get_sigma()) + "/";
	path_results2 += "S" + std::to_string(Global_params::Instance()->get_sigma()) + "/";
    path_results3 += "S" + std::to_string(Global_params::Instance()->get_sigma()) + "/";
    path_results4 += "S" + std::to_string(Global_params::Instance()->get_sigma()) + "/";
    path_results5 += "S" + std::to_string(Global_params::Instance()->get_sigma()) + "/";


	path_results += "H" + std::to_string(Global_params::Instance()->get_H()) + "/";
	path_results2 += "H" + std::to_string(Global_params::Instance()->get_H()) + "/";
    path_results3 += "H" + std::to_string(Global_params::Instance()->get_H()) + "/";
    path_results4 += "H" + std::to_string(Global_params::Instance()->get_H()) + "/";
    path_results5 += "H" + std::to_string(Global_params::Instance()->get_H()) + "/";


	if (Global_params::Instance()->get_chosen_mac() == Mac_protocols::brs_non_p) {
		path_results += "brs_non_p/";
		path_results2 += "brs_non_p/";
        path_results3 += "brs_non_p/";
        path_results4 += "brs_non_p/";
        path_results5 += "brs_non_p/";

	} else if (Global_params::Instance()->get_chosen_mac() == Mac_protocols::tdma_fixed) {
		path_results += "tdma_fixed/";
		path_results2 += "tdma_fixed/";
        path_results3 += "tdma_fixed/";
        path_results4 += "tdma_fixed/";
        path_results5 += "tdma_fixed/";

	} else if (Global_params::Instance()->get_chosen_mac() == Mac_protocols::tdma_weighted) {
		path_results += "tdma_weighted/";
		path_results2 += "tdma_weighted/";
        path_results3 += "tdma_weighted/";
        path_results4 += "tdma_weighted/";
        path_results5 += "tdma_weighted/";

	} else if (Global_params::Instance()->get_chosen_mac() == Mac_protocols::fuzzy_token) {
		path_results += "fuzzy_token/";
		path_results2 += "fuzzy_token/";
        path_results3 += "fuzzy_token/";
        path_results4 += "fuzzy_token/";
        path_results5 += "fuzzy_token/";

	} else if (Global_params::Instance()->get_chosen_mac() == Mac_protocols::token) {
		path_results += "token/";
		path_results2 += "token/";
        path_results3 += "token/";
        path_results4 += "token/";
        path_results5 += "token/";

	}

	// FIXME: this way of saving folders might be wrong. Make sure it's right
	std::string path_cmd = "mkdir -p "+path_results;
	std::string path_cmd2 = "mkdir -p "+path_results2;
    std::string path_cmd3 = "mkdir -p "+path_results3;
    std::string path_cmd4 = "mkdir -p "+path_results4;
    std::string path_cmd5 = "mkdir -p "+path_results5;



	char *y = new char[path_cmd.length() + 1];
	char *y2 = new char[path_cmd2.length() + 1];
    char *y3 = new char[path_cmd3.length() + 1];
    char *y4 = new char[path_cmd4.length() + 1];
    char *y5 = new char[path_cmd5.length() + 1];



	strcpy(y, path_cmd.c_str());
	strcpy(y2, path_cmd2.c_str());
    strcpy(y3, path_cmd3.c_str());
    strcpy(y4, path_cmd4.c_str());
    strcpy(y5, path_cmd5.c_str());



	system(y);
	system(y2);
    system(y3);
    system(y4);
    system(y5);



	path_results += std::to_string(Global_params::Instance()->get_inj_rate());
	path_results2 += std::to_string(Global_params::Instance()->get_inj_rate());
    path_results3 += std::to_string(Global_params::Instance()->get_inj_rate());
    path_results4 += std::to_string(Global_params::Instance()->get_inj_rate());
    path_results5 += std::to_string(Global_params::Instance()->get_inj_rate());



	//path_results += ".csv";
	outdata.open(path_results, std::ios::app);
	outdata2.open(path_results2, std::ios::app);
    outdata3.open(path_results3, std::ios::app);
    outdata4.open(path_results4, std::ios::app);
    outdata5.open(path_results5, std::ios::app);



	// calculate stationary total chip arithmetic average latency per packet
	total_weighted_sum_stationary_latencies_chip = 0;
	total_sum_stationary_packets_chip = 0;
	// for each node in the chip
	for (std::vector<Node*>::const_iterator curr_node = chip.begin(); curr_node != chip.end(); ++curr_node) {
		int node_stationary_packets = (*curr_node)->get_total_stationary_packets_node();
		total_weighted_sum_stationary_latencies_chip += node_stationary_packets * ((*curr_node)->get_mean_stationary_pckt_latencies_node());
		total_sum_stationary_packets_chip += node_stationary_packets;
		//std::cout <<"Totalç-sum: "<<node_stationary_packets << '\n';
	}
//s//td::cout <<"Totalç-sum: "<<total_sum_stationary_packets_chip << '\n';

	// Data: save t	hroughput
	outdata << 1.0*Global_params::Instance()->get_throughput_tx_cycles() / Global_params::Instance()->get_throughput_base_cycles() << std::endl;
	outdata.close();

	// Data 2: save average latency per packet per chip
	// outdata2 << geometric_mean(pckt_latencies_chip) << std::endl; // save geomean (not weighted) of all nodes
	outdata2 << total_weighted_sum_stationary_latencies_chip / total_sum_stationary_packets_chip << std::endl; // save weighted arithmetic latency of all nodes
	outdata2.close();

    // Data 3: save average energy per bit
	outdata3 << get_energy_bit() << std::endl; // save average energy per bit of stationary period
	outdata3.close();

    // Data 4: for each interval, save a pair of max cycle in interval and number of occurrences (in percentage) in each line
	save_pckt_latencies_histogram_chip(chip, outdata4);
	outdata4.close();

    // Data 5:
	save_pckt_latencies_evolution_chip(outdata5);
	outdata5.close();
/*
	std::cout << "--------------------------------------------------" << std::endl;
	std::cout << "Total packets per chip: " << total_injected_packets_chip << "(injected), " << Global_params::Instance()->get_total_served_packets_chip() << " (served)" << std::endl;
	if (Global_params::Instance()->get_chosen_distrib() == Inj_distribs::trace) {
		std::cout << "Total theoretical avg inj rate per chip: N/A" << std::endl;
	}
	else {
		std::cout << "Total theoretical avg inj rate per chip: " << total_theoretical_chip_avg_inj_rate << " pckts/cycle" << std::endl;
	}
	std::cout << "Total real avg inj rate per chip: " << total_real_chip_avg_inj_rate  << " pckts/cycle" << std::endl;
	std::cout << "Total weighted arithmetic latency per packet: " << total_weighted_sum_stationary_latencies_chip / total_sum_stationary_packets_chip << " cycles/packet" << std::endl;
	std::cout << "Total arithmetic mean latency per packet: " << arithmetic_mean(pckt_latencies_chip) << " cycles" << std::endl;
	std::cout << "Total geometric mean latency per packet: " << geometric_mean(pckt_latencies_chip) << " cycles" << std::endl;
	std::cout << "Throughput: " << 1.0*Global_params::Instance()->get_throughput_tx_cycles() / Global_params::Instance()->get_throughput_base_cycles() << std::endl;
	std::cout << "Cycle at which injection finished: " << Global_params::Instance()->get_cycle_injection_stopped() << std::endl;

    long int stationary_collisions_chip = Global_params::Instance()->get_stationary_collisions_chip();
    long int stationary_served_packets_chip = Global_params::Instance()->get_stationary_served_packets_chip();
	std::cout << "Total number of collisions (in stationary period): " << Global_params::Instance()->get_stationary_collisions_chip() << std::endl;
    std::cout << "Total number of served packets (in stationary period): " << Global_params::Instance()->get_stationary_served_packets_chip() << std::endl;
    std::cout << "Average number of retries per successful served packet (in stationary period): " << 1.0*Global_params::Instance()->get_stationary_collisions_chip() / Global_params::Instance()->get_stationary_served_packets_chip() << std::endl;
    std::cout << "Average energy per bit (in stationary period): " << get_energy_bit() << " picoJoules" << std::endl;
    std::cout << "Total execution time: " << Global_params::Instance()->get_total_ncycles() << " cycles" << std::endl;*/
	//plot_pckt_latencies_chip(chip);
}

float get_energy_bit() {
    // number of retries
    float N_re = 1.0*Global_params::Instance()->get_stationary_collisions_chip() / Global_params::Instance()->get_stationary_served_packets_chip();

    int N = Global_params::Instance()->get_ncores();
    int L_pre = 1; // length of preamble in number of cycles
    int L_tx = Global_params::Instance()->get_tx_time(); // length of full packet
    float E_tx = 1.97; // given in picojoules (obtained from P_tx/T_bit)
    float E_rx = E_tx; // according to Replica paper
    float E_ok = E_tx + (N - 1)*E_rx;
    float E_bit = E_ok*(1 + N_re*L_pre/L_tx);

    return E_bit;
}

void save_pckt_latencies_evolution_chip(std::ofstream& outdata) {
	for (const int & packet_latency : Global_params::Instance()->pckt_latencies_chip) {
        outdata << packet_latency << std::endl;
	}
}

void save_pckt_latencies_histogram_chip(const std::vector<Node*>& chip, std::ofstream& outdata) {
    const long int nintervals = 100; // number of intervals
	const int interval_size = 5;
	long int npackets = 0; // count total number of packets

	double max_latency = 0; // we start with the minimum possible max latency value

    int p[nintervals]={};
	// for every node in the chip
	for (std::vector<Node*>::const_iterator curr_node = chip.begin(); curr_node != chip.end(); ++curr_node) {
		// for every packet in the node
		for (const double & element : (*curr_node)->pckt_latencies_node) {
			// if we get a latency value bigger than the one corresponding to the last bin, we put it in the last bin
			if (element >= nintervals*interval_size) {
				p[nintervals-1]++;
			}
			else {
				p[(long int)(element / interval_size)]++;
			}
			npackets++;
			max_latency = std::max(element, max_latency);
		}
	}

	for (long int i=0; i<nintervals; ++i) {
        // for each interval, save a pair of the highest cycle number of the interval, and the number of occurrences of the interval (in percentage)
		if (i < (nintervals-1)) {
			outdata << (i+1)*interval_size << " " << 100.0*p[i]/npackets << std::endl;
		}
		// if we're in the last bin we print the range up to the maximum latency value
		else {
            outdata << max_latency << " " << 100.0*p[i]/npackets << std::endl;
		}
    }
}

void plot_pckt_latencies_chip(const std::vector<Node*>& chip) {
    const int nstars=1000;    // maximum number of stars to distribute
    const int nintervals=100; // number of intervals
	const int interval_size = 5;
	long int npackets = 0; // count total number of packets

	double max_latency = 0; // we start with the minimum possible max latency value

    int p[nintervals]={};
	// for every node in the chip
	for (std::vector<Node*>::const_iterator curr_node = chip.begin(); curr_node != chip.end(); ++curr_node) {
		// for every packet in the node
		for (const double & element : (*curr_node)->pckt_latencies_node) {
			// if we get a latency value bigger than the one corresponding to the last bin, we put it in the last bin
			if (element >= nintervals*interval_size) {
				p[nintervals-1]++;
			}
			else {
				p[(int)(element / interval_size)]++;
			}
			npackets++;
			max_latency = std::max(element, max_latency);
		}
	}

	std::cout << "==================================================" << std::endl;
	std::cout << "Packet latency distribution (per chip), " << nintervals << " intervals of " << interval_size << " cycles each:" << std::endl;

	for (int i=0; i<nintervals; ++i) {
		if (i < (nintervals-1)) {
			std::cout << i*interval_size << "\t - " << (i+1)*interval_size - 0.01 << ": \t" << p[i] << "\t";
		}
		// if we're in the last bin we print the range up to the maximum latency value
		else {
			std::cout << i*interval_size << "\t - " << max_latency << ": \t" << p[i] << "\t";
		}

        std::cout << std::string(p[i]*nstars/npackets,'*') << std::endl;
    }


	// double max_latency = 0; // we start with the minimum possible max latency value
    // double min_latency = std::numeric_limits<double>::max(); // we start with the maximum possible min latency value
	//
	// for (std::vector<Node*>::const_iterator curr_node = chip.begin(); curr_node != chip.end(); ++curr_node) {
	// 	double max_latency_node = *(std::max_element((*curr_node)->pckt_latencies_node.begin(), (*curr_node)->pckt_latencies_node.end()));
	//     double min_latency_node = *(std::min_element((*curr_node)->pckt_latencies_node.begin(), (*curr_node)->pckt_latencies_node.end()));
	//
	// 	max_latency = std::max(max_latency_node, max_latency);
	// 	min_latency = std::min(min_latency_node, min_latency);
	// }
	//
    // double interval_size = (max_latency - min_latency)/(double)nintervals;
	//
    // for (const double & element : pckt_latencies_node) {
    //     if (element >= min_latency && element < min_latency+interval_size) ++p[0];
    //     else if (element >= min_latency+interval_size && element < min_latency+2*interval_size) ++p[1];
    //     else if (element >= min_latency+2*interval_size && element < min_latency+3*interval_size) ++p[2];
    //     else if (element >= min_latency+3*interval_size && element < min_latency+4*interval_size) ++p[3];
    //     else if (element >= min_latency+4*interval_size && element < min_latency+5*interval_size) ++p[4];
    //     else if (element >= min_latency+5*interval_size && element < min_latency+6*interval_size) ++p[5];
    //     else if (element >= min_latency+6*interval_size && element < min_latency+7*interval_size) ++p[6];
    //     else if (element >= min_latency+7*interval_size && element < min_latency+8*interval_size) ++p[7];
    //     else if (element >= min_latency+8*interval_size && element < min_latency+9*interval_size) ++p[8];
    //     else if (element >= min_latency+9*interval_size && element <= max_latency) ++p[9];
    // }
	//
    // for (int i=0; i<nintervals; ++i) {
    //     std::cout << min_latency+i*interval_size << "-" << min_latency+(i+1)*interval_size << ": ";
    //     std::cout << std::string(p[i]*nstars/pckt_latencies_node.size(),'*') << std::endl;
    // }
}

// normal distribution PDF
float normal_pdf(float x, float m, float s) {
    static const float inv_sqrt_2pi = 0.3989422804014327;
    float a = (x - m) / s;

    return inv_sqrt_2pi / s * exp(-0.5f * a * a);
}

// calculate the geometric mean of the input vector
double geometric_mean(std::vector<double> const & in_vector) {
	int n_elements = 0;

	// count number of non-zero elements
	for (const double &element : in_vector) { // access by reference to avoid copying
		if (element > 0) {
        	n_elements++;
		}
    }

	double result = 1.0;
	for (const double &element : in_vector) { // access by reference to avoid copying
		if (element > 0) {
        	result *= pow((double)element, 1.0/n_elements);
		}
    }

	return result;
}

// calculate the arithmetic mean of the input vector
double arithmetic_mean(std::vector<double> const & in_vector) {
	int n_elements = in_vector.size();
	double result = 0.0;
	for (const double &element : in_vector) { // access by reference to avoid copying
        result += (double)element;
    }

	return (result/n_elements);
}

// print the type of distribution that is being used
void print_distribution() {
	switch(Global_params::Instance()->get_chosen_distrib()) {
		case Inj_distribs::poisson	: std::cout<<"Distrib = Poisson"<<std::endl; break;
		case Inj_distribs::burst	: std::cout<<"Distrib = Burst"<<std::endl; break;
	}
}

// print the type of MAC protocol that is being used
void print_mac_protocol() {
	switch(Global_params::Instance()->get_chosen_mac()) {
		case Mac_protocols::csma_non_p	: std::cout<<"MAC = CSMA non-persistent"<<std::endl; break;
		case Mac_protocols::brs_non_p	: std::cout<<"MAC = BRS-MAC non-persistent"<<std::endl; break;
		case Mac_protocols::tdma_fixed	: std::cout<<"MAC = TDMA with fixed slots"<<std::endl; break;
		case Mac_protocols::tdma_weighted	: std::cout<<"MAC = TDMA with weighted slots"<<std::endl; break;
		case Mac_protocols::fuzzy_token	: std::cout<<"MAC = Fuzzy-Token"<<std::endl; break;
		case Mac_protocols::token	: std::cout<<"MAC = Token"<<std::endl; break;
	}
}

// print all input parameters
void print_input_parameters() {
	/*
	print_ncores();
	print_distribution();
	print_mac_protocol();
	print_inj_rate();
	print_thr_pure_brs();
	print_thr_pure_token();
	print_sigma();
	print_assig();
	print_chan();
	print_tx_time();
	print_hurst_exponent();
	print_npackets();
	print_max_buffer_size();
	print_slot_size();*/
}
// print the total number of nodes/cores
void print_ncores() {
	if (Global_params::Instance()->get_ncores() == 0) {
		std::cout << "ERROR: ncores not defined" << std::endl;
		abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
	}
	std::cout << "Number of cores = " << Global_params::Instance()->get_ncores() << std::endl;
}

// print the tx_time of the packets (bitrate/packet_size)
void print_tx_time() {
	if (Global_params::Instance()->get_tx_time() == 0) {
		std::cout << "ERROR: tx_time not defined" << std::endl;
		abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
	}
	std::cout << "Tx time = " << Global_params::Instance()->get_tx_time() << std::endl;
}

// print the average injection rate per node
void print_inj_rate() {
	std::cout << "Avg injection rate per core = " << Global_params::Instance()->get_inj_rate() << " packets/cycle per core" << std::endl;
}

// print sigma, which is the hotspotness value (standard deviation of the normal distribution)
void print_sigma() {
	std::cout << "Sigma = " << Global_params::Instance()->get_sigma() << std::endl;
}

// print the number of packets that the simulator will transmit
void print_npackets() {
	std::cout << "Number of packets to tx = " << Global_params::Instance()->get_npackets() << std::endl;
}

// print the maximum size of the buffer (how many packets can fit without being served)
void print_max_buffer_size() {
	std::cout << "Max buffer size = " << Global_params::Instance()->get_max_buffer_size() << " packets" << std::endl;
}

// print the Hurst exponent
void print_hurst_exponent() {
	std::cout << "H = " << Global_params::Instance()->get_H() << std::endl;
}

// print the slot_size of TDMA
void print_slot_size() {
	std::cout << "Slot size = " << Global_params::Instance()->get_tdma_slot_size() << " cycles" << std::endl;
}

// print threshold of fuzzy area to switch to pure Token mode
void print_thr_pure_token() {
	std::cout << "Threshold fuzzy size for pure token = " << Global_params::Instance()->get_thr_pure_token() << std::endl;
}

void print_assig() {
	std::cout << "Assig = " << Global_params::Instance()->get_chosen_assig() << std::endl;
}

void print_chan() {
	std::cout << "Channels= " << Global_params::Instance()->get_nchannels() << std::endl;
}



// print all input parameters
void print_thr_pure_brs() {
	std::cout << "Threshold fuzzy size for pure BRS = " << Global_params::Instance()->get_thr_pure_brs() << std::endl;
}

// split a string into tokens delimited by delim, return vector of strings with tokens
std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
	std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

// splits a string into multiple key-value pairs, and stores each pair into a map entry
// first delimiter separates the different pairs, second delimiter separates the key from the value
std::map<std::string, std::string> mappify(std::string const& input_string, char const& delim1, char const& delim2) {
    std::map<std::string, std::string> filled_map;

    std::string::size_type pair_pos = 0;
    std::string::size_type pair_end;

    // Save the position of the last character of a pair (delimited by delim1)
    // And do it until we reach the end of the input string
    do {
        pair_end = input_string.find(delim1, pair_pos);
        // std::cout << "pair_pos " << pair_pos << ", pair_end " << pair_end << '\n';
        std::string pair = input_string.substr(pair_pos, pair_end - pair_pos);
        // std::cout << pair << '\n';
        std::string::size_type val_pos = pair.find(delim2);
        std::string key, value;
        if(val_pos == std::string::npos) {
            key = pair;
            value = "";
        }
        else {
            key = pair.substr(0, val_pos);
            value = pair.substr(val_pos+1);
        }
        filled_map.emplace(key, value);
        pair_pos = pair_end + 1;
    } while (pair_end != std::string::npos);

    return filled_map;
}

// if value in range (including edges)
bool value_in_range(int value, int a, int b) {
	return (value >= a && value <= b);
}
