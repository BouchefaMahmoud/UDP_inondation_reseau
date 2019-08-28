
#include "tlvs.h"
#include "create_message.h"


/*
char * smalloc(size_t size){
   char * s = malloc(size);
   if(s == NULL) {
      perror("Echec d'allocation de mémoire\n");
      exit(1);
   }
   return  s;
}
*/



tlv *init_tlv()
{

    tlv *t = smalloc(sizeof(tlv));

    if (t == NULL)
    {
        perror("erreur d'initialisation de la tlv\n");
        exit(1);
    }
    return t;
}

//Le type de retour de la fonction ?
tlv *create_tlv(uint8_t type, void *tlv_body)
{

    tlv *t = init_tlv();
    (t->entete).type = type;
    (t->entete).length = 0;
    t->next = NULL;
    uint8_t size = 0;
    tlv_1 t1;
    tlv_2 t2;
    tlv_3 t3;
    tlv_4 t4;
    tlv_5 t5;
    tlv_6 t6;
    tlv_7 t7;
    uint64_t * source_id ;
    switch (type)
    {
    case 1:

        t1 = *((tlv_1 *)tlv_body);
        size = t1.entete.length;
        t->entete.length = t1.entete.length;
        t->tlv_msg = smalloc(size);
        memcpy(t->tlv_msg, t1.mbz, size);

        break;

    case 2:

        t2 = *((tlv_2 *)tlv_body);

        t->entete = t2.entete ;
        size = t2.entete.length;
        // printf("size %d \n",size);
        t->tlv_msg = malloc(size);
        
        memcpy(t->tlv_msg,  &t2.source_id , 8);
        
        if (size == 16)
        {
            memcpy(t->tlv_msg + 8, &t2.destination_id, 8);
        }

        break;

    case 3:
        t3 = *((tlv_3 *)tlv_body);

        (t->entete).length = t3.entete.length;
        size = t3.entete.length;
        t->tlv_msg = smalloc(size);

        //ip.s6_addr est unsigned char de 16 octets
            memcpy(t->tlv_msg, t3.ip , 16);
            memcpy(t->tlv_msg + 16, &t3.port, 2);
            
     
        break;
    case 4:

        t4 = *((tlv_4 *)tlv_body);

        t->entete = t4.entete;

        size = t4.entete.length;
        t->tlv_msg = malloc(size +13 );
        // sender_id = 8 nonce = 4  typedata =4 data : undefined

        memcpy(t->tlv_msg, &t4.sender_id, 8);
        memcpy(t->tlv_msg + 8, & t4.nonce, 4);
        memcpy(t->tlv_msg + 12, &t4.type_data, 1);
    
        memcpy(t->tlv_msg + 13,t4.data, size);
        break;

    case 5:

        t5 = *((tlv_5 *)tlv_body);
        (t->entete).length = t5.entete.length;
        t->tlv_msg = smalloc(8 + 4);
        memcpy(t->tlv_msg, &t5.sender_id, 8);
        memcpy(t->tlv_msg + 8, &t5.nonce, 4);

        break;

    case 6:

        t6 = *((tlv_6 *)tlv_body);
        (t->entete).length = t6.entete.length;
        size = t6.entete.length;
        // printf("size dans create tlv6 :%d\n", size);
        t->tlv_msg = smalloc(size);
        memcpy(t->tlv_msg, &t6.code, 1);
        memcpy(t->tlv_msg + 1, t6.message, t->entete.length -1);
        // puts("on arrive pas jusqui'ici ? haha");
        break;

    case 7:

        t7 = *((tlv_7 *)tlv_body);
        (t->entete).length = t7.entete.length;
        size = t7.entete.length;
        t->tlv_msg = smalloc(size);
        memcpy(t->tlv_msg, t7.message, size);
        break;

    default:
        perror(" ce type n'existe pas ");
        if (type != 0)
            return NULL;
        break;
    }
    return t;
}

uint16_t tlvs_length(tlv *liste)
{

    uint16_t somme = 0;
    
    while (liste != NULL)
    {
        somme += liste->entete.length;
        somme +=  1;
        // la tlv de type 0 contient 1 octet
        if( liste->entete.type != 0)
            somme +=  1;

        liste = liste->next;
    }
    return somme;
}

char *init_message(tlv *liste_tlvs)
{

    uint8_t ui = 2;
    uint16_t taille_tlvs = tlvs_length(liste_tlvs);
    char *  message  =  NULL;
    //magic=1 version=1 body_length=2   
    message = smalloc(taille_tlvs + 4);
    if( message ==  NULL){
        perror("erreur de allocation\n");
        exit(1);
    }

    char *magic = smalloc(1);
    char *version = smalloc(1);
    int *bodylength = smalloc(ui);
    
    *magic =  93;
    *version = 2;
    *bodylength = taille_tlvs;
    memcpy(message , magic, 1);
    memcpy(message + 1, version, 1);
    *bodylength = htons(taille_tlvs);
    memcpy(message + 2,  bodylength, ui);
    
    int size = 2 + ui;
    // char * str = "BonjourBonjourBonjourBonjour";
    //memcpy(message, str, strlen(str));

    while (liste_tlvs != NULL)
    {
        memcpy(message + size, &(liste_tlvs->entete).type, 1);
        size += 1;
        memcpy(message + size, &(liste_tlvs->entete).length, 1);
        size += 1;
        memcpy(message + size, liste_tlvs->tlv_msg, liste_tlvs->entete.length) ;
        size += liste_tlvs->entete.length;
        // mettre à jour le body-length
        liste_tlvs = liste_tlvs->next;
    }   
    return message;
}
