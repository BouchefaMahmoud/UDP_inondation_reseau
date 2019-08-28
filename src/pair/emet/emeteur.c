
#include "emeteur.h"

/*
BOOL test_connexion(struct sockaddr_in6*  first_neighbour){
    
    BOOL connect = 
    while()
    
    return TRUE;
}

BOOL start(){

    struct addrinfo * first_info = NULL; 
    struct sockaddr_in6 * first_neighbour ;

    first_info = get_ip(HOTE, SERVER_PORT, IPV4_MAPPED);
    BOOL get_ip = FALSE  ;

    while(first_info != NULL){
       first_neighbour = (struct sockaddr_in6 *)first_info->ai_addr;
       if( test_connexion(first_neighbour) ){
         get_ip = TRUE;
           //TODO: le reste du programme
            return TRUE;

       } else {
           first_info = first_info->ai_next ; 
       }
    }
    if( ! get_ip){
        printf("connexion au serveur échoué, vérifiez votre connexion !");
    }
        return ;

}*/

/*
    récupère une liste d'ip
*/

struct addrinfo *get_ip(const char *hote, const char *port, IP type_ip)
{
    struct addrinfo *first_info;
    struct addrinfo hints;

    bzero(&hints, sizeof(struct addrinfo));

    hints.ai_flags = 0;
    
    if (type_ip == IPV4_MAPPED)
    {
        hints.ai_flags = AI_V4MAPPED | AI_ALL;
    }

    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_DGRAM;

    int r = getaddrinfo(hote, port, &hints, &first_info);
    if (r == 0)
    {
        return first_info;
    }

    return NULL;
}

/*
    Envoie un message 
*/
void send_msg(unsigned int sock, struct sockaddr_in6 *adresse, const char *message)
{
    size_t length = 0;
    memcpy(&length, message + 2, 2);
    char ip[40];

    int r = sendto(sock, message, ntohs(length) + 4, 0, (struct sockaddr_in *)adresse, sizeof(struct sockaddr_in6));
    if (r < 0)
    {
        perror("Erreur d'envoie ");
        printf("port: %d\n", ntohs(adresse->sin6_port));
        ipv6_to_str_unexpanded(ip, adresse->sin6_addr.s6_addr);
        printf("ip : %s\n",ip);
    }
}


// int main()
// {
//     char str[16]; 

//     int soc = socket(PF_INET6, SOCK_DGRAM, 0);

//     struct sockaddr_in6 *adresse;

//     if (soc < 0)
//     {
//         perror("creation de socket ");
//         exit(1);
//     }
//     uint64_t id = 12345678;

//     struct addrinfo *first_info = get_ip("jch.irif.fr", "1212", IPV4_MAPPED);

//     if (first_info->ai_addr != NULL)
//     {
//         puts("ok");
//         adresse = (struct sockaddr_in6 *)first_info->ai_addr;
//         printf("Num de port %d\n",ntohs(adresse->sin6_port));
//         ipv6_to_str_unexpanded(str, adresse->sin6_addr.s6_addr);
//         printf("ip : %s\n",str);


//         tlv *t = createTLV2(12345678, -1);
//         id = 0;
//         memcpy(&id, t->tlv_msg, 8);

//         printf("%ld\n", id);

//         char *message = init_message(t);

//         send_msg(soc, adresse, message);
//         puts("Message envoyé !");

//         receiver(soc, "::", PORT, PMTU);
//     }

//     close(soc);

//     return 0;
// }
