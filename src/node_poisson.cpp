#include "node_poisson.hpp"

Node_poisson::Node_poisson(int nid, float node_inj_rate) : Node(nid, node_inj_rate) {
	seed = std::chrono::system_clock::now().time_since_epoch().count();
	generator.seed(seed);
	inj_distribution.param(std::exponential_distribution<double>(node_inj_rate).param());
	update_cnt_next_packet(); // inter-arrival packet times, considering poisson distribution. Constructor sets first value
}

// calculates number of cycles to wait until injecting the new packet
// the distribution of these inter-injection times is obtained from an inverted exponential distribution, with a mean of 1/inj_rate
// notice that because we use ceil(obtained value), the minimum inter-injection time is 1 (not 0), this means that we do not allow
// two packets (or more) to be injected in the same cycle
void Node_poisson::update_cnt_next_packet() {
	cnt_next_packet = ceil(inj_distribution(generator));
}

int Node_poisson::get_cnt_next_packet() {
	return cnt_next_packet;
}

void Node_poisson::decrease_cnt_next_packet() {
	--cnt_next_packet;
}



void Node_poisson::check_if_injection() {
	// first thing we do is reduce the counter for number of cycles until next injection, since we started a new cycle
	decrease_cnt_next_packet();

	// If the injection counter of that node reaches zero, we inject a new packet to its buffer
	if (get_cnt_next_packet() == 0) {
		// But first we make sure that we haven't reached the total number of packets to be transmitted
		if (Global_params::Instance()->get_total_injected_packets_chip() < Global_params::Instance()->get_npackets()) {
			// if the node buffer is not full
			if (get_in_buffer_size() <= Global_params::Instance()->get_max_buffer_size()) {
				push_new_packet( Global_params::Instance()->get_total_ncycles(), get_id() ); // injection of new packet to buffer
			}
			// otherwise (if buffer full), we deal with the loss
			else {
				// this is temporal, we should do more than a printf if a packet is lost
				std::cout << "!!!!!!!!!! A packet has just been lost due to full buffer in node " << get_id() << std::endl;
				abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
			}
			// regardless if we lost the packet (because of full buffer) or not, we keep injecting, because the application must keep on going
			update_cnt_next_packet(); // calculate number of cycles until injecting the next packet (minimum 1, which is the current cycle)
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
	}

	// otherwise we check if for some reason the counter was lower than 0 (error)
	else if (get_cnt_next_packet() < 0) {
		std::cout << "!!!!!!!!!! ERROR in node " << get_id() << ". For some reason the cnt_next_packet was " << get_cnt_next_packet() << " which is lower than 0" << std::endl;
		abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
	}
}
