#include "headers.hpp"
#include "packet.hpp"
#include "node.hpp"
#include "node_poisson.hpp"
#include "node_burst.hpp"
#include "node_trace.hpp"
#include "utilities.hpp"
#include "mac_protocols.hpp"
#include "global_params.hpp"
#include "channel.hpp"

/*-------------------------------------------------------

Main file of RSim. From here we read the input parameters either from INI
or from shell, we initialize the chip vector, we run the loop that iterates
over all cycles, checking if new packets have to be injected for every core,
and for those nodes with non-empty buffers, to be send as an argument to the
corresponding MAC protocol function call.

To compile:
	cd to main folder of simulator
	make clean
	make

To run:
	./Project [H=value] [ncores=value] [mac_protocol=value] [inj_rate=value]
			[sigma=value] [npackets=value] [tx_time=value] [debugging=true/false]

Results are saved in a csv file in rsim/results/, separated by commas, and the debugging output is saved
in rsim/output/log.txt

A running script can be found in rsim/run_sims.sh

---------------------------------------------------------*/

int main(int argc, char* argv[]) {
	// if the user introduces parameters from shell, we overwrite those from INI file
//std::cout << "hola"<< std::endl;
	for (int i=1; i < argc; i++) { // for each of the input arguments
		std::vector<std::string> in_parameter = split(argv[i], '=');
		std::string parameter_name = in_parameter.at(0);
		std::string parameter_value = in_parameter.at(1);

		if (parameter_name.compare("H") == 0) {
			//std::cout << "hola"<< std::endl;
			Global_params::Instance()->set_H(stof(parameter_value));
		}
		else if (parameter_name.compare("inj_rate") == 0) {
			//std::cout << "hola"<< std::endl;
			Global_params::Instance()->set_inj_rate(stof(parameter_value));
		}
		else if (parameter_name.compare("ncores") == 0) {
			Global_params::Instance()->set_ncores(stoi(parameter_value));
		}
		else if (parameter_name.compare("npackets") == 0) {
			Global_params::Instance()->set_npackets(stoi(parameter_value));
		}
		else if (parameter_name.compare("nchannels") == 0) {
			Global_params::Instance()->set_nchannels(stoi(parameter_value));
		}
		else if (parameter_name.compare("assig") == 0) {
			Global_params::Instance()->set_chosen_assig(stoi(parameter_value));
		}
		else if (parameter_name.compare("tx_time") == 0) {
			Global_params::Instance()->set_tx_time(stof(parameter_value));
		}
		else if (parameter_name.compare("slot_size") == 0) {
			Global_params::Instance()->set_tdma_slot_size(stoi(parameter_value));
		}
		else if (parameter_name.compare("sigma") == 0) {
			Global_params::Instance()->set_sigma(stof(parameter_value));
		}
		else if (parameter_name.compare("debugging") == 0) {
			if (parameter_value.compare("true") == 0) {
				Global_params::Instance()->set_debugging_on();
			}
		}
		else if (parameter_name.compare("inj_distribution") == 0) {
			if (parameter_value.compare("poisson") == 0) {
	    	       Global_params::Instance()->set_chosen_distrib(Inj_distribs::poisson);
	    	}
	        else if (parameter_value.compare("burst") == 0) {
				Global_params::Instance()->set_chosen_distrib(Inj_distribs::burst);
	    	}
	        else if (parameter_value.compare("trace") == 0) {
				Global_params::Instance()->set_chosen_distrib(Inj_distribs::trace);
	    	}
		}
		else if (parameter_name.compare("save_trace") == 0) {
			if (parameter_value.compare("true") == 0) {
				Global_params::Instance()->set_save_trace_on();
			}
		}
		else if (parameter_name.compare("save_trace_path") == 0) {
			Global_params::Instance()->set_save_trace_path(parameter_value);
		}
		else if (parameter_name.compare("save_results_path") == 0) {
			Global_params::Instance()->set_save_results_path(parameter_value);
		}
		else if (parameter_name.compare("load_trace_path") == 0) {
			Global_params::Instance()->set_load_trace_path(parameter_value);
		}
		else if (parameter_name.compare("mac_protocol") == 0) {
			if (parameter_value.compare("csma_non_p") == 0) {
	    		Global_params::Instance()->set_chosen_mac(Mac_protocols::csma_non_p);
	    	}
			else if (parameter_value.compare("brs_non_p") == 0) {
	    		Global_params::Instance()->set_chosen_mac(Mac_protocols::brs_non_p);
	    	}
			else if (parameter_value.compare("tdma_fixed") == 0) {
	    		Global_params::Instance()->set_chosen_mac(Mac_protocols::tdma_fixed);
				if (Global_params::Instance()->get_tdma_slot_size() == 0) {
					std::cout << "ERROR: slot_size parameter missing in tdma section" << std::endl;
					abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
				}
	    	}
			else if (parameter_value.compare("tdma_weighted") == 0) {
	    		Global_params::Instance()->set_chosen_mac(Mac_protocols::tdma_weighted);
				if (Global_params::Instance()->get_tdma_slot_size() == 0) {
					std::cout << "ERROR: slot_size parameter missing in tdma section" << std::endl;
					abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
				}
	    	}
			else if (parameter_value.compare("fuzzy_token") == 0) {
	    		Global_params::Instance()->set_chosen_mac(Mac_protocols::fuzzy_token);
				Global_params::Instance()->set_fuzzy_size(); // we set the fuzzy size equal to number of cores
				Global_params::Instance()->set_token_current_node(); // we set the current node of the token to 0
	    	}
			else if (parameter_value.compare("token") == 0) {
	    		Global_params::Instance()->set_chosen_mac(Mac_protocols::token);
				Global_params::Instance()->set_token_current_node(); // we set the current node of the token to 0
	    	}
		}
	}
	//std::cout << "hola"<< std::endl;

	//Global_params::Instance()->set_nchannels(4);
	// TODO: Ensure that all values were read either from parameter file or std input

	// Print all the parameters, to ensure we read them properly from the INI file or the standard input
	print_input_parameters();// where????

	// Initialize the random generator used by rand()
	srand (time(NULL));

	// Declare a vector with all the nodes
	std::vector<Node*> chip;
	std::vector<Channel*> chan;

	// Set a vector of injection rates, according to a normal distribution with mean=1 and std_dev=sigma
	// TODO: MAKE SURE THIS IS DONE PROPERLY, I'M NOT SURE I DID IT CORRECTLY ANYMORE
	std::vector<float> hotspotness_weights;
	std::vector<float> hotspotness_weights_normal;
	float sum_pdf_values = 0;
	float delta = 2.0 / (Global_params::Instance()->get_ncores() - 1);
	float x = 0;
	for (int id=0; id < Global_params::Instance()->get_ncores(); id++) {
		float y = normal_pdf(x, 1, Global_params::Instance()->get_sigma());
		hotspotness_weights.push_back(y);
		hotspotness_weights_normal.push_back(y);

		// hotspotness_weights.push_back(1.0); //WARNING: I DISABLED THE HOTSPOTNESS BY MAKING ALL WEIGHTS 1, CHANGE THIS WHEN YOU NEED THE RIGHT WEIGHTS AGAIN
		sum_pdf_values += y;
		x += delta;


	}
	//std::cout<<"total"<< sum_pdf_values<<std::endl;
	float total=0;


	// TODO: WHAT ARE WE DOING WITH THIS VALUE? IT NEVER GETS USED? WAS IT USED TO CHECK SOMETHING?
	for (float& value: hotspotness_weights) {
		value *= Global_params::Instance()->get_ncores() / sum_pdf_values;
	}

	for (float& value: hotspotness_weights_normal) {
		value = value*(1 / sum_pdf_values);
	}


float number_channels = Global_params::Instance()->get_nchannels();


	// we shuffle the hotspotness_weights so that hot nodes are not next to each other
	std::random_shuffle ( hotspotness_weights.begin(), hotspotness_weights.end() );
	std::random_shuffle ( hotspotness_weights_normal.begin(), hotspotness_weights_normal.end() );


	// we print (if debugging mode is on) the hotspotness values to make sure they are set to what we expect
	if (Global_params::Instance()->is_debugging_on()) {
		for (int id=0; id < Global_params::Instance()->get_ncores(); id++) {
			std::cout << "Core: " << id << " hotspotness weight: " << hotspotness_weights.at(id) << std::endl;
		}
	}

	// TODO: This is to set the right slot size of the first node, since the slot size is set at constructor, but we don't have the weighted values there yet, so we update it here
	if (Global_params::Instance()->get_chosen_mac() == Mac_protocols::tdma_weighted) {
		Global_params::Instance()->set_tdma_current_node_slot_size(Global_params::Instance()->get_tdma_slot_size() * ceil(hotspotness_weights.at(Global_params::Instance()->get_tdma_current_node())));
	}

	// if we choose fuzzy-token, we initialize vector of certain probabilities
	if (Global_params::Instance()->get_chosen_mac() == Mac_protocols::fuzzy_token) {
		Global_params::Instance()->calculate_tx_weights(); // this loads all probbility values according to chosen distribution
		Global_params::Instance()->rotate_tx_weights( Global_params::Instance()->get_token_current_node() ); // this centers the gaussian curve to node 0, so that token holder starts with max probability
		if (Global_params::Instance()->is_debugging_on()) {
			std::cout << "-----------------------------------" << std::endl;
			for (int i=0; i<Global_params::Instance()->get_ncores(); ++i) {
		        std::cout << "Core: " << i << " tx p=" << Global_params::Instance()->tx_weights[i] << std::endl;
		    }
		}
		Global_params::Instance()->reset_mapping_node_ids(); // resets mapping to 1-to-1 without randomization
	}

	// Initialize each of the ncores nodes with its ID and its Inj_distribs type
	for (int id=0; id < Global_params::Instance()->get_ncores(); id++) {
		switch(Global_params::Instance()->get_chosen_distrib()) {
			// TODO: I MADE THE HOTSPOTNESS WEIGHTS TO BE ALL 1, SO NOW ALL NODES HAVE SAME INJECTION RATE AVERAGE FOR POISSON.
			// 		IF I NEED THE WEIGHTS, GO BACK TO THE PART OF THE CODE WHERE I HARDCODED THEM TO BE ALL 1 AND CHANGE FOR PROPER WEIGHT
			case Inj_distribs::poisson	: chip.push_back(new Node_poisson(id, Global_params::Instance()->get_inj_rate() * hotspotness_weights.at(id))); break;
			case Inj_distribs::burst	: chip.push_back(new Node_burst(id, Global_params::Instance()->get_inj_rate() * hotspotness_weights.at(id))); break;
			case Inj_distribs::trace	: chip.push_back(new Node_trace(id)); break;
		}
	}

	// Set up flag to be used during iteration through all the cycles
	bool flag_stop_injection = false;

	// if the chosen injection distribution is actually a trace, we read it from the file and save it into a vector
	if (Global_params::Instance()->get_chosen_distrib() == Inj_distribs::trace) {
		std::cout << "READING TRACE FILE FROM " << Global_params::Instance()->get_load_trace_path() << std::endl;
		int npackets = 0; // we initialize the total counter of packets injected
		std::ifstream injection_trace_file(Global_params::Instance()->get_load_trace_path());
		if (injection_trace_file.is_open()) {
			int last_cycle_injection = 0;
		    std::string line;
			int cycle_number;
			int node_id;
			// we read each line in the trace file
		    while (getline(injection_trace_file, line)) {
				std::istringstream iss(line);
				// each line of the trace should have a cycle number and the node ID separated by a space
			    if (!(iss >> cycle_number >> node_id)) {
					std::cout << "ERROR: injection_trace_file contains a non-valid line" << std::endl;
					abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
				}
				if (node_id >= Global_params::Instance()->get_ncores()) {
					// node IDs in the trace should go from 0 to ncores-1 (maybe we can make it such that ncores is read from the trace as well)
					std::cout << "ERROR: injection_trace_file contains a node ID bigger than the total number of nodes" << std::endl;
					abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
				}

				// We only allow cycle numbers below the limit for a simply 4 byte INT. If we have reached the limit value for a simple 4 byte int, we cut the injection
				// WARNING: do we want to cut the trace when it reaches cycle INT_MAX? Or we want to allow to proceed? If so, we need to convert all INTs to LONG INTs
				if (cycle_number < INT_MAX) {
					// we add an entry with the node ID to the corresponding cycle number entry
					// WARNING: we are assuming that in the trace file the first cycle is 0 instead of 1. In the simulator the first cycle is 1,
					//			however Sergi had prepared the traces so that they start in cycle 0, so I added an offset of 1 to convert the trace file cycle
					//			to the actual simulator cycle
					Global_params::Instance()->injections_lookup_table[(int)cycle_number + 1].push_back(node_id);
					npackets++;
					last_cycle_injection = cycle_number;
				}
		    }
		    injection_trace_file.close();
			std::cout << "FINISHED READING TRACE FILE. Total packets to be injected: " << npackets << ". Last injection at cycle " << last_cycle_injection << std::endl;
			if (last_cycle_injection < cycle_number) {
				std::cout << "READING OF TRACE FILE WAS CUT (CYCLE NUMBER LIMIT REACHED). The last packet in the trace was actually at cycle " << cycle_number << std::endl;
			}
		}
		else {
			std::cout << "ERROR: injection_trace_file couldn't be opened" << std::endl;
			abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
		}

		// once we have finished reading all the injected packets from the trace, we save the total number of packets injected
		Global_params::Instance()->set_npackets(npackets);
	}

		for(int cid=0;cid<Global_params::Instance()->get_nchannels();cid++){
				chan.push_back(new Channel(cid));
		}

		int assig = Global_params::Instance()->get_chosen_assig();
		//std::cout<<"Assig"<<assig<<std::endl;
		float prob_chan=1/number_channels;
		float prob_total=prob_chan;
		float prob_assig;
		int k_chann;
		//std::cout<<prob_total<<std::endl;

	std::vector<float> distribution;
  float probm=0;
	for(int j = 0; j<Global_params::Instance()->get_ncores();j++){
		probm = probm+hotspotness_weights_normal[j];
		distribution.push_back(probm);
		std::cout<<" ["<<probm<<"] ";
	}


		if(assig==3){
			for(int k=0;k<number_channels;k++){
				float prob=0;
				for(int j = 0; j<Global_params::Instance()->get_ncores();j++){
					if(prob<=prob_total){
						//std::cout<<"bebecita"<<std::endl;
						prob = prob+hotspotness_weights_normal[j];
						//std::cout<<"Prob del node "<< j << " es "<< hotspotness_weights_normal[j]<<std::endl;

						if(chip[j]->get_channel_array().size()==0){
							chip[j]->set_channel_array(k);

						prob_assig=prob_total;
						k_chann=k;

						while(prob>prob_assig){
							 k_chann++;
								chip[j]->set_channel_array(k_chann);
								prob_assig=prob_assig+prob_chan;

						}
					}
					}

					if(k==3){
						//std::cout<<prob<<std::endl;
					}
				}
				prob_total=prob_total+prob_chan;
			}

		}
		float prob = 0;
		for(int j = 0; j<Global_params::Instance()->get_ncores();j++){
			prob+=hotspotness_weights_normal[j];
			std::cout<<"Node: "<< j << "  Probabilitat:"<<hotspotness_weights_normal[j]<<"   Suma Prob:  "<< prob;
			int size = chip[j]->get_channel_array().size();
				std::cout<<"   Channel assignat: ";
			for(int l = 0; l<size;l++){
				std::cout<<"  "<<chip[j]->get_channel_node(l);
			}
			std::cout<<std::endl;
		}

	// Every iteration of this do-while represent a cycle of the execution, analyzing what happens at each of them
	do {
		// regardless if we're in debugging mode or not, every 1,000,000 cycles we print a control message (so that we can identify progress)
		if ((Global_params::Instance()->get_total_ncycles() % 1000000) == 0) {
			std::chrono::time_point<std::chrono::system_clock> now;
		    now = std::chrono::system_clock::now();
		    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
			std::cout << "Cycle: " << Global_params::Instance()->get_total_ncycles() << " reached at " << std::ctime(&now_time);
		}

		if (Global_params::Instance()->is_debugging_on()) {
			std::cout << "----------------- Cycle " << Global_params::Instance()->get_total_ncycles() << " -------------------" << std::endl;
		}

		// if we still have packets to inject
		if (Global_params::Instance()->get_total_injected_packets_chip() < Global_params::Instance()->get_npackets()) {
			// iterates through all nodes of the chip to see which ones require injection of new packet in current cycle
			for (std::vector<Node*>::iterator curr_node = chip.begin(); curr_node != chip.end(); ++curr_node) {
				(*curr_node)->check_if_injection();
			}
		}

		// here we only enter the first time that we realize we stopped injecting
		if (Global_params::Instance()->get_total_injected_packets_chip() >= Global_params::Instance()->get_npackets() && !flag_stop_injection) {
			Global_params::Instance()->set_cycle_injection_stopped(Global_params::Instance()->get_total_ncycles());
			flag_stop_injection = true;
		}


		std::vector<int> nodes_ready; // at every cycle we initialize an empty vector that will store the IDs of the nodes with non-empty buffers



		// iterates through all nodes of the chip to see which ones have a packet to transmit
		for (std::vector<Node*>::iterator curr_node = chip.begin(); curr_node != chip.end(); ++curr_node) {
			if (!(*curr_node)->in_buffer_empty()) {
				// when we find a node with a non-empty buffer, we store its ID
				//std::cout<< "gre"<<std::endl;

				//std::cout<< "Before channel: "<< before_funtion <<std::endl;
				(*curr_node)->channel_function("brs", "initialisation of channel link to node", number_channels, 1, assig, 0);
				int before_funtion=(*curr_node)->get_channel_id();
				//std::cout<<"Channel initial:" << before_funtion<<std::endl;
				nodes_ready.push_back((*curr_node)->get_id());
				if (Global_params::Instance()->is_debugging_on()) {

					std::cout << "Node " << (*curr_node)->get_id() << " wants to tx (" << (*curr_node)->get_in_buffer_size() << " pending packets)" << std::endl;
				}
			}
		}


		// We call the appropriate MAC protocol to deal with the concurrent packets that are ready
		switch(Global_params::Instance()->get_chosen_mac()) {
			case Mac_protocols::csma_non_p	: protocol_csma_non_p(Global_params::Instance()->get_total_ncycles(), nodes_ready, chip,chan,number_channels); break;
			case Mac_protocols::brs_non_p	: protocol_brs_non_p(Global_params::Instance()->get_total_ncycles(), nodes_ready, chip,chan,number_channels); break;
			case Mac_protocols::tdma_fixed	: protocol_tdma(Global_params::Instance()->get_total_ncycles(), nodes_ready, chip,chan, hotspotness_weights,number_channels); break;
			case Mac_protocols::token		: protocol_token(Global_params::Instance()->get_total_ncycles(), nodes_ready, chip,chan, hotspotness_weights,number_channels); break;
			case Mac_protocols::tdma_weighted	: protocol_tdma(Global_params::Instance()->get_total_ncycles(), nodes_ready, chip,chan, hotspotness_weights,number_channels); break;
			case Mac_protocols::fuzzy_token	: protocol_fuzzy_token(Global_params::Instance()->get_total_ncycles(), nodes_ready, chip,chan, hotspotness_weights,number_channels); break;

		}
//
		// Once we finish analyzing the current cycle we increase the global counter
		Global_params::Instance()->increase_total_ncycles();

		// This is to calculate throughput. Every cycle that somebody transmits successfully, count it, but only for the 70% of the central packets, so that we allow warm-up and cool-down
		if (Global_params::Instance()->get_total_served_packets_chip() >= 0.1*Global_params::Instance()->get_npackets() && Global_params::Instance()->get_total_served_packets_chip() < 0.8*Global_params::Instance()->get_npackets()) {
			Global_params::Instance()->increase_throughput_base_cycles();
		}
	// TODO: I THINK WE SHOULD MODIFY THE STOPPING CONDITION SO THAT WE ONLY GET SIMULATION STATISTICS AFTER WARM-UP AND BEFORE COOL-DOWN OF BUFFERS AND EVERYTHING
	//			OTHERWISE IF WE STOP WHEN ALL PACKETS ARE SERVED INSTEAD OF INJECTED THE LAST PACKETS TO BE SERVED WILL HAVE LOWER CONTENTION
	//			CHECK IF THIS AFFECTS STATISTICS
	} while (Global_params::Instance()->get_total_served_packets_chip() < Global_params::Instance()->get_npackets()); // We keep generating cycles until all packets are served

	// Print results with function from utilities.cpp
	print_statistics_per_node(chip);

	return 0;
}
