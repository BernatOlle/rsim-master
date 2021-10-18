#include "packet_csma_non_p.hpp"

Packet_csma_non_p::Packet_csma_non_p(int inj_time, int src_node) : Packet(inj_time, src_node) {
	backoff_exp = 1;
	update_cnt_backoff();

}

int Packet_csma_non_p::get_backoff_exp() {
	return backoff_exp;
}

void Packet_csma_non_p::decrease_cnt_backoff() {
	cnt_backoff--;
}

int Packet_csma_non_p::get_cnt_backoff() {
	return cnt_backoff;
}

void Packet_csma_non_p::increase_backoff_exp() {
	backoff_exp++;
}

void Packet_csma_non_p::reset_backoff_exp() {
	backoff_exp = 1;
}

// Choose randomly a new backoff time between 1 and 2^backoff_exp
void Packet_csma_non_p::update_cnt_backoff() {
	cnt_backoff = (rand() % (int)pow(2,backoff_exp))+1;
}
