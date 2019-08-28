//ce module va contenir les fonctionnalités relative à la réception d'un message

#ifndef DATA_PEER
#define DATA_PEER

#define HOTE "jch.irif.fr"
#define SERVER_PORT "1212"
#define CONNECT_PORT 1212
#define PORT 5000
#define MAX 4096
#define PMTU 1024

/***********INCLUDES*************/



#include "../../protocole/tlvs/tlvs.h"
#include "../../protocole/tlvs/create_message.h"

/***************ENUMS******************/
typedef enum IP IP;
enum IP
{
	 
    IPV6,
    IPV4_MAPPED,
    IPV4	
};

/*****************STRUCTURES***********/

/*
    Lorsqu’il reçoit un TLV Neighbour, le pair insère l’adresse contenue dans sa table de voisins_sym potentiels
*/
typedef struct contact
{
    uint16_t port;
    unsigned char ip[16];
    struct contact *next;

} contact;

typedef struct neighbour
{
    contact * neighbour;
    uint64_t id;
    time_t contacted_at;
    //hello cour/long
    time_t sent_at;
    time_t sent_long_at;
    //if neighbour sent a long hello in less than 2 min
    BOOL sym;
    //if neighbour sent a long or short hello in less than 2 min
    BOOL recent;
    struct neighbour *next;
} neighbour;

/**
 *Structure qui sert à stocker un voisin et le nombre de fois qu'il a envoyé d'une data,
 *Pour avoir une meilleure performance, on attribut le nombre d'envoie de data à la structure voisins_sym,
 *pour éviter d'allouer de la mémoire de chaque voisin qui apparait dans la liste d'une autre data, 
 *alors on stocke l'adresse de chaque voisin et le nombre d'envoie dans une structure à part .  
 */
typedef struct data_neighbour
{
    neighbour *voisin;
    unsigned int compt;
    neighbour *next;
} data_neighbour;

/**
 * cette structure va contenir l'ensemble des data, elle identifié par un nonce unique, de plus le champs received_at
 * nous permet de vérifier depuis combien de temps on a stocké la donnée 
 * l'ajout se fera à la fin de la liste comme une file  
*/
typedef struct data
{

    char *content;
    //au bout de 5 min la donnée sera supprimée
    time_t received_at;
    time_t sended_at;
    uint32_t nonce;
    uint64_t sender_id ;
    data_neighbour * voisins_sym;
    struct data *next;

} data;

/************GLOBAL VARIABLES*************/


static neighbour *_voisins =NULL;

static contact *_voisins_potentiels =NULL;

static data *  _contents = NULL;

/****************FONCTIONS**************/
void ipv6_to_str_unexpanded(char *str, const unsigned char  *addr);
data_neighbour *init_data_neighbour(neighbour *voisin);
void add_data_neighbour(data_neighbour **, data_neighbour *);
void delete_data_neighbour(data_neighbour **, contact);
data_neighbour *get_data_neighbour(data_neighbour *, contact);
data_neighbour *get_data_neighbours_from_neighbours(neighbour *);
BOOL exit_data_neighbour(data_neighbour *);

neighbour *init_neighbour(contact* c, uint64_t id, time_t sent_at, time_t sent_at_long, BOOL sym, BOOL recent);
void add_neighbour(neighbour **, neighbour *);
void delete_neighbour(neighbour **, contact);
void update_neighbour(neighbour *, neighbour *);
neighbour *get_neighbour(neighbour *, contact);
int count_neighbour(neighbour *liste);
neighbour * copy_neighbour(neighbour *);
//TODO: à tester
neighbour * get_sym_neighbours(neighbour*);
// BOOL exist_neighbour(neighbour * , contact c);

contact * init_contact(const char * ip, uint16_t port );
void add_contact(contact **, contact *);
void delete_contact(contact **, contact *);
BOOL exist_contact(contact *, contact *);   


// à tester
data * init_data(char *content,uint64_t,uint32_t, data_neighbour *voisins_sym);
//file
void add_data(data **, data *);
void delete_data(data **,  uint64_t sender_id, uint32_t nonce);
//TODO: à tester
BOOL exist_data( data *, uint64_t sender_id,  uint32_t nonce );


//only for developer tests
void afficher_dn(data_neighbour *dn);
void afficher(neighbour *liste);
void afficher_contacts(contact * liste);
void afficher_data(data *);

#endif
