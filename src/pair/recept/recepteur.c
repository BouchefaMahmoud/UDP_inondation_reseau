#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <time.h>




#include "recepteur.h"
#include "data_peer.h"






void receiver(int sock, unsigned char *ip, uint16_t port, uint16_t pmtu)
{
    printf("on vient ici au moins ?");

    struct sockaddr_in6 adresse;
    struct sockaddr_in6 *adr;
    //int soc2 = socket(PF_INET6, SOCK_DGRAM, 0);
    /*if(soc2 <0 ){
        perror("erreur");
    }*/
    memset(&adresse, 0, sizeof(adresse));

    adresse.sin6_family = AF_INET6;
    adresse.sin6_port = htons(port);

    inet_pton(AF_INET6, ip, &adresse.sin6_addr);

    int r = bind(sock, (struct sockaddr *)&adresse, sizeof(struct sockaddr_in6));

    char buff[pmtu];
    int l = 0;
    struct sockaddr_in6 address_emet;
    socklen_t a = sizeof(address_emet);
    tlv_2 *t2, *t2_long;
    tlv_4 *t4;
    tlv *t;
    tlv_5 *t5 ; 
    char *mess = NULL;
    char *data = "Mahmoud:ça marche :)";
    uint64_t id;
    size_t size = 0;
    struct addrinfo *first_info;
    char str[16];
    while (1)
    {
        l = recvfrom(sock, buff, pmtu - 1, 0, (struct sockaddr *)&address_emet, &a);
        buff[l] = '\0';
            ipv6_to_str_unexpanded(str,address_emet.sin6_addr.s6_addr );
            printf("l'ip de l'emeteur : %s\n ",str);
        if (l > 0)
        {
            printf("message recu ! \n");
            // c'est la que la magie va opérer ;)
            tlv *t = getTlvsFromMessage(buff);

            memcpy(&id, t->tlv_msg + 8, 8);
            printf("son id est %ld\n", id);
            printf("le type %d \n", t->entete.type);
            if (t != NULL)
            {
                switch (t->entete.type)
                {
                case 2:
                    puts("hey coucou");
                    t2 = getTLV2(t);
                    printf("type = %d\n", t2->entete.type);
                    printf("length = %d\n", t2->entete.length);
                    printf("source id  = %ld\n", t2->source_id);
                    printf("desitinataire id  = %ld\n", t2->destination_id);

                    t = createTLV2(12345678, t2->source_id);
                    mess = init_message(t);
                    first_info = get_ip("jch.irif.fr", "1212", IPV4_MAPPED);
                    if (first_info->ai_addr != NULL)
                    {
                        adr = (struct sockaddr_in6 *)first_info->ai_addr;
                        send_msg(sock, adr, mess, 22);
                        puts("Message envoyé type 2 long!");
                    }

                    t = createTLV4(12345678, 1234, 0, data);
                    mess = init_message(t);
                    size = tlvs_length(t);
                    printf("le message vaut %d \n ", size);

                    puts("on arrive ici ?");
                    first_info = get_ip("jch.irif.fr", "1212", IPV4_MAPPED);
                    if (first_info->ai_addr != NULL)
                    {
                        adr = (struct sockaddr_in6 *)first_info->ai_addr;
                        send_msg(sock, adr, mess, size + 4);
                        puts("Message envoyé de type 4!");
                    }

                    break;
                case 3:

                    t = createTLV4(12345678, 1234, 0, data);
                    mess = init_message(t);
                    size = tlvs_length(t);
                    printf("le message vaut %d \n ", size);
                    puts("on arrive ici ?");

                    first_info = get_ip("jch.irif.fr", "1212", IPV4_MAPPED);
                    if (first_info->ai_addr != NULL)
                    {
                        adr = (struct sockaddr_in6 *)first_info->ai_addr;
                        send_msg(sock, adr, mess, size + 4);
                        puts("Message envoyé de type 4!");
                    }
                    break;
                        case 5 : 
                            t5 = getTLV5(t);
                            printf("Message de tlv5 : \n");
                            printf("Sender_id : %ld\n",t5->sender_id);
                            printf("Nonce : %d\n",t5->nonce);


                        break ;
                default:
                    break;
                }
            }
        }

        l = 0;
    }
}

// int main()
// {
//     puts("Ok");
//       neighbour *liste = NULL;
//     neighbour *n = NULL;
//     neighbour * voisin = NULL;
//     for (int i = 0; i != 3; i++)
//     {
//         contact * c = init_contact("12345678912345",htons(i) );
//         n = intit_neighbour(c, "12345678", 12345678, 123458, FALSE, TRUE);
//         add_neighbour(&liste, n);
//     }

//     voisin = liste;    
//      afficher(voisin);
//    /*contact * c = init_contact( "12345678912345",2);
//     puts("suppression");
//     delete_neighbour(&liste, *c);
//     afficher(liste);
//     contact * c2 = init_contact( "12345678912345",0);
//     neighbour *update = intit_neighbour(c2, "00000000", 123000, 135455, FALSE, TRUE);

//     // afficher(update);

//     puts("modification 0");
//     update_neighbour(liste, update);
//     afficher(liste);
// /*
//    data_neighbour *dn = get_data_neighbours_from_neighbours(liste);
//     puts("Affichages des dn :");
//     afficher_dn(dn);
//     contact c3 = init_contcat(0, "12345678912345");
//     puts("*************Ajout d'une dn**************");
//     neighbour *v = get_neighbour(liste, c3);
//     //v->neighbour.port = 2;
//     data_neighbour *d = init_data_neighbour(v);
//     //delete_data_neighbour(&dn, c3);

//     //add_data_neighbour(&dn,d);
//     afficher_dn(dn);

//     contact * contacts = NULL; 
//     for(int i = 0 ; i != 2 ; i++ ){
//         contact * c  = init_contact("123456789012345",i);
//         add_contact(&contacts, c);
//     }

//     contact * c =  init_contact("123456789012345",2);
//     add_contact(&contacts, c);
//     afficher_contacts(contacts);
//     printf(" existe : %d\n",exist_contact(contacts, c) );
//     delete_contact(&contacts, c);
//     puts("apres supression");
//    // afficher_contacts(contacts);


//     data * contents = NULL;
//     for (int i = 0; i != 2; i++)
//     {
//         data *d= init_data("Hello",12345678,i,liste);
//         add_data(&contents, d);
//     }

//    // afficher_data(contents);
//     data * d = init_data("Bonjour",12345678,2, liste);
//     add_data(&contents, d);
//     delete_data(&contents, 12345678,1);
//     puts("Après la supression");
//     printf("existe ? : %s\n",  exit_data(contents,12345678,1) ?  "TRUE":"FALSE");

// */

//     return 0;
// }

