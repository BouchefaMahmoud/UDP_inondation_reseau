#include "pair/emet/emeteur.h"


int main()
{
    unsigned *str = NULL;
    int sock = socket(PF_INET6, SOCK_DGRAM, 0);
    if( sock < 0){
        exit(-1);
    }

    // conversion en socket polymorphe  
    int val = 0 ;
    int rc = setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, &val, sizeof(val)); 
    if(rc < 0){
        perror("conversion en socket polymorphe");
        exit(-1);
    }   
    // convertir en socket non-bloquante 
    int r = fcntl(sock, F_SETFL, O_NONBLOCK);
    if(r < 0){
        close(sock);
        exit(-1);
    }

    uint16_t port;
    struct addrinfo *first_info =NULL,* free_addrinfo = NULL;
    struct sockaddr_in6 *first_neighbour = NULL;
    struct sockaddr_in6 addr;
    addr.sin6_family = AF_INET6;

    memset(&addr, 0, sizeof(addr));
    addr.sin6_family = AF_INET6;

    /*********************/
    struct sockaddr_in6 adr_recv;
    memset(&adr_recv, 0, sizeof(adr_recv));
    adr_recv.sin6_family = AF_INET6;
    adr_recv.sin6_port = htons(PORT);
    inet_pton(AF_INET6, "::", &adr_recv.sin6_addr);
    r = bind(sock, (struct sockaddr *)&adr_recv, sizeof(struct sockaddr_in6));
    if(r < 0){ 
        perror("cannont bind");
        close(sock);
        exit(-1);
    }
    /***************************/

    char *mess = NULL;

    tlv *t = NULL;
    tlv_2 *t2 = NULL;
    tlv_3 *t3 = NULL;
    tlv_4 *t4 = NULL;
    tlv_5 *t5 = NULL;
    tlv_6 *t6 = NULL;
    tlv_7 *t7 = NULL;

    fd_set rdfs;
    int fd = 0;

    struct timeval tv;
    int ret = 0;
    tv.tv_sec = 1;
    

    char buff[MAX];
    int l = 0;
    struct sockaddr_in6 address_emet;
    socklen_t a = sizeof(address_emet);


    contact *juliusz, *c = NULL;
    neighbour *voisin = NULL, *p = NULL;
    data_neighbour *data_voisin = NULL;
    neighbour *voisins_sym = NULL;
    data_neighbour *dn = NULL;

    time_t temps_actu = 0;

    int taille = 0;
    data *d = NULL;
    data *contents = NULL;

    while (1)
    {
        tv.tv_sec = 1;
        tv.tv_usec = 500000;

        FD_ZERO(&rdfs);

        FD_SET(fd, &rdfs);
        FD_SET(sock, &rdfs);

        ret = select(8, &rdfs, NULL, NULL, &tv);
        /**
         * Premier contact avec le serveur  
         */
        if (_voisins == NULL)
        {
            first_info = get_ip(HOTE, SERVER_PORT, IPV4_MAPPED);
            free_addrinfo = first_info ;
            while (first_info != NULL)
            {
                first_neighbour = (struct sockaddr_in6 *)first_info->ai_addr;
                if (first_neighbour != NULL)
                {
                    t = createTLV2(_getSource_id(), -1);
                    mess = init_message(t);
                    send_msg(sock, first_neighbour, mess);
                }
                first_info = first_info->ai_next;
            }
            //libérer la mémoire 
            freeaddrinfo(free_addrinfo);
        }

        if (ret > 0)
        {
            if (FD_ISSET(sock, &rdfs))
            {

                l = recvfrom(sock, buff, MAX - 1, 0, (struct sockaddr *)&address_emet, &a);
                if(l < 0){
                    perror("Erreur de recvfrom");
                }else if (l > 0)
                {
                buff[l] = '\0';
                    t = NULL;
                    t = getTlvsFromMessage(buff);
                    c = init_contact(address_emet.sin6_addr.s6_addr, address_emet.sin6_port);

                    if (t != NULL)
                    {
                        // printf("type %d\n",t->entete.type);
                        switch (t->entete.type)
                        {
                        case 2:
                            temps_actu = time(0);
                            t2 = getTLV2(t);

                            // s'il envoie un hello long pour la première fois
                            if (t->entete.length == 16)
                            {
                                // si le champ id dest est égale au notre
                                if (_getSource_id() == t2->destination_id)
                                {
                                    voisin = init_neighbour(c, t2->source_id, temps_actu, temps_actu, TRUE, TRUE);
                                }
                                else
                                {
                                    // ce n'est pas un voisin sym mais c'est un voisin quand même
                                    voisin = init_neighbour(c, t2->source_id, temps_actu, temps_actu, FALSE, TRUE);
                                }

                                // s'il envoie un hello long pour la première fois
                                if (get_neighbour(_voisins, *c) == NULL)
                                {
                                    puts("Connected");
                                    puts("Write 'close' to exit ...");
                                    add_neighbour(&_voisins, voisin);
                                }
                                else
                                {
                                    // mise à jour du voisin
                                    update_neighbour(_voisins, voisin);
                                }
                            }
                            else
                            {
                                voisin = init_neighbour(c, t2->source_id, temps_actu, 0, FALSE, TRUE);
                                // s'il envoie un hello court pour la première fois
                                // c: le contact (ip,port)
                                if (get_neighbour(_voisins, *c) == NULL)
                                {
                                    // puts(" hello court pour la première fois ");
                                    add_neighbour(&_voisins, voisin);
                                }
                                else
                                {
                                    // mise a jour du voisin
                                    update_neighbour(_voisins, voisin);
                                }
                            }

                            break;
                        case 3:
                            // ajouter un voisin potentiel à la liste
                            t3 = getTLV3(t);
                            c = init_contact(address_emet.sin6_addr.s6_addr, address_emet.sin6_port);
                            add_contact(&_voisins_potentiels, c);
                            // la variable est écrasée mais pas libérée
                            if (count_neighbour(_voisins) < 8)
                            {
                                t = NULL;
                                t = createTLV2(_getSource_id(), -1);
                                mess = init_message(t);
                                addr.sin6_port = t3->port;
                                memcpy(&addr.sin6_addr, t3->ip, 16);
                                send_msg(sock, &addr, mess);
                            }

                            break;
                        case 4:
                            t4 = getTLV4(t);
                            /* si c'est un ami sym, et que la data est déjà dans la liste, 
                                on considère ça comme un acquittement et on le supprime de la lise des voisins à inonder pour cette data    
                            */
                            if (exist_data(_contents, t4->sender_id, t4->nonce))
                            {

                                //l'emet nous aqcuitte, supprimer le voisins de la liste de voisin sym de la data correspondante
                                d = _contents;
                                while (d != NULL)
                                {
                                    if (d->sender_id == t4->sender_id && d->nonce == t4->nonce)
                                    {
                                        delete_data_neighbour(&d->voisins_sym, *c);
                                    }
                                    d = d->next;
                                }
                            }
                            else
                            {
                                /**
                                 * d'abord on affiche la data, ensuite on l'ajoute à la liste des data à inonder, ensuite 
                                */

                                voisin = copy_neighbour(_voisins);
                                if (voisin != NULL)
                                    //supprimer l'emeteur de la liste de l'inondation
                                    delete_neighbour(&voisin, *c);

                                dn = NULL;
                                dn = get_data_neighbours_from_neighbours(voisin);
                                //parcourir la liste de tlv4
                                puts("************************");
                                while (t4 != NULL)
                                {
                                    printf("%s\n", t4->data);
                                    d = init_data(t4->data, t4->sender_id, t4->nonce, dn);
                                    add_data(&_contents, d);

                                    // Ensuite acquitter l'émeteur
                                    t = createTLV5(t4->sender_id, t4->nonce);
                                    mess = init_message(t);
                                    send_msg(sock, &address_emet, mess);
                                    t4 = t4->next;
                                }
                                puts("*************************");
                            }

                            break;
                        case 5:
                            t5 = getTLV5(t);

                            d = _contents;
                            while (d != NULL)
                            {
                                if (d->sender_id == t5->sender_id && d->nonce == t5->nonce)
                                {
                                    delete_data_neighbour(&d->voisins_sym, *c);
                                }
                                d = d->next;
                            }

                            break;
                        case 6:
                            t6 = getTLV6(t);
                            printf("Message d'erreur: %s\n", t6->message);

                            break;
                        case 7:
                            printf("\n%s", t->tlv_msg);
                            break;
                        default:
                            continue;
                            break;
                        }
                    }
                }

                l = 0;
            }
            if (FD_ISSET(fd, &rdfs))
            {
                taille = read(fd, buff, MAX - 1);
                // go out
                buff[taille] = '\0';
                if(strcmp(buff,"close\n") == 0 ){
                    goto sortir ;
                }
                //on récupère le nonce depuis la variable globale, on met -1 pour différencier les data crée en locale et celles reçues
                d = init_data(buff, _getSource_id(), -1, get_data_neighbours_from_neighbours(_voisins));
                add_data(&_contents, d);
            }
        }

        //inondation
        contents = _contents;

        while (contents != NULL)
        {
            // on récupère la liste des voisins d'une data
            data_voisin = contents->voisins_sym;

            temps_actu = time(0);

            // supprimer la data au bout de 300 sec/5 min ou si la liste est vide
            if (((temps_actu - contents->received_at) > 300) || (data_voisin == NULL))
            {
                delete_data(&_contents, contents->sender_id, contents->nonce);
                goto cont;
            }
            t = createTLV4(contents->sender_id, contents->nonce, 0, contents->content);
            //  printf("Taille Text : %d\n",strlen(contents->content));
            mess = init_message(t);

            // parcourir la liste des voisins. Rq : il s'agit de struct data_neighbour *, qui est composée de voisin (neighbour *) et le compt
            while (data_voisin != NULL)
            {
                // récuperer le voisin
                voisin = data_voisin->voisin;
                // puts("how many time we were here ?");
                // supression du voisins de la liste data_neighbours, à tester
                if (data_voisin->compt > 5)
                {
                    // puts("are we here  ?");
                    // printf("nonce : %d \n", contents->nonce);
                    t = createTLV6(2, "je n'ai pas été acquitté depuis longtemps");
                    mess = init_message(t);

                    addr.sin6_port = voisin->neighbour->port;
                    puts("avant");
                    str = smalloc(50);
                    puts("après");
                    ipv6_to_str_unexpanded(str, voisin->neighbour->ip);
                    inet_pton(AF_INET6, str, &addr.sin6_addr);
                    // printf("ipv6 : %s\n", str);

                    send_msg(sock, &addr, mess);
                     delete_data_neighbour(&contents->voisins_sym, *voisin->neighbour);
                    goto next_data_neighbour;
                }
                // si le voisin est sym
                if ((data_voisin->voisin->sym == TRUE) &&
                    (contents->received_at + ((int)pow(2, data_voisin->compt - 1)) <= temps_actu) &&
                    (contents->received_at + ((int)pow(2, data_voisin->compt)) >= temps_actu))
                {
                    // puts("we never been here  ?");
                    addr.sin6_port = voisin->neighbour->port;
                    puts("izan");    
                    str = smalloc(50);
                    puts("izan");
                    ipv6_to_str_unexpanded(str, voisin->neighbour->ip);

                    inet_pton(AF_INET6, str, &addr.sin6_addr);

                    send_msg(sock, &addr, mess);
                    data_voisin->compt++;
                }
            next_data_neighbour:
                data_voisin = data_voisin->next;
            }
        cont:
            contents = contents->next;
        }

        voisin = _voisins;
        while (voisin != NULL)
        {

            time_t current_time = time(0);

            if ((current_time - voisin->contacted_at) > 30)
            {

                t = createTLV2(_getSource_id(), voisin->id);
                mess = init_message(t);
                addr.sin6_port = voisin->neighbour->port;
                puts("avant v");
                str = smalloc(50);
                puts("après v");
                ipv6_to_str_unexpanded(str, voisin->neighbour->ip);
                inet_pton(AF_INET6, str, &addr.sin6_addr);
                // printf("ipv6 : %s\n", str);

                send_msg(sock, &addr, mess);
                // mise à jour du temps
                voisin->contacted_at = current_time;
            }
            voisin = voisin->next;
        }
    }

    sortir:
    // libérer toute la mémoire allouée 
        close(sock);
      
    return 0;
}
