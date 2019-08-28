
#include "data_peer.h"

void ipv6_to_str_unexpanded(char *str, const unsigned char *addr)
{
    sprintf(str, "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
            (int)addr[0], (int)addr[1],
            (int)addr[2], (int)addr[3],
            (int)addr[4], (int)addr[5],
            (int)addr[6], (int)addr[7],
            (int)addr[8], (int)addr[9],
            (int)addr[10], (int)addr[11],
            (int)addr[12], (int)addr[13],
            (int)addr[14], (int)addr[15]);
}

neighbour *init_neighbour(contact *c, uint64_t id, time_t sent_at, time_t sent_long_at, BOOL sym, BOOL recent)
{

    neighbour *n = malloc(sizeof(neighbour));

    if (n == NULL)
    {
        perror("Allocation de la mémoire pour un voisin");
    }

    n->neighbour = init_contact(c->ip, c->port);
    n->id = id;
    n->contacted_at = 0;
    n->sent_at = sent_at;
    n->sent_long_at = sent_long_at;
    n->sym = sym;
    n->recent = recent;
    n->next = NULL;
    return n;
}

void add_neighbour(neighbour **liste, neighbour *n)
{

    if (*liste == NULL)
    {
        *liste = n;
    }
    else
    {
        n->next = *liste;
        *liste = n;
    }
}

void delete_neighbour(neighbour **liste, contact c)
{

    neighbour *p = NULL;
    if (*liste != NULL)
    {

        if ((*liste)->neighbour->port == c.port && strcmp((*liste)->neighbour->ip, c.ip) == 0)
        {
            p = *liste;
            (*liste) = (*liste)->next;
            free(p);
            return;
        }
        neighbour *l = *liste;
        p = l->next;
        while (p != NULL)
        {

            if (p->neighbour->port == c.port && strcmp(p->neighbour->ip, c.ip) == 0)
            {
                l->next = p->next;
                free(p);
                return;
            }
            l = l->next;
            p = p->next;
        }
    }
}

void update_neighbour(neighbour *liste, neighbour *new_neighbour)
{

    while (liste != NULL)
    {
        if (liste->neighbour->port == new_neighbour->neighbour->port && strcmp(liste->neighbour->ip, new_neighbour->neighbour->ip) == 0)
        {
            liste->sent_at = new_neighbour->sent_at;
            liste->sent_long_at = new_neighbour->sent_long_at;
            liste->id = new_neighbour->id;
            liste->sym = new_neighbour->sym;
            liste->recent = new_neighbour->recent;
            break;
        }
        liste = liste->next;
    }
}

neighbour *get_neighbour(neighbour *liste, contact c)
{

    while (liste != NULL)
    {
        if (liste->neighbour->port == c.port && strcmp(liste->neighbour->ip, c.ip) == 0)
        {
            return liste;
        }
        liste = liste->next;
    }
    return NULL;
}

neighbour *get_sym_neighbours(neighbour *voisins)
{
    neighbour *sym_neighbours = NULL;
    /*sym_neighbours = malloc(sizeof(neighbour));
    if (sym_neighbours == NULL)
    {
        perror("Allocation  de sym_neighbours ");
        return NULL;
    }*/

    while (voisins != NULL)
    {
        if (voisins->sym == TRUE)
        {
            add_neighbour(&sym_neighbours, voisins);
        }
        voisins = voisins->next;
    }

    return sym_neighbours;
}

neighbour *copy_neighbour(neighbour *voisins)
{
    neighbour *copie = NULL, *v = NULL;
    while (voisins != NULL)
    {
        v = init_neighbour(voisins->neighbour, voisins->id, voisins->sent_at, voisins->sent_long_at, voisins->sym, voisins->recent);
        if (v != NULL)
        {
            add_neighbour(&copie, v);
        }
        voisins = voisins->next;
    }
    return copie;
}

int count_neighbour(neighbour *liste)
{
    int count = 0;
    while (liste != NULL)
    {
        if (liste->sym == TRUE)
            count += 1;
        liste = liste->next;
    }
    return count;
}

void afficher(neighbour *liste)
{
    while (liste != NULL)
    {
        printf("IP : %s\n", liste->neighbour->ip);
        printf("PORT : %d\n", htons(liste->neighbour->port));
        printf("ID : %ld\n", liste->id);
        printf("SENT_AT : %ld\n", liste->sent_at);
        printf("SENT_LONG_AT : %ld\n", liste->sent_long_at);
        printf("sym : %d\n", liste->sym);
        printf("recent : %d\n", liste->recent);
        printf("/*************************/\n");
        puts("pas encore ?");
        liste = liste->next;
    }
}

data_neighbour *init_data_neighbour(neighbour *voisin)
{
    data_neighbour *dn = malloc(sizeof(data_neighbour));
    if (dn == NULL)
    {
        perror("Erreur d'allocation data_neighbour");
        return NULL;
    }
    unsigned int *compt = malloc(sizeof(unsigned int));
    *compt = 1;
    memcpy(&dn->compt, compt, sizeof(unsigned int));
    dn->voisin = voisin;
    dn->next = NULL;
    return dn;
}

void add_data_neighbour(data_neighbour **liste, data_neighbour *dn)
{

    if (*liste == NULL)
    {
        *liste = dn;
    }
    else
    {
        dn->next = *liste;
        *liste = dn;
    }
}

void delete_data_neighbour(data_neighbour **liste, contact c)
{
    data_neighbour *p = NULL;
    if (*liste != NULL)
    {

        if ((*liste)->voisin->neighbour->port == c.port && strcmp((*liste)->voisin->neighbour->ip, c.ip) == 0)
        {
            p = *liste;
            (*liste) = (*liste)->next;
            free(p);
        // puts("free");

        }
        return;
        puts("1");
        data_neighbour *l = *liste;
        p = l->next;
        while (p != NULL)
        {

            if (p->voisin->neighbour->port == c.port && strcmp(p->voisin->neighbour->ip, c.ip) == 0)
            {
                // puts("2");

                l->next = p->next;
                free(p);
                return;
            }
            l = l->next;
            p = p->next;
        }
    }
}

data_neighbour *get_data_neighbour(data_neighbour *liste, contact c)
{
    while (liste != NULL)
    {
        if (liste->voisin->neighbour->port == c.port && strcmp(liste->voisin->neighbour->ip, c.ip) == 0)
        {
            return liste;
        }
        liste = liste->next;
    }
    return NULL;
}

data_neighbour *get_data_neighbours_from_neighbours(neighbour *liste)
{

    data_neighbour *dn = NULL;
    while (liste != NULL)
    {
        data_neighbour *p = init_data_neighbour(liste);
        add_data_neighbour(&dn, p);
        liste = liste->next;
    }
    return dn;
}

void afficher_dn(data_neighbour *dn)
{
    while (dn != NULL)
    {
        puts("/********************/");
        printf("le nombre d'envoie %d\n", dn->compt);
        afficher(dn->voisin);
        dn = dn->next;
    }
}

contact *init_contact(const char *ip, uint16_t port)
{
    contact *c = malloc(sizeof(contact));

    if (c == NULL)
    {
        perror("Allocation de la mémoire pour un voisin");
        return NULL;
    }
    /*uint16_t * p = malloc(2);
    if( p == NULL) {
        perror("Alloc");
        return NULL;
    }*/

    // *p = port ;
    // memcpy(&c->port, p,2);
    c->next = NULL;
    c->port = port;
    memcpy(c->ip, ip, 16);

    return c;
}

void add_contact(contact **liste, contact *c)
{
    if (*liste == NULL)
    {
        *liste = c;
    }
    else
    {
        contact *l = *liste;

        while (l->next != NULL)
        {
            l = l->next;
        }
        l->next = c;
    }
}

void delete_contact(contact **liste, contact *c)
{
    contact *p = NULL;
    if (*liste != NULL)
    {

        if ((*liste)->port == c->port && strcmp((*liste)->ip, c->ip) == 0)
        {
            p = *liste;
            (*liste) = (*liste)->next;
            free(p);
            return;
        }

        contact *l = *liste;
        p = l->next;
        printf("est ce qu'on vient ici ?");
        while (p != NULL)
        {

            if (p->port == c->port && strcmp(p->ip, c->ip) == 0)
            {
                printf("est ce qu'on vient ici ?");
                l->next = p->next;
                free(p);
                return;
            }
            l = l->next;
            p = p->next;
        }
    }
}

BOOL exist_contact(contact *liste, contact *c)
{
    while (liste != NULL)
    {

        if (liste->port == c->port && strcmp(liste->ip, c->ip) == 0)
        {
            return TRUE;
        }
        liste = liste->next;
    }

    return FALSE;
}

//la ségmentation
data *init_data(char *content, uint64_t sender_id, uint32_t nonce, data_neighbour *voisins_sym)
{
    int taille_data = strlen(content);
    // on limite la taille d'une data à 230
    int nb_tlv = taille_data / DATA_LIMIT_TLV4;
    //TODO: on peut déterminer le nombre de tlv4 par message en déterminant le  TMPU, pour 1024 oct on peut pas envoyer plus de tlv4 à la fois
    if (nb_tlv > (PMTU / DATA_LIMIT_TLV4) ) 
    {
        perror("Le message est trop long");
        return NULL;
    }

    data *d = NULL, *liste = NULL;

    for (int i = 0; i < nb_tlv; i++)
    {
        d = malloc(sizeof(data));
        if (d == NULL)
        {
            perror("Allocation de la mémoire pour une data");
            return NULL;
        }

        d->content = NULL;
        d->content = malloc(DATA_LIMIT_TLV4);
        if (d->content == NULL)
        {
            perror("Allocation de la mémoire pour lecontent");
            return NULL;
        }
        memcpy(d->content, content + nb_tlv * DATA_LIMIT_TLV4, DATA_LIMIT_TLV4);
        // variable globale
        _nonce++;
        d->nonce = _nonce;
        d->sender_id = sender_id;
        d->received_at = time(0);
        d->voisins_sym = voisins_sym;
        // printf("compt = %d\n",voisins_sym->compt);
        d->next = NULL;
        add_data(&liste, d);
    }

    taille_data %= DATA_LIMIT_TLV4;

    d = malloc(sizeof(data));
    if (d == NULL)
    {
        perror("Allocation de la mémoire pour une data");
        return NULL;
    }

    d->content = NULL;
    d->content = malloc(taille_data);
    if (d->content == NULL)
    {
        perror("Allocation de la mémoire pour lecontent");
        return NULL;
    }
    memcpy(d->content, content, taille_data);

    d->sender_id = sender_id;
    d->received_at = time(0);
    d->voisins_sym = voisins_sym;
    d->next = NULL;
    if (nonce == -1)
    {
        _nonce++;
        d->nonce = _nonce;
        add_data(&liste, d);
    }
    else
    {
        d->nonce = nonce;
        return d ;
    }

    return liste;
}


//fifo
void add_data(data **liste, data *d)
{
    if (*liste == NULL)
    {
        *liste = d;
    }
    else
    {
        data *l = *liste;
        while (l->next != NULL)
        {
            l = l->next;
        }

        l->next = d;
    }
}

BOOL exist_data(data *liste, uint64_t sender_id, uint32_t nonce)
{

    while (liste != NULL)
    {
        if (liste->nonce == nonce && liste->sender_id == sender_id)
        {
            return TRUE;
        }
        liste = liste->next;
    }
    return FALSE;
}

void delete_data(data **liste, uint64_t sender_id, uint32_t nonce)
{
    data *p = NULL;
    if (*liste != NULL)
    {

        if ((*liste)->nonce == nonce && (*liste)->sender_id == sender_id)
        {
            p = *liste;
            (*liste) = (*liste)->next;
            free(p);
            return;
        }
        data *l = *liste;
        p = l->next;
        while (p != NULL)
        {
            if (p->nonce == nonce && p->sender_id == sender_id)
            {
                l->next = p->next;
                free(p);
                return;
            }
            l = l->next;
            p = p->next;
        }
    }
}

void afficher_contacts(contact *liste)
{

    char *str;

    while (liste != NULL)
    {
        printf("IP : %s\n", liste->ip);
        printf("PORT : %d\n", ntohs(liste->port));
        liste = liste->next;
    }
}

void afficher_data(data *liste)
{
    while (liste != NULL)
    {
        printf("CONTENT : %s\n", liste->content);
        printf("RECEIVE_AT : %ld\n", liste->received_at);
        printf("NONCE : %d\n", liste->nonce);
        liste = liste->next;
    }
}
