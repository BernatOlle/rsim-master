#include "packet.hpp"

Packet::Packet(int inj_time, int src_node) : inj_time(inj_time), src_node(src_node) {

}

int Packet::get_inj_time() {
    return inj_time;
}
