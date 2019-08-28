
#ifndef CREATE_MESSAGE
#define CREATE_MESSAGE

#include "tlvs.h"



//la construction de la liste des tlvs précede celle du message
tlv* init_tlv();
char * init_message( tlv *liste_tlvs);
tlv * create_tlv(uint8_t type , void * tlv_body);

uint16_t tlvs_length(tlv * liste);

// getter and setter pour source_id


#endif