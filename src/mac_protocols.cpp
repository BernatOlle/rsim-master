#include "mac_protocols.hpp"
#include <memory>
#include <algorithm>
#include "utilities.hpp"

// Specification of CSMA non-persistant
void protocol_csma_non_p(int curr_cycle, const std::vector<int>& nodes_ready, std::vector<bool>& channel_avaible,std::vector<Node*>& chip,std::vector<Channel*>& chan,int number_channels,std::string mac_protocol_string) {
// 	// if more than one node has packets to transmit in the current cycle
// 	if (nodes_ready.size() > 1) {
// 		std::vector<int> nodes_zero_backoff; // at every cycle we initialize an empty vector that will store the IDs of the nodes with backoff at zero
// 		// iterates through all nodes which packets to tx to see which have a backoff of zero
// 		for (std::vector<int>::const_iterator curr_node_id = nodes_ready.begin(); curr_node_id != nodes_ready.end(); ++curr_node_id) {
// 			Node* p_node = chip.at(*curr_node_id);
// 			// We cast the Packet* into a Packet_csma_non_p* so that we can access its own methods
// 			if (Packet_csma_non_p* p_packet = dynamic_cast<Packet_csma_non_p*>(p_node->get_in_buffer_front())) {
// 				if (p_packet->get_cnt_backoff() > 0) {
// 					p_packet->decrease_cnt_backoff();
// 				}
// 				else {
// 					nodes_zero_backoff.push_back(*curr_node_id);
// 				}
// 		    }
// 			// If the cast fails
// 			else {
// 				std::cout << "ERROR: Cast from Packet* to Packet_csma_non_p* failed" << std::endl;
// 			}
// 		}
//
// 		if (nodes_zero_backoff.size() > 0) {
// 			srand (time(NULL));
// 			int chosen_node = rand() % nodes_zero_backoff.size();
// 			if (chosen_node >= nodes_zero_backoff.size()) {
// 				std::cout << "error" << std::endl;
// 			} else {
// 				chip.at(nodes_zero_backoff.at(chosen_node))->pop_packet_buffer(curr_cycle);
// 				nodes_zero_backoff.erase(nodes_zero_backoff.begin()+chosen_node);
// 			}
//
// 			for (std::vector<int>::iterator curr_node_id = nodes_zero_backoff.begin(); curr_node_id != nodes_zero_backoff.end(); ++curr_node_id) {
// 				Node* p_node = chip.at(*curr_node_id);
// 				// We cast the Packet* into a Packet_csma_non_p* so that we can access its own methods
// 				if (Packet_csma_non_p* p_packet = dynamic_cast<Packet_csma_non_p*>(p_node->get_in_buffer_front())) {
// 					if (p_packet->get_backoff_exp() < 10) {
// 						p_packet->increase_backoff_exp();
//
// 					} else {
// 						p_packet->reset_backoff_exp(); // if we reach the top of the binary exponential backoff size, we restart it
// 						std::cout << "EXPONENTIAL BACKOFF SIZE REACHED FOR NODE " << *curr_node_id << std::endl;
// 					}
// 					p_packet->update_cnt_backoff(); // Choose randomly a new backoff time between 1 and 2^backoff_exp
// 				}
// 				// If the cast fails
// 				else {
// 					std::cout << "ERROR: Cast from Packet* to Packet_csma_non_p* failed" << std::endl;
// 				}
// 			}
// 		}
// 	}
// 	// otherwise, if only one node has packets to transmit in current cycle
// 	else if (nodes_ready.size() == 1){
// 		chip.at(nodes_ready.at(0))->pop_packet_buffer(curr_cycle);
// 	}
}
//===============================================================

// Specification of BRS-MAC non-persistent. Returns 0 if nobody transmitted, 1 if collision occurred and 2 if somebody transmitted successfully
int protocol_brs_non_p(int curr_cycle, const std::vector<int>& nodes_ready, std::vector<bool>& channel_avaible,std::vector<Node*>& chip,std::vector<Channel*>& chan,int nchannels, std::string mac_protocol_string) {
	// If the medium is idle
	for (int channel_id = 0; channel_id < nchannels; channel_id++) {
		//int si = nodes_ready.size();
		//std::cout << "nodes_ready= "<< si<<std::endl;
			//for(int h=0;h<nodes_ready.size();h++){
			//std::cout<<nodes_ready[h]<<std::endl;
			//}
	if (!Global_params::Instance()->is_channel_busy(channel_id)) {
		//std::cout << "Inside channel idle = "<<channel_id<<std::endl;
		// For each node with a non-empty buffer, regardless if its 0, 1 or 2+ nodes...
		for (std::vector<int>::const_iterator curr_node_id = nodes_ready.begin(); curr_node_id != nodes_ready.end(); ++curr_node_id) {
			Node* p_node = chip.at(*curr_node_id);
			int node_channel = p_node->get_channel_id();
			//std::cout<< "Node cid = "<<node_channel<<std::endl;
			if (node_channel == channel_id) {
			// We cast the Packet* into a Packet_brs_non_p* so that we can access its own methods
				if (Packet_brs_non_p* p_packet = dynamic_cast<Packet_brs_non_p*>(p_node->get_in_buffer_front())) {
				// If backoff is still not zero, we decrease it
					int backoff = p_packet->get_cnt_backoff();
					//std::cout<<"backoff = "<<backoff<<std::endl;
					if (p_packet->get_cnt_backoff() > 0) {
					p_packet->decrease_cnt_backoff();
					}
				// If backoff is zero, we transmit first cycle/preamble of packet
					else {
						Global_params::Instance()->set_channel_busy(channel_id);
						Channel* p_channel = chan.at(channel_id);

						p_channel->push_ids_concurrent_tx_nodes(*curr_node_id);
						int ids = p_channel->get_ids_concurrent_tx_nodes_size();
						//std::cout<<"Courrent tx size idle = "<<ids<<std::endl;
					// Notice we don't decrease the cycles_left of the packet, since we have to leave one extra cycle after the header to check for collisions
				}
		    }
			// If the cast fails
			else {
				std::cout << "ERROR: AACast from Packet* to Packet_brs_non_p* failed" << std::endl;
				abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
			}
		}
		} // End of for-each
	} // End of if-medium-idle
	// If the medium is busy do the following:
	// -check if vector of transmitting nodes has size more than 1
	// -if it's more than 1 it means a collision has been detected and therefore a NACK being sent, and therefore
	// the colliding nodes will update their cnt_backoff, we will empty the vector of transmitting nodes and set the medium to idle.
	// -if there was only one transmitting node we decrease the cycles_left and check if cycles_left is zero, if it's zero it means we'll be
	// done tx at the end of this cycle. So we empty the vector of transmitting nodes, we set the medium to idle, we take the packet out
	// of the buffer, we increase counters of total served packets per node and per chip and if it isn't zero we don't have to do nothing because we already decreased cycles_left
	else {
		Channel* p_channel = chan.at(channel_id);
		//std::cout<<"Indide channel busy = "<< channel_id<<std::endl;
		int sizeCur= p_channel->get_ids_concurrent_tx_nodes_size();
		//std::cout<<"Courrent tx size busy = "<< sizeCur<<std::endl;
		// If multiple colliding nodes
		if(p_channel->get_ids_concurrent_tx_nodes_size() > 1) {
			// for each ids_concurrent_tx_nodes, update_cnt_backoff, then empty vector of ids_concurrent_tx_nodes, then set medium to idle
			for (std::vector<int>::const_iterator curr_node_id = p_channel->ids_concurrent_tx_nodes_begin(); curr_node_id != p_channel->ids_concurrent_tx_nodes_end(); ++curr_node_id) {
				Node* p_node = chip.at(*curr_node_id);
				//std::cout<<"Node before change cid = "<<p_node->get_channel_id()<<std::endl;
				int assig = Global_params::Instance()->get_chosen_assig();
				p_node->channel_function(mac_protocol_string, "colision", nchannels, 1, assig,0);
				//std::cout<<"Node change cid = "<<p_node->get_channel_id()<<std::endl;

				// We cast the Packet* into a Packet_brs_non_p* so that we can access its own methods
				if (Packet_brs_non_p* p_packet = dynamic_cast<Packet_brs_non_p*>(p_node->get_in_buffer_front())) {
					p_packet->update_cnt_backoff();
			    }
				// If the cast fails
				else {
					std::cout << "ERROR: BBCast from Packet* to Packet_brs_non_p* failed" << std::endl;
					abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
				}
                Global_params::Instance()->increase_counter_collisions(); // for every colliding node we register the cycle in which the collision occurred
			} // End of for-each
			p_channel->flush_ids_concurrent_tx_nodes();
			Global_params::Instance()->set_channel_idle(channel_id);

			//std::cout<<"COLISSION!!!!!!!!"<<std::endl;
			//return 1; // return collision code
		}
		// If only one node is transmitting
		else if(p_channel->get_ids_concurrent_tx_nodes_size() == 1) {
			int k =p_channel->get_unique_kids_concurrent_tx_nodes();
			Node* p_node = chip.at(p_channel->get_unique_kids_concurrent_tx_nodes());
			// We cast the Packet* into a Packet_brs_non_p* so that we can access its own methods
			if (Packet_brs_non_p* p_packet = dynamic_cast<Packet_brs_non_p*>(p_node->get_in_buffer_front())) {
				// We decrease cycles_left for the current packet
				p_packet->decrease_cycles_left();
				if (Global_params::Instance()->get_total_served_packets_chip() >= 0.1*Global_params::Instance()->get_npackets() && Global_params::Instance()->get_total_served_packets_chip() < 0.8*Global_params::Instance()->get_npackets()) {
					Global_params::Instance()->increase_throughput_tx_cycles();
				}

				// If at the end of this cycle we have 0 cycles left it means we successfully transmitted the packet. So do the following:
				// Empty vector of transmitting nodes, set the medium to idle, take the packet out
				// of the buffer, increase counters of total served packets per node and per chip
				int cycless = p_packet->get_cycles_left();
			 //std::cout<<"Cycles left = "<<cycless<<std::endl;
				if (p_packet->get_cycles_left() == 0) {
					p_node->pop_packet_buffer(curr_cycle);
					p_channel->flush_ids_concurrent_tx_nodes();
					Global_params::Instance()->set_channel_idle(channel_id);
					int assig = Global_params::Instance()->get_chosen_assig();

					p_node->set_channel_id(-1);

				//std::cout<<"Node :"<< k<<" TRAMITED!!!!!!!!"<<std::endl;
				}			}
			// If the cast fails
			else {
				std::cout << "ERROR: CCCast from Packet* to Packet_brs_non_p* failed" << std::endl;
				abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
			}
		}
		else {
			std::cout << "ERROR: Uncoherence detected. The size of ids_concurrent_tx_nodes is zero but the medium is set as busy" << std::endl;
			abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
		}

	}
	//std::cout<<"__________________________"<<std::endl;
}
//std::cout<<"........................................"<<std::endl;
}
//===============================================================

// Specification of TDMA for both fixed and weighted schemes
// We assume no collisions, so we don't take care of unexpected collisions
void protocol_tdma(int curr_cycle, const std::vector<int>& nodes_ready, std::vector<bool>& channel_avaible,std::vector<Node*>& chip,std::vector<Channel*>& chan, std::vector<float>& hotspotness_weights,int number_channels,std::string mac_protocol_string) {
	if (Global_params::Instance()->is_debugging_on()) {
		std::cout << "TDMA: Node " << Global_params::Instance()->get_tdma_current_node() << ". Cycles left: " << Global_params::Instance()->get_tdma_current_node_slot_size() << std::endl;
	}

	// If there's a node with a non-empty buffer, whose id matches the tdma_current_node
	std::vector<int>::const_iterator found_node = std::find(nodes_ready.begin(), nodes_ready.end(), Global_params::Instance()->get_tdma_current_node());
	if (found_node != nodes_ready.end()) {
		Node* p_node = chip.at(*found_node);
		// We cast the Packet* into a Packet_tdma* so that we can access its own methods
		if (Packet_tdma* p_packet = dynamic_cast<Packet_tdma*>(p_node->get_in_buffer_front())) {
			// We decrease cycles_left for the current packet
			p_packet->decrease_cycles_left();
			if (Global_params::Instance()->get_total_served_packets_chip() >= 0.1*Global_params::Instance()->get_npackets() && Global_params::Instance()->get_total_served_packets_chip() < 0.8*Global_params::Instance()->get_npackets()) {
				Global_params::Instance()->increase_throughput_tx_cycles();
			}

			if (Global_params::Instance()->is_debugging_on()) {
				std::cout << "Node " << p_node->get_id() << " txed for a cycle. Cycles left of current packet: " << p_packet->get_cycles_left() << std::endl;
			}

			// If at the end of this cycle we have 0 cycles left it means we successfully transmitted the packet. So do the following:
			// Take the packet out of the buffer, increase counters of total served packets per node and per chip
			if (p_packet->get_cycles_left() == 0) {
				p_node->pop_packet_buffer(curr_cycle);
				if (Global_params::Instance()->is_debugging_on()) {
					std::cout << "Packet by " << p_node->get_id() << " has been successfully tx" << std::endl;
				}
			}
		}
		// If the cast fails
		else {
			std::cout << "ERRORC: Cast from Packet* to Packet_tdma* failed" << std::endl;
			abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
		}
	}

	// now we update the TDMA status for next cycle
	// if the number of cycles left to be used in current slot is 1 it means that
    // we have to switch to next node and reset the slot size corresponding to that node
    if (Global_params::Instance()->get_tdma_current_node_slot_size() == 1) {
        // first we move to the next slot by setting the current_node to the next one
        Global_params::Instance()->update_tdma_current_node();

        // and now we set the slot size that corresponds to the next node (in fixed TDMA this is fixed)
		if (Global_params::Instance()->get_chosen_mac() == Mac_protocols::tdma_fixed) {
        	Global_params::Instance()->set_tdma_current_node_slot_size(Global_params::Instance()->get_tdma_slot_size());
		} else if (Global_params::Instance()->get_chosen_mac() == Mac_protocols::tdma_weighted){
			Global_params::Instance()->set_tdma_current_node_slot_size(Global_params::Instance()->get_tdma_slot_size() * int(ceil(hotspotness_weights.at(Global_params::Instance()->get_tdma_current_node()))));
		} else {
			std::cout << "ERROR: No matching protocol for set_tdma_current_node_slot_size" << std::endl;
			abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
		}
    } else {
        Global_params::Instance()->decrease_tdma_current_node_slot_size();
    }
}
//===============================================================

// Specification of Fuzzy token
void protocol_fuzzy_token(int curr_cycle, const std::vector<int>& nodes_ready, std::vector<bool>& channel_avaible,std::vector<Node*>& chip,std::vector<Channel*>& chan, std::vector<float>& hotspotness_weights,int number_channels,std::string mac_protocol_string) {
	std::vector<int> fuzzy_nodes_ready;
	int ncores = Global_params::Instance()->get_ncores();

	// if we're in the first period (only token holder allowed to tx)
	if (Global_params::Instance()->is_txing_token_on()) {
		if (Global_params::Instance()->is_debugging_on()) {
			std::cout << "1st period (Token-mode)" << std::endl;
			std::cout << "Token holder: node " << Global_params::Instance()->get_token_current_node() << std::endl;
			std::cout << "Fuzzy size: " << Global_params::Instance()->get_fuzzy_size() << std::endl;
		}
		// We check if we're already in the middle of a transmission, if so we decrease cycles_left and check if transmission is finished
		if (Global_params::Instance()->is_medium_busy()) {
			Node* p_node = chip.at(Global_params::Instance()->get_unique_ids_concurrent_tx_nodes());
			// We cast the Packet* into a Packet_brs_non_p* so that we can access its own methods
			if (Packet_brs_non_p* p_packet = dynamic_cast<Packet_brs_non_p*>(p_node->get_in_buffer_front())) {
				// We decrease cycles_left for the current packet
				p_packet->decrease_cycles_left();
				if (Global_params::Instance()->is_debugging_on()) {
					std::cout << "Node " << p_node->get_id() << " txed for a cycle. Cycles left of current packet: " << p_packet->get_cycles_left() << std::endl;
				}
				if (Global_params::Instance()->get_total_served_packets_chip() >= 0.1*Global_params::Instance()->get_npackets() && Global_params::Instance()->get_total_served_packets_chip() < 0.8*Global_params::Instance()->get_npackets()) {
					Global_params::Instance()->increase_throughput_tx_cycles();
				}

				// If at the end of this cycle we have 0 cycles left it means we successfully transmitted the packet. So do the following:
				// Empty vector of transmitting nodes (only one, the token holder), set the medium to idle, take the packet out
				// of the buffer, update the token ID (pass the token)
				if (p_packet->get_cycles_left() == 0) {
					int lat = curr_cycle - p_node->get_in_buffer_front()->get_inj_time() + 1;
					p_node->pop_packet_buffer(curr_cycle); // this pops the packet out and also updates statistics (total served packets per node and per chip)
					if (Global_params::Instance()->is_debugging_on()) {
						std::cout << "Packet by " << p_node->get_id() << " has been successfully tx (latency " << lat << ")" << std::endl;
					}
					Global_params::Instance()->flush_ids_concurrent_tx_nodes();
					Global_params::Instance()->set_medium_idle();
					Global_params::Instance()->update_token_current_node();
					Global_params::Instance()->rotate_tx_weights(1); // since we have passed the token to next node, we shift all tx weights one position as well
					if (Global_params::Instance()->is_debugging_on()) {
						std::cout << "TOKEN PASSED" << std::endl;
					}
					// Notice we leave txing_token=on so that it skips the second period and starts again next epoch in token mode
				}
			}
			// If the cast fails
			else {
				std::cout << "ERROR: Cast from Packet* to Packet_brs_non_p* failed" << std::endl;
				abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
			}
		}
		// If not (meaning we are in first cycle of first period), we check if the token holder has packets to tx
		else {
			std::vector<int>::const_iterator found_node = std::find(nodes_ready.begin(), nodes_ready.end(), Global_params::Instance()->get_token_current_node());
			// if we find the token holder in the list of nodes that are ready to send, the token holder starts tx
			if (found_node != nodes_ready.end()) {
				Global_params::Instance()->set_medium_busy();
				Global_params::Instance()->push_ids_concurrent_tx_nodes(*found_node);
				Node* p_node = chip.at(*found_node);
				// We cast the Packet* into a Packet_brs_non_p* so that we can access its own methods
				if (Packet_brs_non_p* p_packet = dynamic_cast<Packet_brs_non_p*>(p_node->get_in_buffer_front())) {
					// We decrease cycles_left for the current packet
					p_packet->decrease_cycles_left();
					if (Global_params::Instance()->is_debugging_on()) {
						std::cout << "Node " << p_node->get_id() << " txed for a cycle. Cycles left of current packet: " << p_packet->get_cycles_left() << std::endl;
					}
					if (Global_params::Instance()->get_total_served_packets_chip() >= 0.1*Global_params::Instance()->get_npackets() && Global_params::Instance()->get_total_served_packets_chip() < 0.8*Global_params::Instance()->get_npackets()) {
						Global_params::Instance()->increase_throughput_tx_cycles();
					}

					// If at the end of this cycle we have 0 cycles left it means we successfully transmitted the packet. So do the following:
					// Empty vector of transmitting nodes, set the medium to idle, take the packet out
					// of the buffer, increase counters of total served packets per node and per chip, pass the token and start next epoch
					if (p_packet->get_cycles_left() == 0) {
						int lat = curr_cycle - p_node->get_in_buffer_front()->get_inj_time() + 1;
						p_node->pop_packet_buffer(curr_cycle);
						if (Global_params::Instance()->is_debugging_on()) {
							std::cout << "Packet by " << p_node->get_id() << " has been successfully tx (latency " << lat << ")" << std::endl;
						}
						if(Global_params::Instance()->get_ids_concurrent_tx_nodes_size() == 1) {
							if (Global_params::Instance()->get_unique_ids_concurrent_tx_nodes() == *found_node) {
								Global_params::Instance()->flush_ids_concurrent_tx_nodes();
								Global_params::Instance()->set_medium_idle();
								Global_params::Instance()->update_token_current_node();
								Global_params::Instance()->rotate_tx_weights(1); // since we have passed the token to next node, we shift all tx weights one position as well
								if (Global_params::Instance()->is_debugging_on()) {
									std::cout << "TOKEN PASSED" << std::endl;
								}
								// Notice we leave txing_token=on so that it skips the second period and starts again next epoch in token mode
							}
							else {
								std::cout << "!!!!!!!!!!! ERROR: get_unique_ids_concurrent_tx_nodes is different than token holder" << std::endl;
								abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
							}
						}
						else {
							std::cout << "!!!!!!!!!!! ERROR: get_ids_concurrent_tx_nodes_size is different than 1, this doesn't make sense" << std::endl;
							abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
						}
					}
				}
				// If the cast fails
				else {
					std::cout << "ERROR: Cast from Packet* to Packet_brs_non_p* failed" << std::endl;
				}
			}
			//otherwise if token holder has nothing to tx, we predict load is low so we increase fuzzy area
			else {
				if (Global_params::Instance()->is_debugging_on()) {
					std::cout << "Token holder has nothing to tx" << std::endl;
				}

				Global_params::Instance()->increase_fuzzy_size(); // this passes token and also updates vector of tx probabilities

				// If fuzzy size is below threshold, we skip 2nd period and start next epoch in token mode again (pure token)
				if ( Global_params::Instance()->get_fuzzy_size() <= ceil((Global_params::Instance()->get_thr_pure_token())*ncores) ) {
					Global_params::Instance()->update_token_current_node();
					Global_params::Instance()->rotate_tx_weights(1); // since we have passed the token to next node, we shift all tx weights one position as well
					if (Global_params::Instance()->is_debugging_on()) {
						std::cout << "Fuzzy size (" << Global_params::Instance()->get_fuzzy_size() << ") below or equal to threshold (";
						std::cout << ceil((Global_params::Instance()->get_thr_pure_token())*ncores) << "). PURE TOKEN" << std::endl;
						std::cout << "TOKEN PASSED" << std::endl;
					}
				}
				// Otherwise (we are in hybrid mode), we turn off token mode and get ready for 2nd period as usual
				else {
					Global_params::Instance()->set_txing_token_off(); // we switch from first to second period
				}
			}
		}
	}
	// otherwise (if we are in second period) we do BRS (a modified version, see comments below) in some nodes
	else {
		if (Global_params::Instance()->is_debugging_on()) {
			std::cout << "2nd period (BRS-mode)" << std::endl;
			std::cout << "Token holder: node " << Global_params::Instance()->get_token_current_node() << std::endl;
			std::cout << "Fuzzy size: " << Global_params::Instance()->get_fuzzy_size() << std::endl;
		}

		// IF MEDIUM IS IDLE
		if (!Global_params::Instance()->is_medium_busy()) {
			// DESIGN OPTION 1 ----------------WE CHECK WHICH NODES ARE INSIDE FUZZY AREA AND ARE NON-EMTPY, AND WE ONLY ALLOW THOSE TO TX WITH PROBABILITY P_i
			//								   THE PROBABILITY P_i MIGHT BE 1 OR 1/FUZZY_SIZE OR SOMETHING DIFFERENT DEPENDING ON THE DESIGN CHOICE (LOOK FOR "DESIGN OPTION 1 - PART 2")
			// // If the medium is idle (we are in first cycle of second period), check if any node inside fuzzy area has packets to tx
			// // FIXME: temporary hack (adapting fuzzy area to remove the token holder)
			// // fuzzy_size++;
            //
			// for (auto& node : nodes_ready) {
			// 	if ((Global_params::Instance()->get_token_current_node() + Global_params::Instance()->get_fuzzy_size()) >= ncores) {
			// 		if (value_in_range(node, Global_params::Instance()->get_token_current_node(), ncores-1) || value_in_range(node, 0, Global_params::Instance()->get_token_current_node() + Global_params::Instance()->get_fuzzy_size()-ncores)) {
			// 			fuzzy_nodes_ready.push_back(node);
			// 		}
			// 	} else {
			// 		if (value_in_range(node, Global_params::Instance()->get_token_current_node(), Global_params::Instance()->get_token_current_node() + Global_params::Instance()->get_fuzzy_size() - 1)) {
			// 			fuzzy_nodes_ready.push_back(node);
			// 		}
			// 	}
			// }
            //
			// // WARNING: this was even commented out in design option 1, which we used to test whether removing the token holder in the 2nd period made any difference (it didn't)
			// // std::vector<int>::const_iterator found_node = std::find(fuzzy_nodes_ready.begin(), fuzzy_nodes_ready.end(), Global_params::Instance()->get_token_current_node());
			// // if (found_node != fuzzy_nodes_ready.end()) {
			// // 	fuzzy_nodes_ready.erase(found_node);
			// // }
			// END DESIGN OPTION 1 ---------

			// DESIGN OPTION 2 -------------WE USE TX_WEIGHTS VECTOR, SO THAT NOW FUZZY_NODES_READY CONTAINS ALL NON-EMTPY NODES THAT PICKED A NUMBER SMALLER
			//								THAN THEIR PROBABILITY, SO WE SHOULD NOT USE THE WEIGHTS BELOW ON TOP OF THAT. THIS IS DIFFERENT THAN DESIGN OPTION 1
			// 								BECAUSE NOW WE APPLY WEIGHTS TO ALL NODES IN CHIP (FUZZY_SIZE WILL DETERMINE SHARPNESS OF CURVE) AND SAVE IN FUZZY_NODES_READY
			//								THE ONES THAT GOT PICKED AND HAD SOMETHING TO TX. IN DESIGN OPTION 1 WE SAVED ONLY NON-EMTPY NODES INSIDE FUZZY AREA, AND THEN
			//								WE APPLIED A WEIGHT (WHICH WAS EITHER 1 OR 1/FUZZY_SIZE) TO EACH OF THOSE TO DECIDE WHETHER THEY SHOULD TRY TO TX OR NOT

			// For each node in chip with a non-empty buffer
			for (std::vector<int>::const_iterator curr_node_id = nodes_ready.begin(); curr_node_id != nodes_ready.end(); ++curr_node_id) {
				Node* p_node = chip.at(*curr_node_id);
				float value_picked = p_node->get_randomly_uniform_value(); // current node picks randomly uniform value between 0 and 1
				if (Global_params::Instance()->is_debugging_on()) {
					std::cout << "NON-EMPTY Node " << *curr_node_id << " has tx p=";
					// WARNING: the probability is obtained from the gaussian distribution, but this sometimes is not uniform, since we shuffle the values upon a collision
					std::cout << Global_params::Instance()->tx_weights[Global_params::Instance()->mapping_node_ids[*curr_node_id]] << " and picked " << value_picked << std::endl;
				}

				// if current node picked random value below it's probability limit, it will try to tx
				// WARNING: the probability is obtained from the gaussian distribution, but this sometimes is not uniform, since we shuffle the values upon a collision
				if (value_picked <= Global_params::Instance()->tx_weights[Global_params::Instance()->mapping_node_ids[*curr_node_id]]) {
					fuzzy_nodes_ready.push_back(*curr_node_id);
					if (Global_params::Instance()->is_debugging_on()) {
						std::cout << "CONTENDER Node " << *curr_node_id << " got lucky and will try to tx" << std::endl;
					}
				}
			}
			// END DESIGN OPTION 2----------

			// If the nodes inside the fuzzy area either had no packets to transmit or had probability to tx too low (during the second period)
			if (fuzzy_nodes_ready.size() == 0) {
				if (Global_params::Instance()->is_debugging_on()) {
					std::cout << "Fuzzy area nodes did not tx (either no packets or tx probability too low)" << std::endl;
				}

				// AF: BIG CHANGE, WITH SERGI'S NEW MECHANISM (SERGI EXTRAVAGANZA 1), WE STAY IN BRS FOR AS LONG AS THERE'S NO COLLISION. SO I COMMENTED OUT THE LINES BELOW
				// --------------------------------- BEGINNING OF CODE AFFECTED
				// // If fuzzy size is above threshold, we keep txing_token=false to skip 1st period and start next epoch in BRS mode again (pure BRS)
				// if ( Global_params::Instance()->get_fuzzy_size() >= ceil((Global_params::Instance()->get_thr_pure_brs())*ncores) ) {
				// 	if (Global_params::Instance()->is_debugging_on()) {
				// 		std::cout << "Fuzzy size (" << Global_params::Instance()->get_fuzzy_size() << ") above threshold (";
				// 		std::cout << ceil((Global_params::Instance()->get_thr_pure_brs())*ncores) << "). PURE BRS" << std::endl;
				// 	}
				// }
				// // Otherwise (we are in hybrid mode), we turn on token mode and get ready for 1st period of next epoch as usual
				// else {
				// 	Global_params::Instance()->set_txing_token_on(); // at the end of the second period, prepare for next epoch
				// }

				// also now with this new version of protocol we increase fuzzy area when there's no tx in BRS mode
				Global_params::Instance()->increase_fuzzy_size();
				// --------------------------------- END OF CODE AFFECTED


				Global_params::Instance()->update_token_current_node(); // we pass the token (implicitly)
				Global_params::Instance()->rotate_tx_weights(1); // since we have passed the token to next node, we shift all tx weights one position as well
				// Global_params::Instance()->randomize_mapping_node_ids(); // WARNING: we shuffle the probabilities so that we avoid non-emtpy nearby nodes having similar probability
				if (Global_params::Instance()->is_debugging_on()) {
					std::cout << "TOKEN PASSED" << std::endl;
				}
			}
			// Otherwise (if one or more nodes inside fuzzy area have packets)
			else {
				// For each node in fuzzy-area with a non-empty buffer
				for (std::vector<int>::const_iterator curr_node_id = fuzzy_nodes_ready.begin(); curr_node_id != fuzzy_nodes_ready.end(); ++curr_node_id) {
					Node* p_node = chip.at(*curr_node_id);

					// TODO: !!!!!!!!!!!!!
					//		*TX PROBABILITY SHOULD BE GAUSSIAN INSTEAD OF UNIFORM AMONG FUZZY NODES
					//		*NEIGHBOR NODES SHOULD HAVE MUCH DIFFERENT PROBABILITY SO THAT THEY DON'T KEEP FALLING IN SAME FUZZY AREA AND TRY TO TX AT SAME TIME
					//		*THIS HAS TO BE DONE WITH A PROBABILITY P_i, WHICH DEPENDS ON THE DISTANCE FROM THE TOKEN HOLDER

					// DESIGN OPTION 1 (PART 2) ----------- WE CONSIDER ONLY NON-EMPTY NODES INSIDE FUZZY AREA, WE SAVE THOSE IDS IN FUZZY_NODES_READY
					//										AMONG ALL FUZZY_NODES_READY, THEY CAN TRANSMIT WITH A CERTAIN PROBABILITY
					//										WARNING: IF WE USE DESIGN OPTION 2 (USE TX_WEIGHTS VECTOR) THEN FUZZY_NODES_READY CONTAINS ALL NON-EMTPY NODES
					//										THAT PICKED A NUMBER SMALLER THAN THEIR PROBABILITY, SO WE SHOULD NOT USE THE WEIGHTS BELOW ON TOP OF THAT
					//										THEREFORE IF WE USE DESIGN OPTION 2 WE SHOULD ALWAYS HAVE IF(TRUE) BELOW
					// Nodes inside fuzzy area transmit with uniform probability P_i=(1/fuzzy_size), such that sum of all P_i is equal to 1
					// WARNING: IF THIS IF IS TRUE INSTEAD OF CHECKING PROBABILITY, WE'RE NOT USING THE PROBABILITY WEIGHTS!!!!!!!!!
					if (true) { // WARNING: NO WEIGHTS
					// if (p_node->get_randomly_uniform_value() <= 0.5) { // WARNING: WEIGHTS 1/2 (ASK SERGI)
					// if (p_node->get_randomly_uniform_value() <= (1.0/Global_params::Instance()->get_fuzzy_size())) { // WARNING: WEIGHTS 1/FUZZY_SIZE
					// if (p_node->get_randomly_uniform_value() <= (1.0/fuzzy_nodes_ready.size())) { // WARNING: CHEAT WEIGHTS
						// We cast the Packet* into a Packet_brs_non_p* so that we can access its own methods
						if (Packet_brs_non_p* p_packet = dynamic_cast<Packet_brs_non_p*>(p_node->get_in_buffer_front())) {
							// we transmit first cycle/preamble of packet
							Global_params::Instance()->set_medium_busy();
							Global_params::Instance()->push_ids_concurrent_tx_nodes(*curr_node_id);
							if (Global_params::Instance()->is_debugging_on()) {
								std::cout << "Node " << p_node->get_id() << " txed header" << std::endl;
							}
							// Notice we don't decrease the cycles_left of the packet, since we have to leave one extra cycle after the header to check for collisions
					    }
						// If the cast fails
						else {
							std::cout << "ERROR: Cast from Packet* to Packet_brs_non_p* failed" << std::endl;
							abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
						}
					}
					// if a node inside the fuzzy area didn't pick a lucky number (probability 1/fuzzy_size)
					else {
						if (Global_params::Instance()->is_debugging_on()) {
							std::cout << "Node " << p_node->get_id() << " didn't pick lucky number" << std::endl;
						}
					}
				}
			}
		}
		// However, if the medium is busy it means someone either sent the header or is successfully txing payload, so do the following:
		// -check if vector of transmitting nodes has size more than 1 (collision of headers)
		// -if it's more than 1 it means a collision has been detected and therefore an (imaginary) NACK was sent in current cycle.
		// At this point we have to empty the vector of transmitting nodes, set the medium to idle again, update parameters of fuzzy-token (area, collisions, idles, etc.)
		// and get ready to start the next epoch in token mode. Note that we don't do anything with the collision (no backoff nor anything, we just start next epoch)
		// -if there was only one transmitting node we decrease the cycles_left and check if cycles_left is zero, if it's zero it means we'll be
		// done tx at the end of this cycle. So we empty the vector of transmitting nodes, we set the medium to idle, we take the packet out
		// of the buffer, we increase counters of total served packets per node and per chip, and we get ready for next expoch by setting token mode on
		else {
			// first we check if any two or more headers collided in previous cycle
			// TODO: this check occurs at each cycle, even when we already know that the headers didn't collide. You should avoid checking for collisions at each cycle
			// IF WE FOUND A COLLISION
			if(Global_params::Instance()->get_ids_concurrent_tx_nodes_size() > 1) {

				// save the IDs of all collided nodes
				std::string collided_nodes = "";
				std::string separator = ""; // separator for the first iteration of the for-each
				for (std::vector<int>::const_iterator curr_node_id = Global_params::Instance()->ids_concurrent_tx_nodes_begin(); curr_node_id != Global_params::Instance()->ids_concurrent_tx_nodes_end(); ++curr_node_id) {
					Node* p_node = chip.at(*curr_node_id);
					// We cast the Packet* into a Packet_brs_non_p* so that we can access its own methods
					if (Packet_brs_non_p* p_packet = dynamic_cast<Packet_brs_non_p*>(p_node->get_in_buffer_front())) {
						collided_nodes += ( separator + std::to_string(p_node->get_id()) );
						separator = ", "; // separator for the second and following iterations
				    }
					// If the cast fails
					else {
						std::cout << "ERROR: Cast from Packet* to Packet_brs_non_p* failed" << std::endl;
						abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
					}
                    Global_params::Instance()->increase_counter_collisions(); // for every colliding node we register the cycle in which the collision occurred
				}

				// Notice we don't backoff if there's a collision, instead we simply end the second period and get ready for the next epoch
				Global_params::Instance()->flush_ids_concurrent_tx_nodes();
				Global_params::Instance()->set_medium_idle();
				Global_params::Instance()->decrease_fuzzy_size();

				// If fuzzy size is above threshold, we keep txing_token=false to skip 1st period and start next epoch in BRS mode again (pure BRS)
				if ( Global_params::Instance()->get_fuzzy_size() >= ceil((Global_params::Instance()->get_thr_pure_brs())*ncores) ) {
					if (Global_params::Instance()->is_debugging_on()) {
						std::cout << "Fuzzy size (" << Global_params::Instance()->get_fuzzy_size() << ") above or equal to threshold (";
						std::cout << ceil((Global_params::Instance()->get_thr_pure_brs())*ncores) << "). PURE BRS" << std::endl;
					}
				}
				// Otherwise (we are in hybrid mode), we turn on token mode and get ready for 1st period of next epoch as usual
				else {
					Global_params::Instance()->set_txing_token_on(); // at the end of the second period, prepare for next epoch
				}
				Global_params::Instance()->update_token_current_node(); // we pass the token (implicitly)
				Global_params::Instance()->rotate_tx_weights(1); // since we have passed the token to next node, we shift all tx weights one position as well
				// Global_params::Instance()->randomize_mapping_node_ids(); // WARNING: we shuffle the probabilities so that we avoid non-emtpy nearby nodes having similar probability
				if (Global_params::Instance()->is_debugging_on()) {
					std::cout << "Header collision detected (nodes: " << collided_nodes << ")" << std::endl;
					std::cout << "TOKEN PASSED" << std::endl;
				}
			}

			// otherwise it means only one node is transmitting (either header or payload)
			else if(Global_params::Instance()->get_ids_concurrent_tx_nodes_size() == 1) {
				Node* p_node = chip.at(Global_params::Instance()->get_unique_ids_concurrent_tx_nodes());
				// We cast the Packet* into a Packet_brs_non_p* so that we can access its own methods
				if (Packet_brs_non_p* p_packet = dynamic_cast<Packet_brs_non_p*>(p_node->get_in_buffer_front())) {
					// We decrease cycles_left for the current packet
					// If this happens in the cycle right after header (where we check for collisions), we will be decreasing the cycle corresponding to the header
					// And if it happens in any cycle after that intermediate, we will be decreasing the cycles corresponding to the payload
					p_packet->decrease_cycles_left();

					// if the header isn't marked as sent yet, it means this is the cycle where we checked for collisions
					if (!p_packet->is_header_sent()) {
						p_packet->set_header_sent(); // mark header as sent
						if (Global_params::Instance()->is_debugging_on()) {
							std::cout << "No header collision detected in intermediate cycle" << std::endl;
						}
					}
					else if (Global_params::Instance()->is_debugging_on()) {
						std::cout << "Node " << p_node->get_id() << " txed for a cycle. Cycles left of current packet: " << p_packet->get_cycles_left() << std::endl;
					}

					// TODO: do we need this way of collecting statistics? Is it considering the intermediate cycles as well? I don't think it should!
					if (Global_params::Instance()->get_total_served_packets_chip() >= 0.1*Global_params::Instance()->get_npackets() && Global_params::Instance()->get_total_served_packets_chip() < 0.8*Global_params::Instance()->get_npackets()) {
						Global_params::Instance()->increase_throughput_tx_cycles();
					}

					// If at the end of this cycle we have 0 cycles left it means we successfully transmitted the packet. So do the following:
					// Empty vector of transmitting nodes, set the medium to idle, take the packet out
					// of the buffer, increase counters of total served packets per node and per chip
					if (p_packet->get_cycles_left() == 0) {
						int lat = curr_cycle - p_node->get_in_buffer_front()->get_inj_time() + 1;
						p_node->pop_packet_buffer(curr_cycle);
						if (Global_params::Instance()->is_debugging_on()) {
							std::cout << "Packet by " << p_node->get_id() << " has been successfully tx (latency " << lat << ")" << std::endl;
						}
						Global_params::Instance()->flush_ids_concurrent_tx_nodes();
						Global_params::Instance()->set_medium_idle();

						// AF: BIG CHANGE, WITH SERGI'S NEW MECHANISM ("EXTRAVAGANZA 1"), WE STAY IN BRS FOR AS LONG AS THERE'S NO COLLISION
						// --------------------------------- BEGINNING OF CODE AFFECTED
						// // If fuzzy size is above threshold, we keep txing_token=false to skip 1st period and start next epoch in BRS mode again (pure BRS)
						// if ( Global_params::Instance()->get_fuzzy_size() >= ceil((Global_params::Instance()->get_thr_pure_brs())*ncores) ) {
						// 	if (Global_params::Instance()->is_debugging_on()) {
						// 		std::cout << "Fuzzy size (" << Global_params::Instance()->get_fuzzy_size() << ") above threshold (";
						// 		std::cout << ceil((Global_params::Instance()->get_thr_pure_brs())*ncores) << "). PURE BRS" << std::endl;
						// 	}
						// }
						// // Otherwise (we are in hybrid mode), we turn on token mode and get ready for 1st period of next epoch as usual
						// else {
						// 	Global_params::Instance()->set_txing_token_on(); // at the end of the second period, prepare for next epoch
						// }
						// --------------------------------- END OF CODE AFFECTED


						Global_params::Instance()->update_token_current_node(); // we pass the token (implicitly)
						Global_params::Instance()->rotate_tx_weights(1); // since we have passed the token to next node, we shift all tx weights one position as well
						if (Global_params::Instance()->is_debugging_on()) {
							std::cout << "TOKEN PASSED" << std::endl;
						}

					}
				}
				// If the cast fails
				else {
					std::cout << "ERROR: Cast from Packet* to Packet_brs_non_p* failed" << std::endl;
					abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
				}
			}
			else {
				std::cout << "ERROR: Uncoherence detected. The size of ids_concurrent_tx_nodes is zero but the medium is set as busy" << std::endl;
				abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
			}
		}
	}
}
//===============================================================

// Specification of Token
// We assume no collisions, so we don't take care of unexpected collisions
void protocol_token(int curr_cycle, const std::vector<int>& nodes_ready, std::vector<bool>& channel_avaible, std::vector<Node*>& chip,std::vector<Channel*>& chan, std::vector<float>& hotspotness_weights,int number_channels,std::string mac_protocol_string) {
int assig = Global_params::Instance()->get_chosen_assig();
for (int channel_id = 0; channel_id < number_channels; channel_id++) {

	Channel* p_channel = chan.at(channel_id);
	//int si = nodes_ready.size();
	//std::cout << "nodes_ready= "<< si<<std::endl;
	//		for(int h=0;h<nodes_ready.size();h++){
	//		Node* act = chip.at(nodes_ready[h]);
	//std::cout<<nodes_ready[h]<<" chan: "<<act->get_channel_id()<<std::endl;
	//		}

	if (Global_params::Instance()->is_debugging_on()) {
		std::cout << "Token: Node " << Global_params::Instance()->get_token_current_node() << std::endl;
	}

	// We check if we're already in the middle of a transmission, if so we decrease cycles_left and check if transmission is finished
	if (Global_params::Instance()->is_channel_busy(channel_id)) {
//	std::cout<<"Inside channel busy: "<<channel_id<<std::endl;
		Node* p_node = chip.at(p_channel->get_unique_kids_concurrent_tx_nodes());
			int node_channel = p_node->get_channel_id();
		//	std::cout<< "Node cid = "<<node_channel<<std::endl;
		if(p_node->get_channel_id()==channel_id){
		// We cast the Packet* into a Packet_tdma* so that we can access its own methods
		if (Packet_tdma* p_packet = dynamic_cast<Packet_tdma*>(p_node->get_in_buffer_front())) {
			// We decrease cycles_left for the current packet
			p_packet->decrease_cycles_left();
//	std::cout<<"Cycles left = "<<p_packet->get_cycles_left() <<std::endl;
			if (Global_params::Instance()->is_debugging_on()) {
			std::cout << "Node " << p_node->get_id() << " txed for a cycle. Cycles left of current packet: " << p_packet->get_cycles_left() << std::endl;
			}
			if (Global_params::Instance()->get_total_served_packets_chip() >= 0.1*Global_params::Instance()->get_npackets() && Global_params::Instance()->get_total_served_packets_chip() < 0.8*Global_params::Instance()->get_npackets()) {
				Global_params::Instance()->increase_throughput_tx_cycles();
			}

			// If at the end of this cycle we have 0 cycles left it means we successfully transmitted the packet. So do the following:
			// Empty vector of transmitting nodes (only one, the token holder), set the medium to idle, take the packet out
			// of the buffer, update the token ID (pass the token)
			if (p_packet->get_cycles_left() == 0) {
				int lat = curr_cycle - p_node->get_in_buffer_front()->get_inj_time() + 1;
				p_node->pop_packet_buffer(curr_cycle); // this pops the packet out and also updates statistics (total served packets per node and per chip)
				if (Global_params::Instance()->is_debugging_on()) {
				std::cout << "Packet by " << p_node->get_id() << " has been successfully tx (latency " << lat << ")" << std::endl;
				}
				p_channel->flush_ids_concurrent_tx_nodes();
				Global_params::Instance()->set_channel_idle(channel_id);
				channel_avaible[channel_id] = true;
				//p_channel->update_token_current_node(assig);

					if(assig == 2 || assig == 3){
					p_node->set_channel_id(-1);
				}
	//	std::cout<<"Token updated: "<<p_channel->get_token_current_node()<<std::endl;
		//		std::cout << "TOKEN PASSED" << std::endl;

			}
		}

		// If the cast fails
		else {
			std::cout << "ERRORA: Cast from Packet* to Packet_tdma* failed" << std::endl;
			abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
		}
	}
	}
	// Otherwise (medium is idle), we check if the token holder has packets to tx
	else {
//	std::cout<<"Indide channel idel = "<< channel_id<<std::endl;
		std::vector<int>::const_iterator found_node = std::find(nodes_ready.begin(), nodes_ready.end(), p_channel->get_token_current_node());

		// if we find the token holder in the list of nodes that are ready to send (token holder has a packet to send), the token holder starts tx
		if (found_node != nodes_ready.end()) {
			Node* p_node = chip.at(*found_node);
			p_node->channel_function(mac_protocol_string, "error", number_channels, 1, assig,channel_id);
			if(p_node->get_channel_id()==channel_id){
			p_channel->push_ids_concurrent_tx_nodes(*found_node);

		//	std::cout<<"Id Node token = "<< p_node->get_id()<<" chan id: "<<p_node->get_channel_id()<<std::endl;

					Global_params::Instance()->set_channel_busy(channel_id);
			// We cast the Packet* into a Packet_tdma* so that we can access its own methods
			if (Packet_tdma* p_packet = dynamic_cast<Packet_tdma*>(p_node->get_in_buffer_front())) {
				// We decrease cycles_left for the current packet
				p_packet->decrease_cycles_left();
		//	std::cout<<"Cycles left = "<<p_packet->get_cycles_left()<<std::endl;
				if (Global_params::Instance()->is_debugging_on()) {
		//	std::cout << "Node " << p_node->get_id() << " txed for a cycle. Cycles left of current packet: " << p_packet->get_cycles_left() << std::endl;
				}
				if (Global_params::Instance()->get_total_served_packets_chip() >= 0.1*Global_params::Instance()->get_npackets() && Global_params::Instance()->get_total_served_packets_chip() < 0.8*Global_params::Instance()->get_npackets()) {
					Global_params::Instance()->increase_throughput_tx_cycles();
				}

				// If at the end of this cycle we have 0 cycles left it means we successfully transmitted the packet. So do the following:
				// Empty vector of transmitting nodes, set the medium to idle, take the packet out of the buffer,
				// increase counters of total served packets per node and per chip, pass the token and start next epoch
				if (p_packet->get_cycles_left() == 0) {
					int lat = curr_cycle - p_node->get_in_buffer_front()->get_inj_time() + 1;
					p_node->pop_packet_buffer(curr_cycle); // this pops the packet out and also updates statistics (total served packets per node and per chip)
					if (Global_params::Instance()->is_debugging_on()) {
					std::cout << "Packet by " << p_node->get_id() << " has been successfully tx (latency " << lat << ")" << std::endl;
					}
					if(p_channel->get_ids_concurrent_tx_nodes_size() == 1) {
						if (p_channel->get_unique_kids_concurrent_tx_nodes() == *found_node) {
							p_channel->flush_ids_concurrent_tx_nodes();
							Global_params::Instance()->set_channel_idle(channel_id);
								channel_avaible[channel_id] = true;
							//	p_channel->update_token_current_node(assig);
								if(assig == 2 || assig == 3){
								p_node->set_channel_id(-1);
							}
				//		std::cout<<"Token updated: "<<p_channel->get_token_current_node()<<std::endl;
				//	std::cout << "TOKEN PASSED 2" << std::endl;


						}
						else {
							std::cout << "!!!!!!!!!!! ERROR: get_unique_ids_concurrent_tx_nodes is different than token holder" << std::endl;
							abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
						}
					}
					else {
						std::cout << "!!!!!!!!!!! ERROR: get_ids_concurrent_tx_nodes_size is different than 1, this doesn't make sense" << std::endl;
						abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
					}
				}
			}
			// If the cast fails
			else {
				std::cout << "ERRORB: Cast from Packet* to Packet_tdma* failed" << std::endl;
			}
		}
		else {
			if(assig==2 || assig==3){
		//	std::cout<<"Token updated: "<<p_channel->get_token_current_node()<<std::endl;
			//p_channel->update_token_current_node(assig);
			int k = p_node->get_id();
			channel_avaible[channel_id] = true;



//std::cout<<"Next token up3dated: "<<p_channel->get_token_current_node()<<std::endl;
			if (Global_params::Instance()->is_debugging_on()) {
	//		std::cout << "Token holder has nothing to tx" << std::endl;
	//			std::cout << "TOKEN PASSED" << std::endl;
			}

	}
}
	}
		//otherwise if token holder has nothing to tx, we pass the token right away
		else {
	//		std::cout<<"Token up5dated: "<<p_channel->get_token_current_node()<<std::endl;

				//p_channel->update_token_current_node(assig);
				channel_avaible[channel_id] = true;

			if (Global_params::Instance()->is_debugging_on()) {
	//			std::cout << "Token holder has nothing to tx" << std::endl;
	//		std::cout << "TOKEN PASSED" << std::endl;
			}

	}
	}
//	std::cout<<"***************************************"<<std::endl;
}//for channels
std::vector<int> chan_token(number_channels);
int k = 0;

//std::cout<<"Vector chan= ";
for(int o = 0; o<number_channels;o++){
	//std::cout<<channel_avaible[o]<<" ";
	Channel* p_channel = chan.at(o);
	if(channel_avaible[o] == true){
			p_channel->update_token_current_node(assig);
	}
		chan_token[o]=p_channel->get_token_current_node();
}
//std::cout<<"\n";
//std::cout<<"Vector token= ";

int num =0;
if(assig ==3){
	num = rand() %2;
}
for(int j = 0; j<number_channels; j++){
		//std::cout<<chan_token[j]<<" ";
	for(int i=0; i<number_channels-1;i++){

		if(chan_token[j]==chan_token[(j+i+1)%number_channels]){
			//std::cout<<"puja ' "<<chan_token[j]<<"'";
			if((num % 2) == 0){
				//std::cout<<"Parell: "<<num<<"TTT   \n";
			if(channel_avaible[j]==true){
				
				Channel* p_channel = chan.at(j);
				p_channel->update_token_current_node(assig);
				chan_token[j]=p_channel->get_token_current_node();
			}else{
				Channel* p_channel = chan.at((j+i+1)%number_channels);
				p_channel->update_token_current_node(assig);
				chan_token[((j+i+1)%number_channels)]=p_channel->get_token_current_node();
			}
			}else{
				
				//std::cout<<"Inparell \n";
				if (channel_avaible[j+1]==true){
				Channel* p_channel = chan.at((j+i+1)%number_channels);
				p_channel->update_token_current_node(assig);
				chan_token[((j+i+1)%number_channels)]=p_channel->get_token_current_node();

			}else{
				Channel* p_channel = chan.at(j);
				p_channel->update_token_current_node(assig);
				chan_token[j]=p_channel->get_token_current_node();
			}
			}

		}
	}
}

//std::cout<<"\n";

//std::cout<<"Vector token final= ";
//for(int o = 0; o<number_channels;o++){
//	std::cout<<chan_token[o]<<" ";
//}
//std::cout<<"\n";

//std::cout<<"\n\n-------------------------------------"<<std::endl;
}
//===============================================================
