#include "packet_brs_non_p.hpp"

Packet_brs_non_p::Packet_brs_non_p(int inj_time, int src_node) : Packet(inj_time, src_node) {
	cycles_left = Global_params::Instance()->get_tx_time();

	backoff_exp = 0;
	cnt_backoff = 0;
	header_sent = false;
	//update_cnt_backoff();
	channel="";

}
int Packet::get_channelID(){
 //return channel.get_channel_Id;
}

int Packet_brs_non_p::get_cycles_left() {
	return cycles_left;
}

void Packet_brs_non_p::reset_cycles_left() {
	cycles_left = Global_params::Instance()->get_tx_time();
}

void Packet_brs_non_p::decrease_cycles_left() {
	cycles_left--;
}

bool Packet_brs_non_p::is_header_sent() {
	return header_sent;
}

void Packet_brs_non_p::set_header_sent() {
	header_sent = true;
}

int Packet_brs_non_p::get_backoff_exp() {
	return backoff_exp;
}

void Packet_brs_non_p::decrease_cnt_backoff() {
	cnt_backoff--;
}

int Packet_brs_non_p::get_cnt_backoff() {
	return cnt_backoff;
}

void Packet_brs_non_p::reset_backoff_exp() {
	backoff_exp = 0;
}

// Choose randomly a new backoff time between 1 and 2^backoff_exp
// Every time we update is because there has been a collision, therefore we increase the
// contention window by the double. The first time we update is when the packet is added to the buffer
// this is why is set to 0, so that the first time we update, the exp is 1.
void Packet_brs_non_p::update_cnt_backoff() {
	backoff_exp++;
	cnt_backoff = (rand() % (int)pow(2,backoff_exp))+1;
}

void Packet_brs_non_p::set_cnt_backoff_zero() {
	cnt_backoff = 0;
}
