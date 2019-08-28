
#ifndef TLVS
#define TLVS


#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../header/header.h"

//Mettre une queue avec la tete pour faire polymorphisme
typedef struct in6_addr in6_addr;
typedef struct tlv_1
{
    struct tlv_entete entete;
    char *  mbz;
} tlv_1;


typedef struct tlv_2
{
    struct tlv_entete entete;
      uint64_t source_id;
      uint64_t  destination_id;
} tlv_2;

/*
on peut utiliser cette structure in6_addr pour stocker directement une ipv6
struct in6_addr {
    unsigned char   s6_addr[16];    IPv6 address 
};
*/

typedef struct tlv_3
{
    struct tlv_entete entete;
    unsigned char ip[16];
    uint16_t port;
} tlv_3;

// sender_id== source_id ?
typedef struct tlv_4
{
    struct tlv_entete entete;
    uint64_t sender_id;
    uint32_t nonce ;
    uint8_t type_data;
    char * data;
    struct tlv4 * next; 
} tlv_4;

typedef struct tlv_5
{
    struct tlv_entete entete;
    uint64_t sender_id;
    uint32_t nonce;

} tlv_5;

typedef struct tlv_6
{
    struct tlv_entete entete;
    uint8_t code;
    char * message;
    
} tlv_6;

typedef struct tlv_7
{
    struct tlv_entete entete;
    char * message;    
} tlv_7;


void afficher_tlv(tlv * t);
tlv * createTLV0();
tlv * createTLV1(uint8_t length);
tlv * createTLV2(uint64_t source_id, uint64_t destination_id);
tlv * createTLV3(unsigned char *  ip, uint16_t port);
tlv * createTLV4(uint64_t sender_id,uint32_t nonce, uint8_t type_data, char* data);
tlv * createTLV5(uint64_t sender_id,uint32_t nonce);
tlv * createTLV6(uint8_t code, char *message);
tlv * createTLV7( char *message);



/*
    les tlv0,1,7 ne diffèrent pas de la tlv 
*/
tlv_2 * getTLV2(tlv * t);    
tlv_3 * getTLV3(tlv * t);    
tlv_4 * getTLV4(tlv * t);    
void insert_fifo_tlv4(tlv_4 ** , tlv_4*);
tlv_5 * getTLV5(tlv * t);    
tlv_6 * getTLV6(tlv * t);    


tlv * getTlvsFromMessage(char * message);
void insert_fifo_tlv(tlv ** , tlv*);


#endif
