//=================================
// include guard
#ifndef PACKET_H
#define PACKET_H

//=================================
// included dependencies

//=================================
class Packet {
private:
	int inj_time; // cycle in which the packet was injected
	int src_node; // id of the source node
public:
	Packet(int inj_time, int src_node);
	virtual ~Packet() {} // Virtual destructor
	int get_inj_time();
};


#endif // PACKET_H
