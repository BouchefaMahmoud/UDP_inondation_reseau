

#ifndef HEADER
#define HEADER

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <fcntl.h>
#include "errno.h"



#define DATA_LIMIT_TLV4 230


// TODO:static ou pas ? à voir avec le prof 
/***************ENUMS******************/
typedef enum BOOL BOOL ;
enum BOOL{
    FALSE,TRUE
} ;

/**************************************/
/*
    Lorsqu’il reçoit un TLV Neighbour, le pair insère l’adresse contenue dans sa table de voisins potentiels
*/

static uint64_t _source_id = 0 ; 
static uint32_t _nonce = 0 ;



typedef struct tlv_entete
{
    uint8_t type;
    uint8_t length;
    
} tlv_entete;


typedef struct tlv
{
    tlv_entete entete ;
    char* tlv_msg ; 
    struct tlv * next ; 
     
 }tlv;

/******************FONCTIONS********************/

 uint64_t _getSource_id();
 void _setSource_id();
 char * smalloc(size_t);
    


#endif