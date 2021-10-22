#include "channel.hpp"


channel::channel(int xid){
    id=xid;
    num = Global_params::Global_params()->get_nchannels();
}

int Channel:: get_channel_Id(){
 return id;

}

void Channel:: set_channel_id(int nid){
this->id=nid;
}

int Channel:: get_nchannels(){
     return num;
}

int Channel::channel_function(string protocol,string step, int node, Packet packet,int channel_id){

    id=packet.get_channelID();
    if(id==''){
        id=channel_id;
        channel_busy[id]=true;
    }
    std::cout << "Protocol:" << protocol<< "Step:" << step<<"Node: "<<node<<"\n";





}
