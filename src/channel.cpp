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
