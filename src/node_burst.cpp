#include "node_burst.hpp"

Node_burst::Node_burst(int nid, float node_inj_rate) : Node(nid, node_inj_rate) {
	a = 3-2*Global_params::Instance()->get_H();
	bon = 1;
	boff = bon*(1.0/node_inj_rate-1);
	seed = std::chrono::system_clock::now().time_since_epoch().count();
	generator.seed(seed);
	distribution.param(std::uniform_real_distribution<double>(0.0,1.0).param());
	update_cnt_on();
	update_cnt_off();
}

void Node_burst::update_cnt_on() {
	cnt_on = ceil(bon/(pow(1-distribution(generator),1.0/a)));
}

void Node_burst::update_cnt_off() {
	cnt_off = ceil(boff/(pow(1-distribution(generator),1.0/a)));
}

int Node_burst::get_cnt_on() {
	return cnt_on;
}

int Node_burst::get_cnt_off() {
	return cnt_off;
}

void Node_burst::decrease_cnt_on() {
	cnt_on--;
}

void Node_burst::decrease_cnt_off() {
	cnt_off--;
}

void Node_burst::check_if_injection() {
	// TODO: CHECK HOW WE DID IT WITH POISSON TO MAKE SURE THAT TWO PACKETS CAN'T BE INJECTED IN SAME CYCLE, BUT CAN BE BACK TO BACK

	// If we are on the ON state of the burst of a node, we inject a new packet to its buffer and decrease the ON time counter
	if (get_cnt_on() > 0) {
		// But first we make sure that we haven't reached the total number of packets to be transmitted
		if (Global_params::Instance()->get_total_injected_packets_chip() < Global_params::Instance()->get_npackets()) {
			// if the node buffer is not full
			if (get_in_buffer_size() <= Global_params::Instance()->get_max_buffer_size()) {
				push_new_packet( Global_params::Instance()->get_total_ncycles(), get_id() ); // injection of new packet to buffer
				total_injected_packets_node++;
				Global_params::Instance()->increase_total_injected_packets_chip();

				// If save_trace=true, every time a node injects a packet it copies the current cycle into it (it allows for duplicates if two or more nodes inject at the same time)
				if (Global_params::Instance()->is_save_trace_on()) {
					std::ofstream cycles_trace_file;
				    cycles_trace_file.open (Global_params::Instance()->get_save_trace_path(), std::ios_base::app);
					// PRINT TRACE FILE FORMAT 1 (prepared for getting statistic in matlab), we only save injection cycle of each packet, without saving node id
				    // cycles_trace_file << Global_params::Instance()->get_total_ncycles() << " ";

					// PRINT TRACE FILE FORMAT 2 (prepared to be read in rsim), each line contains cycle and node id separated by a space
					// WARNING: to keep coherence with traces that Sergi is getting from multi2sim, we make the trace to start at cycle 0
					cycles_trace_file << Global_params::Instance()->get_total_ncycles() - 1 << " " << nid << std::endl;

					cycles_trace_file.close();
				}

			}
			// otherwise (if buffer full), we deal with the loss
			else {
				// this is temporal, we should do more than a printf if a packet is lost
				std::cout << "!!!!!!!!!! A packet has just been lost due to full buffer in node " << get_id() << std::endl;
			}
			// regardless if we lost the packet (because of full buffer) or not, we keep injecting, because the application must keep on going
			decrease_cnt_on();
		}
	}
	// otherwise if we still have some OFF cycles left, we simply decrease the number of OFF cycles until next ON state
	else if (get_cnt_off() > 0) {
		decrease_cnt_off();
		// when we finish the OFF state, we prepare the duration of next both ON and OFF states
		if (get_cnt_off() == 0) {
			update_cnt_on();
			update_cnt_off();
		}
	}
	else {
		std::cout << "ERROR DURING INJECTION: no condition was satisfied" << std::endl;
	}
}
