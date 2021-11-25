//=================================
// include guard
#ifndef GLOBAL_PARAMS_H
#define GLOBAL_PARAMS_H

//=================================
// included dependencies
#include <vector>
#include <fstream>
#include <map>

//=================================
enum class Inj_distribs { poisson, burst, trace };
enum class Mac_protocols { csma_non_p, brs_non_p, tdma_fixed, tdma_weighted, fuzzy_token, token };

// we define the Singleton class used for the parameters of the simulator
class Global_params {
private:
    Global_params(); // Private constructor so that it can not be called
    ~Global_params(); // Private destructor - TODO: does it make sense to be private?
    Global_params(Global_params const&){}; // copy constructor is private
    Global_params& operator=(Global_params const&){}; // assignment operator is private
    static Global_params* s_instance;
    int ncores; // number of cores/nodes in total
    int nchannels; // number of channels in total
    int npackets; // number of packets that the application wants to transmit
    double inj_rate; // desired avg injection rate per node, when averaged among all nodes
    float H; // Hurst exponent, only for Burst injection distribution
    int max_buffer_size; // input maximum buffer size of each node. Processor injects packets at different rates, so buffer allows temporal storage
    Inj_distribs chosen_distrib; // choose kind of injection distribution from the enum defined in Inj_distribs
    Mac_protocols chosen_mac; // choose kind of MAC protocol from the enum defined in Mac_protocols
    int tx_time; // how many cycles does it take for the transceiver to tx a packet (is like if we were adjusting the bitrate)
    int total_injected_packets_chip; // Counter of total amount of packets injected so far
    int total_served_packets_chip; // Counter of total amount of packets served among all the cores so far
    int cycle_injection_stopped; // As soon as we inject the last packet out of npackets, we store the cycle at which the injection finished
    int total_ncycles; // At every iteration we increase this counter, so that we know the total number of cycles executed so far
    std::vector<bool> channel_busy(int nchannels); // This flag will determine if the channel (in a single-channel model) is busy or not
    bool medium_busy; // This flag will determine if the channel (in a single-channel model) is busy or not
    std::vector<std::vector<int>> ids_concurrent_tx_nodes;
	std::vector<std::vector<int>> ids_and_channels_concurrent_tx_nodes;
    std::vector<int> channel_concurrent_tx_nodes;
    bool debugging;
    bool save_trace;
    float sigma; // value of hotspotness, it's the standard deviation of the normal distribution
    int throughput_base_cycles;
    int throughput_tx_cycles;
    std::vector<long int> cycles_collisions_chip; // we register the cycle at which each collision occurs
    std::vector<long int> cycles_served_packets_chip; // we register the cycle at which each packet is served

    // Only for TDMA fixed and weighted. These are global for all nodes, since it imitates the behavior
    // in real applications, where all nodes have the same TDMA information
    int tdma_current_node; // what node is elegible to transmit in the current cycle
    int tdma_slot_size; // In fixed TDMA this will be used always as initial value for tdma_current_node_slot_size, and in weighted will act as a base value
    int tdma_current_node_slot_size; // number of cycles left in the current slot (initialized to slot_size in the fixed version, and to a variable value in weighted)

    // Only for token ring and fuzzy token
    int token_current_node;
    bool txing_token;

    // Only for Fuzzy token
    int fuzzy_size;
    float thr_pure_token;
    float thr_pure_brs;
    std::vector<int> log_curr_token;

    // Only used when reading and writing injections from and to a trace (packet injection trace)
    std::string save_trace_path; // contains path to trace file to be saved
    std::string load_trace_path; // contains path to trace file to be loaded

    // Folder to save the results
    std::string save_results_path; // contains path to trace file to be saved

public:
    static Global_params* Instance();
    int get_ncores();
    void set_ncores(int);
    void set_npackets(int);
    int get_nchannels();
    void set_nchannels(int);
    int get_npackets();
    double get_inj_rate();
    float get_sigma();
    float get_thr_pure_brs();
    float get_thr_pure_token();
    void set_sigma(float);
    void set_tx_time(int);
    void set_inj_rate(double);
    void increase_throughput_base_cycles();
    void increase_throughput_tx_cycles();
    int get_throughput_base_cycles();
    int get_throughput_tx_cycles();


    // H-related methods are specific to Burst injection distribution
    double get_H();
    void set_H(float);

    int get_max_buffer_size();
    Inj_distribs get_chosen_distrib();
    void set_chosen_distrib(Inj_distribs);
    Mac_protocols get_chosen_mac();
    void set_chosen_mac(Mac_protocols);
    int get_tx_time();
    int get_total_injected_packets_chip();
    int get_stationary_served_packets_chip();
    int get_total_served_packets_chip();
    int get_cycle_injection_stopped();
    int get_total_ncycles();
    void increase_total_injected_packets_chip();
    void increase_total_served_packets_chip();
    void set_cycle_injection_stopped(int);
    void increase_total_ncycles();
    bool is_medium_busy(); // Is medium_busy == true?
    bool is_channel_busy(int);
    void set_channel_busy(int);
    void set_channel_idle(int);
    void set_medium_busy(); // set medium_busy = true
    void set_medium_idle(); // set medium_busy = false
    int get_ids_concurrent_tx_nodes_size(); // Get size of ids_concurrent_tx_nodes (number of nodes simultaneously transmitting)
    int get_ids_and_channels_concurrent_tx_nodes_size(); // Get size of ids_and_channels_concurrent_tx_nodes (number of nodes simultaneously transmitting)
    int get_channel_concurrent_tx_nodes_size(); // Get size of channel_concurrent_tx_nodes (number of channels concerned by the simultaneous transmition)
    void push_ids_concurrent_tx_nodes(int nid); // Push new node_id into ids_concurrent_tx_nodes (new node starts transmitting)
    void push_ids_and_channels_concurrent_tx_nodes(std::vector<int> ncid); // Push new node_id and channel_id into ids_and_channels_concurrent_tx_nodes (new node starts transmitting)
    void push_channel_concurrent_tx_nodes(int cid); // Push channel_id into ids_concurrent_tx_nodes
    void flush_ids_concurrent_tx_nodes(); // Delete all elements in ids_concurrent_tx_nodes (no node will be transmitting)
    void flush_ids_and_channels_concurrent_tx_nodes(); // Delete all elements in ids_and_channels_concurrent_tx_nodes (no node will be transmitting)
    void flush_channel_concurrent_tx_nodes(); // Delete all elements in channel_concurrent_tx_nodes (no channel will be used for transmission)
    void delete_ids_concurrent_tx_nodes(int cid, std::vector<int> nid); // Delete all nodes that finished transmitting for the given channel
    int get_unique_ids_concurrent_tx_nodes(); // If we have ensured that there's only 1 tx node, we return its id
    std::vector<int>::const_iterator ids_concurrent_tx_nodes_begin();
    std::vector<int>::const_iterator ids_concurrent_tx_nodes_end();
    std::vector<int>::const_iterator ids_and_channels_concurrent_tx_nodes_begin();
    std::vector<int>::const_iterator ids_and_channels_concurrent_tx_nodes_end();
    bool is_debugging_on(); // check value of debugging
    void set_debugging_on(); // sets debugging=true
    bool is_save_trace_on(); // check value of save_trace
    void set_save_trace_on(); // sets save_trace=true

    // Only for TDMA fixed and weighted. These are global for all nodes, since it imitates the behavior
    // in real applications, where all nodes have the same TDMA information
    int get_tdma_current_node();
    void update_tdma_current_node();
    void set_tdma_slot_size(int);
    int get_tdma_slot_size();
    int get_tdma_current_node_slot_size();
    void set_tdma_current_node_slot_size(int);
    void decrease_tdma_current_node_slot_size();

    // Only for token ring and Fuzzy token
    int get_token_current_node();
    void update_token_current_node();
    void set_token_current_node();
    bool is_txing_token_on();
    void set_txing_token_on();
    void set_txing_token_off();

    // Only for BRS and fuzzy token
    void increase_counter_collisions();
    long int get_stationary_collisions_chip();

    // Only for Fuzzy token
    int get_fuzzy_size();
    void set_fuzzy_size(); // Initialize the fuzzy size to the number of cores
    void increase_fuzzy_size();
    void decrease_fuzzy_size();
    void add_log_code(int);
    int get_max_code();
    void reset_log();
    void calculate_normal_distribution();
    void calculate_tx_weights();
    void calculate_fuzzy_area_all_one_distribution();
    void calculate_fuzzy_area_all_one_div_fuzzysize_distribution();
    void rotate_tx_weights(int);
    std::vector<float> tx_weights; // TODO: make a getter and setter method to access this vector!!
    void reset_mapping_node_ids();
    void randomize_mapping_node_ids();
    std::vector<int> mapping_node_ids; // TODO: make a getter and setter method to access this vector!!

    // Only for statistics and testing purposes
    std::string get_save_trace_path();
    std::string get_load_trace_path();
    void set_save_trace_path(std::string);
    void set_load_trace_path(std::string);
    // Only used when reading injections from trace (packet injection trace)
    // We use this table to save a pair of cycle with all node IDs that injected a packet in it
    std::map<int, std::vector<int>> injections_lookup_table; // TODO: this should be private and I should define the accesses with public methods!!!

    std::string get_save_results_path();
    void set_save_results_path(std::string);

    std::vector<int> pckt_latencies_chip; // TODO: THIS SHOULD BE PRIVATE. here we store the cycles that each packet in the chip it takes to be transmitted

};

#endif // GLOBAL_PARAMS_H
