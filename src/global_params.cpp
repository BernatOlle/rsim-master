#include "global_params.hpp"
#include <stddef.h>
#include "ini_parser.hpp"
#include "utilities.hpp"
#include <vector>

// Global static pointer used to ensure a single instance of the class.
Global_params* Global_params::s_instance = NULL;

/*  This function is called to create an instance of the class.
    Calling the constructor publicly is not allowed. The constructor
    is private and is only called by this Instance function.
*/
Global_params* Global_params::Instance() {
    if (!s_instance) { // Only allow one instance of class to be generated.
        s_instance = new Global_params;
    }
    return s_instance;
}

Global_params::Global_params() {
    // Fill the input parameters from the parameters.ini file
    // TODO: WE CAN'T HARD-CODE URL OF PARAMETERS.INI, WE NEED TO GET IT FROM USER ARGUMENT AT CALL TIME. AND PRINT ERROR OTHERWISE.
    config cfg("parameters.ini");
    if (cfg.get_section("input") != NULL) {
    	if (cfg.get_value("input", "inj_distribution").compare("poisson") == 0) {
    	       chosen_distrib = Inj_distribs::poisson;
    	}
        else if (cfg.get_value("input", "inj_distribution").compare("burst") == 0) {
    		chosen_distrib = Inj_distribs::burst;
    		if (cfg.get_section("burst") != NULL) {
    			H = stof(cfg.get_value("burst", "H"));
    		} else {
    			std::cout << "ERROR: Burst parameters not found" << std::endl;
                abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
    		}
    	}
        else if (cfg.get_value("input", "inj_distribution").compare("trace") == 0) {
    		chosen_distrib = Inj_distribs::trace;
            if (cfg.get_section("trace") != NULL) {
                if (cfg.get_value("trace", "load_trace_path").compare("") != 0) {
                    load_trace_path = cfg.get_value("trace", "load_trace_path");
                }
                if (cfg.get_value("trace", "save_trace_path").compare("") != 0) {
                    save_trace_path = cfg.get_value("trace", "save_trace_path");
                }
    		}
    	}
        else {
    		std::cout << "ERROR: No distribution type was found" << std::endl;
            abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
    	}

    	inj_rate = stof(cfg.get_value("input", "inj_rate"));
        sigma = stof(cfg.get_value("input", "sigma"));
    	ncores = stoi(cfg.get_value("input", "ncores"));
        // TODO: what happens if user doesn't enter npackets? it should give error for poisson and burst but be fine for trace
    	npackets = stoi(cfg.get_value("input", "npackets"));
        if (cfg.get_value("tdma", "slot_size").compare("") != 0) {
            tdma_current_node = 0; // the first slot is assigned to node 0
            tdma_slot_size = stoi(cfg.get_value("tdma", "slot_size"));
            // the slot is initialized with a duration of slot_size, which in fixed tdma is always the same
            tdma_current_node_slot_size = tdma_slot_size;
        }
        if (cfg.get_value("fuzzy_token", "thr_pure_token").compare("") != 0) {
            thr_pure_token = stof(cfg.get_value("fuzzy_token", "thr_pure_token"));
        }
        if (cfg.get_value("fuzzy_token", "thr_pure_brs").compare("") != 0) {
            thr_pure_brs = stof(cfg.get_value("fuzzy_token", "thr_pure_brs"));
        }

    	if (cfg.get_value("input", "mac_protocol").compare("csma_non_p") == 0) {
    		chosen_mac = Mac_protocols::csma_non_p;
    	} else if (cfg.get_value("input", "mac_protocol").compare("brs_non_p") == 0) {
    		chosen_mac = Mac_protocols::brs_non_p;
        } else if (cfg.get_value("input", "mac_protocol").compare("fuzzy_token") == 0) {
    		chosen_mac = Mac_protocols::fuzzy_token;
            if (cfg.get_value("fuzzy_token", "thr_pure_token").compare("") == 0) {
                std::cout << "ERROR: thr_pure_token parameter missing in fuzzy_token section" << std::endl;
                abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
            }
            if (cfg.get_value("fuzzy_token", "thr_pure_brs").compare("") == 0) {
                std::cout << "ERROR: thr_pure_brs parameter missing in fuzzy_token section" << std::endl;
                abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
            }
        } else if (cfg.get_value("input", "mac_protocol").compare("token") == 0) {
    		chosen_mac = Mac_protocols::token;
    	} else if (cfg.get_value("input", "mac_protocol").compare("tdma_fixed") == 0) {
    		chosen_mac = Mac_protocols::tdma_fixed;
            if (cfg.get_value("tdma", "slot_size").compare("") == 0) {
                std::cout << "ERROR: slot_size parameter missing in tdma_fixed section" << std::endl;
                abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
            }
    	} else if (cfg.get_value("input", "mac_protocol").compare("tdma_weighted") == 0) {
    		chosen_mac = Mac_protocols::tdma_weighted;
            if (cfg.get_value("tdma", "slot_size").compare("") == 0) {
                std::cout << "ERROR: slot_size parameter missing in tdma section" << std::endl;
                abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
            }

    	} else {
    		std::cout << "ERROR: No MAC protocol type was found" << std::endl;
            abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
    	}
    	max_buffer_size = stoi(cfg.get_value("input", "max_buffer_size"));
    	tx_time = stoi(cfg.get_value("input", "tx_time"));
	} else {
		std::cout << "ERROR: Input parameters not found in main folder" << std::endl;
        abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
	}
    for (int i=0; i<=nchannels; i++){
    	channel_busy[i] = false;
    }
    medium_busy = false;
    txing_token = false; // we start the simulation always in the second period (pure BRS)
    total_ncycles = 1; // the minimum execution time is 1 cycle, in order to check everything
    total_injected_packets_chip = 0;
    total_served_packets_chip = 0;
    throughput_base_cycles = 0; // count number of cycles used as a total to count the throughput
    throughput_tx_cycles = 0; // count number of cycles with successful transmissions

    // Fuzzy-token specifics


    // Only for statistics and testing purposes

}

void Global_params::set_save_trace_path(std::string save_trace_path) {
    this->save_trace_path = save_trace_path;
}

std::string Global_params::get_save_trace_path() {
    return save_trace_path;
}

void Global_params::set_load_trace_path(std::string load_trace_path) {
    this->load_trace_path = load_trace_path;
}

void Global_params::set_save_results_path(std::string save_results_path) {
    this->save_results_path = save_results_path;
}

std::string Global_params::get_save_results_path() {
    return save_results_path;
}

std::string Global_params::get_load_trace_path() {
    return load_trace_path;
}

bool Global_params::is_medium_busy() {
    return medium_busy;
}
bool Global_params::is_channel_busy(int id){
    return channel_busy(id);
}
void Global_params::set_medium_busy() {
    medium_busy = true;
}

void Global_params::set_medium_idle() {
    medium_busy = false;
}
void Global_params::set_channel_busy(int id) {
    channel_busy[id] = true;
}

void Global_params::set_channel_idle(int id) {
    medium_busy = false;
}

// Get size of ids_concurrent_tx_nodes (number of nodes simultaneously transmitting)
int Global_params::get_ids_concurrent_tx_nodes_size() {
    return ids_concurrent_tx_nodes.size();
}
int Global_params::get_channel_concurrent_tx_nodes_size() {
    return channel_concurrent_tx_nodes.size();
}
// Push new node_id into ids_concurrent_tx_nodes (new node starts transmitting)
void Global_params::push_ids_concurrent_tx_nodes(int nid) {
    ids_concurrent_tx_nodes.push_back(nid);
}
// Push new node_id and channel_id into ids_and_channels_concurrent_tx_nodes (new node starts transmitting)
void Global_params::push_ids_and_channels_concurrent_tx_nodes(vector ncid) {
	ids_and_channels_concurrent_tx_nodes.push_pack(ncid);
}
void Global_params::push_channel_concurrent_tx_nodes(int cid) {
    channel_concurrent_tx_nodes.push_back(nid);
}
// Delete all elements in ids_concurrent_tx_nodes (no node will be transmitting)
void Global_params::flush_ids_concurrent_tx_nodes() {
    ids_concurrent_tx_nodes.erase(ids_concurrent_tx_nodes.begin(), ids_concurrent_tx_nodes.end());
}
// Delete all elements in ids_and_channels_concurrent_tx_nodes (no node will be transmitting)
void Global_params::flush_ids_and_channels_concurrent_tx_nodes() {
	ids_and_channels_concurrent_tx_nodes.erase(ids_and_channels_concurrent_tx_nodes.begin(), ids_and_channels_concurrent_tx_nodes.end());
}
void Global_params::flush_channel_concurrent_tx_nodes() {
    channel_concurrent_tx_nodes.erase(channel_concurrent_tx_nodes.begin(), channel_concurrent_tx_nodes.end());
}
// Delete all nodes that finished transmitting for the given channel
void delete_ids_concurrent_tx_nodes(int cid, vector *curr_node){
	if (*curr_node[1] == cid){
		// delete
		ids_concurrent_tx_nodes.erase(*curr_node)
	}
}
// Provides begin iterator for ids_concurrent_tx_nodes
std::vector<int>::const_iterator Global_params::ids_concurrent_tx_nodes_begin() {
    return ids_concurrent_tx_nodes.begin();
}

// Provides end iterator for ids_concurrent_tx_nodes
std::vector<int>::const_iterator Global_params::ids_concurrent_tx_nodes_end() {
    return ids_concurrent_tx_nodes.end();
}

int Global_params::get_unique_ids_concurrent_tx_nodes() {
    if (ids_concurrent_tx_nodes.size() == 1) {
        return ids_concurrent_tx_nodes.front();
    }
    else {
        std::cout << "ERROR: none or multiple concurrent tx nodes still in vector, but you are asking for only one" << std::endl;
        abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
    }
}

int Global_params::get_ncores() {
    return ncores;
}

void Global_params::set_ncores(int ncores) {
    this->ncores = ncores;
}

int Global_params::get_nchannels() {
    return nchannels;
}

void Global_params::set_nchannels(int nchannels) {
    this->nchannels = nchannels;
}

void Global_params::set_npackets(int npackets) {
    this->npackets = npackets;
}

void Global_params::set_tx_time(int tx_time) {
    this->tx_time = tx_time;
}

int Global_params::get_npackets() {
    return npackets;
}

double Global_params::get_inj_rate() {
    return inj_rate;
}

void Global_params::set_inj_rate(double inj_rate) {
    this->inj_rate = inj_rate;
}

float Global_params::get_sigma() {
    return sigma;
}

void Global_params::set_sigma(float sigma) {
    this->sigma = sigma;
}

double Global_params::get_H() {
    return H;
}

void Global_params::set_H(float H) {
    this->H = H;
}

int Global_params::get_max_buffer_size() {
    return max_buffer_size;
}

Inj_distribs Global_params::get_chosen_distrib() {
    return chosen_distrib;
}

void Global_params::set_chosen_distrib(Inj_distribs chosen_distrib) {
    this->chosen_distrib = chosen_distrib;
}

Mac_protocols Global_params::get_chosen_mac() {
    return chosen_mac;
}

void Global_params::set_chosen_mac(Mac_protocols chosen_mac) {
    this->chosen_mac = chosen_mac;
}

int Global_params::get_tx_time() {
    return tx_time;
}

int Global_params::get_total_injected_packets_chip() {
    return total_injected_packets_chip;
}

int Global_params::get_total_served_packets_chip() {
    return total_served_packets_chip;
}

// we should only call this function at the end, since the total number of cycles won't be final before that
int Global_params::get_stationary_served_packets_chip() {
    long int stationary_served_packets_chip = 0;

    for (auto& entry: cycles_served_packets_chip) {
        // we discard the statistics of the warm-up and cool-down cycles
        if (entry >= floor(0.1*total_ncycles) && entry < ceil(0.8*total_ncycles)) {
            stationary_served_packets_chip++;
        }
    }

    return stationary_served_packets_chip;
}

int Global_params::get_cycle_injection_stopped() {
    return cycle_injection_stopped;
}

int Global_params::get_total_ncycles() {
    return total_ncycles;
}

void Global_params::increase_total_injected_packets_chip() {
    total_injected_packets_chip++;
}

void Global_params::increase_total_served_packets_chip() {
    total_served_packets_chip++;
    cycles_served_packets_chip.push_back(total_ncycles); // we save the cycle number at which each packet is served
}

void Global_params::set_cycle_injection_stopped(int cycle_injection_stopped) {
    this->cycle_injection_stopped = cycle_injection_stopped;
}

void Global_params::increase_total_ncycles() {
    total_ncycles++;
}

void Global_params::increase_throughput_base_cycles() {
    throughput_base_cycles++;
}

void Global_params::increase_throughput_tx_cycles() {
    throughput_tx_cycles++;
}

int Global_params::get_throughput_base_cycles() {
    return throughput_base_cycles;
}
int Global_params::get_throughput_tx_cycles() {
    return throughput_tx_cycles;
}

// check value of debugging
bool Global_params::is_debugging_on() {
    return debugging;
}

// sets debugging=true
void Global_params::set_debugging_on() {
    debugging = true;
}

// check value of save_trace
bool Global_params::is_save_trace_on() {
    return save_trace;
}

// sets save_trace=true
void Global_params::set_save_trace_on() {
    save_trace = true;
}


// TDMA-specific section ==========================================================
int Global_params::get_tdma_current_node() {
    return tdma_current_node;
}

void Global_params::update_tdma_current_node() {
    if (tdma_current_node < (ncores - 1)) {
        tdma_current_node++;
    } else {
        tdma_current_node = 0;
    }
}

void Global_params::set_tdma_slot_size(int tdma_slot_size) {
    this->tdma_slot_size = tdma_slot_size;
}

int Global_params::get_tdma_slot_size() {
    return tdma_slot_size;
}

int Global_params::get_tdma_current_node_slot_size() {
    return tdma_current_node_slot_size;
}

void Global_params::set_tdma_current_node_slot_size(int new_size) {
    tdma_current_node_slot_size = new_size;
}

void Global_params::decrease_tdma_current_node_slot_size() {
    tdma_current_node_slot_size--;
}
// END of TDMA-specific section ===================================================

// TokenRing and FuzzyToken specific section ===================================================
float Global_params::get_thr_pure_brs() {
    return thr_pure_brs;
}

float Global_params::get_thr_pure_token() {
    return thr_pure_token;
}

int Global_params::get_token_current_node() {
    return token_current_node;
}
void Global_params::update_token_current_node() {
    if (token_current_node < (ncores - 1)) {
        token_current_node++;
    } else {
        token_current_node = 0;
    }
}
void Global_params::set_token_current_node() { // Initialize the position of the first token
    token_current_node = 0;
}
bool Global_params::is_txing_token_on() {
    return txing_token;
}
void Global_params::set_txing_token_on() {
    txing_token = true;
}
void Global_params::set_txing_token_off() {
    txing_token = false;
}
// END of TokenRing and FuzzyToken specific section ===================================================

void Global_params::increase_counter_collisions() {
    // every time there's a collision we call this function
    // and we register the cycle of the collision
    cycles_collisions_chip.push_back(total_ncycles);
}

// we should only count this method at the end of the execution, so that the number of total cycles is final
long int Global_params::get_stationary_collisions_chip() {
    long int stationary_collisions = 0;

    for (auto& entry: cycles_collisions_chip) {
        // we discard the statistics of the warm-up and cool-down cycles
        if (entry >= floor(0.1*total_ncycles) && entry < ceil(0.8*total_ncycles)) {
            stationary_collisions++;
        }
    }

    return stationary_collisions;
}

// FuzzyToken-specific section ===================================================
int Global_params::get_fuzzy_size() {
    return fuzzy_size;
}
void Global_params::set_fuzzy_size() { // Initialize the fuzzy size to the number of cores
    fuzzy_size = ncores;
}
void Global_params::increase_fuzzy_size() {
    fuzzy_size++;
    // fuzzy_size += 2;
    // fuzzy_size *= 2;

    if (fuzzy_size >= ncores) {
        fuzzy_size = ncores;
    }

    if (Global_params::Instance()->is_debugging_on()) {
        std::cout << "fuzzy size INCREASED to " << fuzzy_size << std::endl;
    }

    calculate_tx_weights(); // we recalculate the tx weights according to a certain distribution
    rotate_tx_weights(Global_params::Instance()->get_token_current_node()); // we shift the vector to center it at the position of the token holder
}
void Global_params::decrease_fuzzy_size() {
    // fuzzy_size--;
    //fuzzy_size -= 2;
    // fuzzy_size = int(ceil(fuzzy_size/2.0));
    fuzzy_size = 1;

    if (fuzzy_size < 1) {
        fuzzy_size = 1;
    }

    if (Global_params::Instance()->is_debugging_on()) {
        std::cout << "fuzzy size DECREASED to " << fuzzy_size << std::endl;
    }

    calculate_tx_weights(); // we recalculate the tx weights according to a certain distribution
    rotate_tx_weights(Global_params::Instance()->get_token_current_node()); // we shift the vector to center it at the position of the token holder
}

// shifts all positions of tx_weights a "shift" amount to the right
void Global_params::rotate_tx_weights(int shift) {
    // we check if we're overflowing the shift value
    if (shift >= ncores) {
        shift = shift % ncores;
    }

    // because this algorithm shifts to the left by default, by using algorithm_shift we'll be effectively shifting to the right
    int algorithm_shift = ncores - shift;

    int n = tx_weights.size();

    // Create an auxiliary array of twice size.
    std::vector<float> temp_tx_weights(tx_weights.size()*2, 0);

    // Copy tx_weights[] to temp_tx_weights[] two times
    for (int i = 0; i < n; i++) {
        temp_tx_weights[i] = temp_tx_weights[n + i] = tx_weights[i];
    }

    // save from algorithm_shift-th index to (n+i)th index
    int i_original = 0;
    for (int i = algorithm_shift; i < n + algorithm_shift; i++) {
        tx_weights[i_original] = temp_tx_weights[i];
        i_original++;
    }

    if (Global_params::Instance()->is_debugging_on()) {
        std::cout << "Rotated tx_weights by " << shift << std::endl;
        for (int i=0; i<ncores; ++i) {
            std::cout << "Core: " << i << " tx p=" << tx_weights[i] << std::endl;
        }
    }
}

// ensures that mapping vector is equal to ncores and 1-to-1 mapping in it (no randomization)
void Global_params::reset_mapping_node_ids() {
    // we ensure that mapping_node_ids vector is as big as the number of cores
    while (mapping_node_ids.size() < ncores) {
        mapping_node_ids.push_back(0);
    }

    // we create a direct node_id mapping without randomization
    int node_id=0;
    for (auto &entry : mapping_node_ids) {
        entry = node_id;
        node_id++;
    }
}

// randomizes mapping of node IDs, which we will use to effectively randomize the probability of transmission
void Global_params::randomize_mapping_node_ids() {
    std::random_shuffle ( mapping_node_ids.begin(), mapping_node_ids.end() );
    if (Global_params::Instance()->is_debugging_on()) {
        std::cout << "SHUFFLED mapping_node_ids" << std::endl;
    }
}

void Global_params::calculate_tx_weights() {
    int distribution = 1; // all inside fuzzy area equal to 1
    // int distribution = 2; // all inside fuzzy area equal to 1/fuzzy_size
    // int distribution = 3; // all IN CHIP (fuzzy area and outside) gaussian

    switch (distribution) {
        case 1: calculate_fuzzy_area_all_one_distribution();
            break;
        case 2: calculate_fuzzy_area_all_one_div_fuzzysize_distribution();
            break;
        case 3: calculate_normal_distribution();
            break;
        // default:
    }
}

void Global_params::calculate_fuzzy_area_all_one_div_fuzzysize_distribution() {
    // we ensure that tx_weights vector is as big as the number of cores
    while (tx_weights.size() < ncores) {
        tx_weights.push_back(0);
    }

    int node_id = 0;
    for (float& value: tx_weights) {
        // we set a range from 0 to fuzzy_size-1 IDs with all values to 1
        if (value_in_range(node_id, 0, fuzzy_size-1)) {
            value = 1.0/fuzzy_size;
        }
        // and the rest of IDs outside this range have value 0
        else {
            value = 0;
        }

        node_id++;
    }

    if (Global_params::Instance()->is_debugging_on()) {
        std::cout << "Updated tx_weights to all ones (fuzzy area)" << std::endl;
        for (int i=0; i<ncores; ++i) {
            std::cout << "Core: " << i << " tx p=" << tx_weights[i] << std::endl;
        }
    }
}

// ensures that tx_weights is equal to ncores and puts probability of all ones to nodes inside fuzzy area
void Global_params::calculate_fuzzy_area_all_one_distribution() {
    // we ensure that tx_weights vector is as big as the number of cores
    while (tx_weights.size() < ncores) {
        tx_weights.push_back(0);
    }

    int node_id = 0;
    for (float& value: tx_weights) {
        // we set a range from 0 to fuzzy_size-1 IDs with all values to 1
        if (value_in_range(node_id, 0, fuzzy_size-1)) {
            value = 1;
        }
        // and the rest of IDs outside this range have value 0
        else {
            value = 0;
        }

        node_id++;
    }

    if (Global_params::Instance()->is_debugging_on()) {
        std::cout << "Updated tx_weights to all ones (fuzzy area)" << std::endl;
        for (int i=0; i<ncores; ++i) {
            std::cout << "Core: " << i << " tx p=" << tx_weights[i] << std::endl;
        }
    }
}

// ensures that tx_weights is equal to ncores and saves gaussian (normal) distribution in it
void Global_params::calculate_normal_distribution() {
    // we ensure that tx_weights vector is as big as the number of cores
    while (tx_weights.size() < ncores) {
        tx_weights.push_back(0);
    }

    // the bigger the fuzzy size, the flatter the curve, the more probability all nodes will have
    const float sigma = 0.01*fuzzy_size;
    // const float sigma = 0.5*sqrt(fuzzy_size);

    const int mean = 1;

    float delta = 2.0/(ncores-1);
    float x = 0;
    float max_pdf_value = 0;
    for (int id=0; id < ncores; id++) {
        float y = normal_pdf(x, mean, sigma);
        tx_weights[id] = y;
        if (y > max_pdf_value) {
            max_pdf_value = y;
        }
        x += delta;
    }

    for (float& value: tx_weights) {
        value /= max_pdf_value;
    }

    if (Global_params::Instance()->is_debugging_on()) {
        std::cout << "Updated tx_weights with gaussian distribution" << std::endl;
        for (int i=0; i<ncores; ++i) {
            std::cout << "Core: " << i << " tx p=" << tx_weights[i] << std::endl;
        }
    }
}

void Global_params::add_log_code(int code) {
    log_curr_token.push_back(code);
}
int Global_params::get_max_code() {
    int temp = 0;
    for (auto& value: log_curr_token) {
        if (value > temp) {
            temp = value;
        }
    }
    return temp;
}
void Global_params::reset_log() {
    log_curr_token.clear();
}
// END of FuzzyToken-specific section ===================================================
