//ce module va contenir les fonctionnalités relative à la réception d'un message

#ifndef RECEPTEUR
#define RECEPTEUR

/***********INCLUDES*************/

#include "data_peer.h"




/***************ENUMS******************/


/*****************STRUCTURES***********/



/****************FONCTIONS**************/


//receiver 

void receiver(int sock, unsigned char * ip, uint16_t port, uint16_t pmtu); 




#endif