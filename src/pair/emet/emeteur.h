//ce module va contenir les fonctionnalités relative à l'envoie d'un message 


#ifndef EMETEUR 
#define EMETEUR

/***********INCLUDES*************/

#include "../data_pair/data_peer.h"

/***************ENUMS******************/

/*****************STRUCTURES***********/

/****************FONCTIONS**************/

// DONE : get ip 
struct addrinfo * get_ip(const char * hote, const char * port, IP type_ip );

void start();
BOOL test_connexion(struct sockaddr_in6*);



//fonction called when we send a message 
void send_msg(unsigned int sock , struct sockaddr_in6 * adresse, const char * message);







#endif