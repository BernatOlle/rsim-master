//=================================
// include guard
#ifndef UTILITIES_H
#define UTILITIES_H

//=================================
// included dependencies
#include "headers.hpp"
#include "node.hpp"

//=================================
//int rsim_params::total_injected_packets_chip;
//int rsim_params::total_served_packets_chip;

// split a string into tokens delimited by delim, return vector of strings with tokens
std::vector<std::string> split(const std::string &s, char delim);

// splits a string into multiple key-value pairs, and stores each pair into a map entry
// first delimiter separates the different pairs, second delimiter separates the key from the value
std::map<std::string, std::string> mappify(std::string const& s, char const& delim1, char const& delim2);

// normal distribution PDF
float normal_pdf(float x, float m, float s);

// if value in range (including edges)
bool value_in_range(int value, int a, int b);

// calculate the geometric mean of the input vector
double geometric_mean(std::vector<double> const & in_vector);

// calculate the arithmetic mean of the input vector
double arithmetic_mean(std::vector<double> const & in_vector);

// print the number of injected packets and injection rate (total_injected_packets/ncycles) for each node
void print_statistics_per_node(const std::vector<Node*>&);

// print the type of distribution that is being used
void print_distribution();

// print the type of MAC protocol that is being used
void print_mac_protocol();

// print the tx_time of the packets (bitrate/packet_size)
void print_tx_time();

// print threshold of fuzzy area to switch to pure BRS mode
void print_thr_pure_brs();

// print threshold of fuzzy area to switch to pure Token mode
void print_thr_pure_token();

void print_chan();

void print_assig();

// print all input parameters
void print_input_parameters();

// print the total number of nodes/cores
void print_ncores();

// print the average injection rate per node
void print_inj_rate();

// print sigma, which is the hotspotness value (standard deviation of the normal distribution)
void print_sigma();

// print the Hurst exponent
void print_hurst_exponent();

// print the number of packets that the simulator will transmit
void print_npackets();

// print the maximum size of the buffer (how many packets can fit without being served)
void print_max_buffer_size();

// print the slot_size of TDMA
void print_slot_size();

void plot_pckt_latencies_chip(const std::vector<Node*>&);

// calculate the energy per bit based on the number of retransmissions
float get_energy_bit();

void save_pckt_latencies_histogram_chip(const std::vector<Node*>&, std::ofstream&);

void save_pckt_latencies_evolution_chip(std::ofstream&);

#endif // UTILITIES_H
