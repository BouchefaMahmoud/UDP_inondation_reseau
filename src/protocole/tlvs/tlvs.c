
#include "create_message.h"
#include "tlvs.h"

void afficher_tlv(tlv *t)
{
    printf("Le type %d\n", t->entete.type);
    printf("Le length %d\n", t->entete.length);
    printf("Le body %s\n", t->tlv_msg);
}

tlv *createTLV0()
{
    tlv *t = NULL;
    t = create_tlv(0, NULL);
    return t;
}

tlv *createTLV1(uint8_t length)
{
    tlv *t = NULL;
    tlv_1 *t1 = smalloc(sizeof(tlv_1));
    t1->entete.type = 1;
    t1->entete.length = length;

    t1->mbz = smalloc(length);
    for (int i = 0; i < length; i++)
    {
        memcpy(t1->mbz + i, "0", 1);
    }

    t = create_tlv(1, t1);
    return t;
}

tlv *createTLV2(uint64_t source_id, uint64_t destination_id)
{
    tlv *t = NULL;
    tlv_2 *t2 = smalloc(sizeof(tlv_2));
    t2->entete.type = 2;
    // t2->source_id = smalloc(8);
    uint64_t *source = malloc(8);
    uint64_t *dest;
    *source = source_id;
    memcpy(&t2->source_id, source, 8);

    if (destination_id == -1)
    {
        t2->entete.length = 8;
    }
    else
    {
        t2->entete.length = 16;
        // t2->destination_id = smalloc(8);
        dest = malloc(8);
        *dest = destination_id;
        memcpy(&(t2->destination_id), dest, 8);
    }

    t = create_tlv(2, t2);

    return t;
}

tlv *createTLV3(unsigned char *ip, uint16_t port)
{
    tlv *t = NULL;
    tlv_3 *t3 = smalloc(sizeof(tlv_3));
    t3->entete.type = 3;
    t3->port = htons(port);
    uint8_t size = 0;
    //t3->ip = malloc(16);
    memcpy(t3->ip, ip, 16);
    // ip : 128bits donc 16 octets + port 2 octets
    t3->entete.length = 16 + 2;
    t = create_tlv(3, t3);
    return t;
}
void insert_fifo_tlv(tlv **liste, tlv *t)
{

    tlv *p = NULL;
    if (*liste == NULL)
    {
        *liste = t;
    }
    else
    {
        p = (*liste);
        while (p->next != NULL)
        {
            p = p->next;
        }
        p->next = t;
    }
}



tlv *createTLV4(uint64_t sender_id, uint32_t nonce, uint8_t type_data, char *data)
{
    tlv *t, *t2 = NULL;
    tlv_4 *t4 = malloc(sizeof(tlv_4));

    uint64_t *source = malloc(8);
    uint32_t *n = malloc(4);
    t4->entete.type = 4;
    uint8_t size = strlen(data);

    if (DATA_LIMIT_TLV4 < size)
    {
        size = DATA_LIMIT_TLV4;
    }

    char *msg = malloc(size);
    memcpy(msg, data, size);
    t4->entete.length = 13 + size;
    *source = sender_id;
    memcpy(&t4->sender_id, source, 8);
    *n = nonce;
    memcpy(&t4->nonce, n, 4);
    t4->type_data = type_data;
    t4->data = smalloc(size);
    memcpy(t4->data, msg, size);
    t = create_tlv(4, t4);

    return t;
}

tlv *createTLV5(uint64_t sender_id, uint32_t nonce)
{
    tlv *t = NULL;
    tlv_5 *t5 = malloc(sizeof(tlv_5));
    t5->entete.type = 5;
    // nonce = 4  senderid = 8
    t5->entete.length = 8 + 4;

    // t5->sender_id = smalloc(8);

    memcpy(&t5->sender_id, &sender_id, 8);
    // t5->nonce = smalloc(4);
    memcpy(&t5->nonce, &nonce, 4);

    t = create_tlv(5, t5);
    return t;
}

tlv *createTLV6(uint8_t code, char *message)
{
    tlv *t = NULL;
    tlv_6 *t6 = malloc(sizeof(tlv_6));
    t6->entete.type = 6;
    uint8_t size = 0;
    if (message != NULL)
        size = strlen(message);
    // code : 1
    t6->entete.length = size + 1;

    t6->code = code;
    if (message != NULL)
    {
        t6->message = smalloc(size);
        memcpy(t6->message, message, size);
    }
    // printf("on est au moins la ? \n");
    t = create_tlv(6, t6);
    return t;
}

tlv *createTLV7(char *message)
{
    tlv *t = NULL;
    tlv_7 *t7 = smalloc(sizeof(tlv_7));
    t7->entete.type = 7;
    uint8_t size = strlen(message);
    t7->entete.length = size;

    t7->message = smalloc(size);
    memcpy(t7->message, message, size);
    t = create_tlv(7, t7);
    return t;
}

//TODO: vérifier les erreurs
tlv *getTlvsFromMessage(char *message)
{

    uint8_t magic;
    uint8_t version;
    uint16_t taille;

    uint8_t type;
    uint8_t length;
    char *tlv_msg;

    // notre liste de tlvs
    tlv *liste = NULL;

    if (memcpy(&magic, message, 1) == NULL)
    {
        perror("copie dans magic");
        return NULL;
    }

    if (memcpy(&version, message + 1, 1) == NULL)
    {
        perror("copie dans version");
        return NULL;
    }

    if (magic != 93 || version != 2)
    {
        return NULL;
    }

    if (memcpy(&taille, message + 2, 2) == NULL)
    {
        perror("copie dans body-length");
        return NULL;
    }

    /*
        si la charge du datagramme est plus grande que le body + 4 alors ignore it !
    */
    if ((ntohs(taille) + 4) <= strlen(message))
    {
        return NULL;
    }

    message += 4;
    int size = 0;
    taille = ntohs(taille) - 2;
    while (size <= taille)
    {
        memcpy(&type, message + size, 1);
        size += 1;
        memcpy(&length, message + size, 1);

        size += 1;
        tlv_msg = smalloc(length);
        memcpy(tlv_msg, message + size, length);

        size += length;

        tlv *t = init_tlv();

        // remplissage de la tlv
        t->entete.length = length;
        t->entete.type = type;
        t->tlv_msg = smalloc(length);
        memcpy(t->tlv_msg, tlv_msg, length);

        // le chainage :
        t->next = liste;

        liste = t;
    }

    return liste;
}

tlv_2 *getTLV2(tlv *t)
{

    tlv_2 *t2 = NULL;
    t2 = malloc(sizeof(tlv_2));
    if (t2 == NULL)
    {
        perror("erreur d'allocation");
        return NULL;
    }
    t2->entete = t->entete;

    // t2->source_id = smalloc(8);
    memcpy(&t2->source_id, t->tlv_msg, 8);

    if (t->entete.length == 16)
    {
        // t2->destination_id = smalloc(8);
        memcpy(&t2->destination_id, t->tlv_msg + 8, 8);
    }
    return t2;
}

tlv_3 *getTLV3(tlv *t)
{
    tlv_3 *t3 = NULL;
    t3 = malloc(sizeof(tlv_3));
    if (t3 == NULL)
    {
        perror("erreur d'allocation");
        return NULL;
    }
    t3->entete = t->entete;
    memcpy(t3->ip, t->tlv_msg, 16);
    memcpy(&t3->port, t->tlv_msg + 16, 4);
    return t3;
}

void insert_fifo_tlv4(tlv_4 **liste, tlv_4 *t4)
{

    tlv_4 *p = NULL;
    if (*liste == NULL)
    {
        *liste = t4;
    }
    else
    {
        p = (*liste);
        while (p->next != NULL)
        {
            p = p->next;
        }
        p->next = t4;
    }
}

//TODO: vérifier
tlv_4 *getTLV4(tlv *t)
{
    tlv_4 *t4 = NULL, *liste = NULL;
    while (t != NULL)
    {

        t4 = malloc(sizeof(tlv_4));
        if (t4 == NULL)
        {
            perror("erreur d'allocation");
            return NULL;
        }

        t4->entete = t->entete;
        // t4->sender_id = malloc(8);
        // t4->nonce = malloc(4);
        memcpy(&t4->sender_id, t->tlv_msg, 8);
        memcpy(&t4->nonce, t->tlv_msg + 8, 4);
        memcpy(&t4->type_data, t->tlv_msg + 12, 1);
        t4->data = smalloc(t->entete.length - 13);
        memcpy(t4->data, t->tlv_msg + 13, t->entete.length - 13);
        t4->next =NULL;
        //add at the end :fifo
        insert_fifo_tlv4(&liste, t4);

        t = t->next;
    }

    return liste;
}

tlv_5 *getTLV5(tlv *t)
{
    tlv_5 *t5 = NULL;
    t5 = malloc(sizeof(tlv_5));
    if (t5 == NULL)
    {
        perror("erreur d'allocation");
        return NULL;
    }

    t5->entete = t->entete;
    // t5->sender_id = smalloc(8);
    // t5->nonce = smalloc(4);
    memcpy(&t5->sender_id, t->tlv_msg, 8);
    memcpy(&t5->nonce, t->tlv_msg + 8, 4);
    return t5;
}

tlv_6 *getTLV6(tlv *t)
{

    tlv_6 *t6 = NULL;
    t6 = malloc(sizeof(tlv_6));
    if (t6 == NULL)
    {
        perror("erreur d'allocation");
        return NULL;
    }

    t6->entete = t->entete;

    // printf("length %d \n", t->entete.length);

    memcpy(&t6->code, t->tlv_msg, 1);

    if (t->entete.length > 1)
    {
        t6->message = smalloc(t->entete.length - 1);
        memcpy(t6->message, t->tlv_msg + 1, t->entete.length - 1);
    }
    return t6;
}
