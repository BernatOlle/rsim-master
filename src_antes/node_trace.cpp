#include "node_trace.hpp"

Node_trace::Node_trace(int nid) : Node(nid) {

}


void Node_trace::check_if_injection() {
	// first we make sure that there's still packets left to inject
	if (Global_params::Instance()->get_total_injected_packets_chip() < Global_params::Instance()->get_npackets()) {
		std::map<int, std::vector<int>>::iterator cycles_iterator;
		// we check if the current cycle has any nodes that are supposed to inject (we look up the current cycle in the map)
		cycles_iterator = Global_params::Instance()->injections_lookup_table.find(Global_params::Instance()->get_total_ncycles());
		if (cycles_iterator != Global_params::Instance()->injections_lookup_table.end()) { // if we find that current cycle has some nodes to tx
            // TODO: this could be optimized. why does each node have to search through the whole lookup table and copy the second vector? it should be done only once per chip
			std::vector<int> &nodes_in_cycle = cycles_iterator->second; // we get a reference to the vector containing the list of nodes to tx in current cycle
            
			// we traverse the node ID list corresponding to the current cycle, looking for those nodes that match the ID of the current node
			// WARNING: this might lead to more than one packet injection per node per cycle, depending whether the trace allowed it or not
			for (const int& node : nodes_in_cycle) {
				if (node == get_id()) {  // if it matches the current node ID, we try to inject them in the buffer and update statistics
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
		}
	}
}
