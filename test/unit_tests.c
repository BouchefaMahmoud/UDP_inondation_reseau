#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <check.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>

#include "../src/protocole/tlvs/tlvs.h"
#include "../src/protocole/tlvs/create_message.h"
#include "../src/protocole/header/header.h"

//fonction qui affiche et qui retourne une ipv6 en chaine de car
void ipv6_to_str_unexpanded(char *str, const struct in6_addr *addr)
{
  sprintf(str, "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
          (int)addr->s6_addr[0], (int)addr->s6_addr[1],
          (int)addr->s6_addr[2], (int)addr->s6_addr[3],
          (int)addr->s6_addr[4], (int)addr->s6_addr[5],
          (int)addr->s6_addr[6], (int)addr->s6_addr[7],
          (int)addr->s6_addr[8], (int)addr->s6_addr[9],
          (int)addr->s6_addr[10], (int)addr->s6_addr[11],
          (int)addr->s6_addr[12], (int)addr->s6_addr[13],
          (int)addr->s6_addr[14], (int)addr->s6_addr[15]);
}

START_TEST(tlv0)
{
  tlv *t = createTLV0();
  printf("bodylength == %d\n", t->entete.length);
  char *message = init_message(t);
  uint8_t magic;
  uint8_t version;
  uint16_t body_length;
  uint8_t type;

  memcpy(&magic, message, 1);
  memcpy(&version, message + 1, 1);
  memcpy(&body_length, message + 2, 2);
  memcpy(&type, message + 4, 1);

  ck_assert_int_eq(magic, 93);
  ck_assert_int_eq(version, 2);
  body_length = ntohs(body_length);
  ck_assert_int_eq(body_length, 1);
  ck_assert_int_eq(type, 0);
  free(message);
}
END_TEST

START_TEST(tlv1)
{

  tlv *t = createTLV1(15);
  printf("bodylength == %d\n", t->entete.length);
  char *message = init_message(t);
  uint8_t magic;
  uint8_t version;
  uint16_t body_length;
  uint8_t type;
  uint8_t length;

  memcpy(&magic, message, 1);
  memcpy(&version, message + 1, 1);
  memcpy(&body_length, message + 2, 2);
  memcpy(&type, message + 4, 1);
  memcpy(&length, message + 5, 1);

  char *mbz = malloc(body_length);
  memcpy(mbz, message + 6, 15);

  ck_assert_int_eq(magic, 93);
  ck_assert_int_eq(version, 2);
  body_length = ntohs(body_length);
  ck_assert_int_eq(body_length, 17);
  ck_assert_int_eq(type, 1);
  ck_assert_int_eq(length, 15);
  ck_assert_str_eq(mbz, "000000000000000");

  free(mbz);
  free(message);
}
END_TEST

START_TEST(tlv2)
{

  tlv *t = createTLV2("00000000", "11111111");

  char *mess = init_message(t);

  uint8_t magic;
  uint8_t version;
  uint16_t body_length;
  uint8_t type;
  uint8_t length;

  char *ids = malloc(8);
  char *des = malloc(8);

  memcpy(&magic, mess, 1);
  memcpy(&version, mess + 1, 1);
  memcpy(&body_length, mess + 2, 2);
  memcpy(&type, mess + 4, 1);
  memcpy(&length, mess + 5, 1);
  memcpy(ids, mess + 6, 8);
  memcpy(des, mess + 14, 8);

  ck_assert_int_eq(magic, 93);
  ck_assert_int_eq(version, 2);
  body_length = ntohs(body_length);
  ck_assert_int_eq(body_length, 18);
  ck_assert_int_eq(type, 2);
  ck_assert_int_eq(length, 16);

  free(ids);
  free(des);
  free(mess);
}
END_TEST

START_TEST(tlv3)
{

  uint8_t magic;
  uint8_t version;
  uint16_t body_length;
  uint8_t type;
  uint8_t length;
  unsigned char ip[16];
  uint16_t port;

  struct addrinfo *first_info;
  struct addrinfo hints;
  struct sockaddr_in6 *adress;

  bzero(&hints, sizeof(struct addrinfo));
  //hints.ai_flags = AI_V4MAPPED | AI_ALL;
  hints.ai_family = AF_INET6;
  hints.ai_socktype = SOCK_DGRAM;
  char *mess3 = NULL;
  int r = getaddrinfo("jch.irif.fr", "1212", &hints, &first_info);
  if (r == 0)
  {
    if (first_info != NULL)
    {
      //récuperation de l'ipv4 -mapped
      adress = (struct sockaddr_in6 *)first_info->ai_addr;

      // char *str = malloc(16);
      tlv *t = createTLV3(adress->sin6_addr.s6_addr, 1212);

      mess3 = init_message(t);

      memcpy(&magic, mess3, 1);
      memcpy(&version, mess3 + 1, 1);
      memcpy(&body_length, mess3 + 2, 2);
      memcpy(&type, mess3 + 4, 1);
      memcpy(&length, mess3 + 5, 1);
      memcpy(ip, mess3 + 6, 16);
      memcpy(&port, mess3 + 6 + 16, 2);

      //tests
      ck_assert_int_eq(magic, 93);
      ck_assert_int_eq(version, 2);
      body_length = ntohs(body_length);
      ck_assert_int_eq(body_length, 20);
      ck_assert_int_eq(type, 3);
      ck_assert_int_eq(length, 18);
      ck_assert_str_eq(ip, adress->sin6_addr.s6_addr);
      port = ntohs(port);
      ck_assert_int_eq(port, 1212);
    }
  }

   free(mess3);
 }
 END_TEST
/*
START_TEST(tlv4)
{

  char *data = "mahmoudbouchefacoucou";
  int size = strlen(data);
  uint64_t ids = 12345678 ;
  uint32_t nonce =1234 ;
  tlv *t = createTLV4(ids, nonce, 0, data);

 
  char *mess4 = init_message(t);
  uint8_t magic;
  uint8_t version;
  uint16_t body_length;
  uint8_t type;
  uint8_t length;
  uint64_t ids ;
  uint32_t nonce ;
  uint8_t type_data;
  char *data_sended = malloc(size);
  memcpy(&magic, mess4, 1);
  printf("magic system : %d\n", magic);
  memcpy(&version, mess4 + 1, 1);
  memcpy(&body_length, mess4 + 2, 2);
  memcpy(&type, mess4 + 4, 1);
  memcpy(&length, mess4 + 5, 1);
  memcpy(&ids, mess4 + 6, 8);
  memcpy(&nonce, mess4 + 14, 4);
  memcpy(&type_data, mess4 + 18, 1);
  memcpy(data_sended, mess4 + 19, size);

  ck_assert_int_eq(magic, 93);
  ck_assert_int_eq(version, 2);
  body_length = ntohs(body_length);
  ck_assert_int_eq(body_length, size + 15);
  ck_assert_int_eq(type, 4);
  ck_assert_int_eq(length, size + 13);
  ck_assert_int_eq(ids, 12345678);
  ck_assert_int_eq(nonce, 1234);
  ck_assert_int_eq(type_data, 0);
  ck_assert_str_eq(data_sended, data);



}
END_TEST

START_TEST(tlv5)
{

  tlv *t = createTLV5("11111111", "1234");
  printf("bodylength == %d\n", t->entete.length);
  char *mess5 = init_message(t);
  uint8_t magic;
  uint8_t version;
  uint16_t body_length;
  uint8_t type;
  uint8_t length;
  char *ids = malloc(8);
  char *nonce = malloc(4);

  memcpy(&magic, mess5, 1);
  printf("magic system : %d\n", magic);
  memcpy(&version, mess5 + 1, 1);
  memcpy(&body_length, mess5 + 2, 2);
  memcpy(&type, mess5 + 4, 1);
  memcpy(&length, mess5 + 5, 1);
  memcpy(ids, mess5 + 6, 8);
  memcpy(nonce, mess5 + 14, 4);

  ck_assert_int_eq(magic, 93);
  ck_assert_int_eq(version, 2);
  body_length = ntohs(body_length);
  ck_assert_int_eq(body_length, 14);
  ck_assert_int_eq(type, 5);
  ck_assert_int_eq(length, 12);
  ck_assert_str_eq(ids, "11111111");
  ck_assert_str_eq(nonce, "1234");
  free(mess5);
  free(ids);
  free(nonce);
}
END_TEST

START_TEST(tlv6)
{
  tlv *t = createTLV6(0, "Hello");
  uint8_t magic;
  uint8_t version;
  uint16_t body_length;
  uint8_t type;
  uint8_t length;
  uint8_t code;
  char *s = NULL;

  char *mess6 = init_message(t);

  memcpy(&magic, mess6, 1);
  memcpy(&version, mess6 + 1, 1);
  memcpy(&body_length, mess6 + 2, 2);
  memcpy(&type, mess6 + 4, 1);
  memcpy(&length, mess6 + 5, 1);

  s = malloc(length - 1);
  memcpy(&code, mess6 + 6, 1);
  memcpy(s, mess6 + 7, length - 1);

  ck_assert_int_eq(magic, 93);
  ck_assert_int_eq(version, 2);
  body_length = ntohs(body_length);
  ck_assert_int_eq(body_length, 8);
  ck_assert_int_eq(type, 6);
  ck_assert_int_eq(code, 0);
  ck_assert_int_eq(length, 6);
  ck_assert_str_eq(s, "Hello");
}
END_TEST

*/
START_TEST(get_t2)
{

  tlv *t = createTLV2("00000000", "11111111");
  char *mess = init_message(t);

  t = getTlvsFromMessage(mess);

  tlv_2 *t2 = getTLV2(t);

  ck_assert_str_eq(t2->source_id, "00000000");
  ck_assert_str_eq(t2->destination_id, "11111111");
}
END_TEST

START_TEST(get_t3)
{

  tlv *t = createTLV3("123456789012345", 1212);

  char *mess = init_message(t);

  t = getTlvsFromMessage(mess);

  tlv_3 *t3 = getTLV3(t);

  ck_assert_str_eq(t3->ip, "123456789012345");
  ck_assert_int_eq(ntohs(t3->port), 1212);
}
END_TEST

START_TEST(get_t4)
{

  char *data = "mahmoud:coucou";

  tlv *t = createTLV4("00000000", "1234", 0, data);

  char *mess = init_message(t);

  t = getTlvsFromMessage(mess);

  tlv_4 *t4 = getTLV4(t);

  ck_assert_str_eq(t4->sender_id, "00000000");
  ck_assert_str_eq(t4->nonce, "1234");
  ck_assert_int_eq(t4->type_data, 0);
  ck_assert_str_eq(t4->data, data);

}
END_TEST

START_TEST(get_t5)
{

  tlv *t = createTLV5("00000000", "1234");

  char *mess = init_message(t);

  t = getTlvsFromMessage(mess);

  tlv_5 *t5 = getTLV5(t);

  ck_assert_str_eq(t5->sender_id, "00000000");
  ck_assert_str_eq(t5->nonce, "1234");
}
END_TEST





START_TEST(tlv4){
 char *data = "mahmoudbouchefacoucou";
  int size = strlen(data);
  uint64_t ids = 12345678 ;
  uint32_t nonce =1234 ;
  tlv *t = createTLV4(ids, nonce, 0, data);

  puts(" on est la  ?");
 
  printf("bodylength == %d\n", t->entete.length);
  char *mess4 = init_message(t);
  uint8_t magic;
  uint8_t version;
  uint16_t body_length;
  uint8_t type;
  uint8_t length;

  uint8_t type_data;
  char *data_sended = malloc(size);
  memcpy(&magic, mess4, 1);
  printf("magic system : %d\n", magic);
  memcpy(&version, mess4 + 1, 1);
  memcpy(&body_length, mess4 + 2, 2);
  memcpy(&type, mess4 + 4, 1);
  memcpy(&length, mess4 + 5, 1);
  memcpy(&ids, mess4 + 6, 8);
  memcpy(&nonce, mess4 + 14, 4);
  memcpy(&type_data, mess4 + 18, 1);
  memcpy(data_sended, mess4 + 19, size);

  ck_assert_int_eq(magic, 93);
  ck_assert_int_eq(version, 2);
  body_length = ntohs(body_length);
  ck_assert_int_eq(body_length, size + 15);
  ck_assert_int_eq(type, 4);
  ck_assert_int_eq(length, size + 13);
  ck_assert_int_eq(ids, 12345678);
  ck_assert_int_eq(nonce, 1234);
  ck_assert_int_eq(type_data, 0);
  ck_assert_str_eq(data_sended, data);

}END_TEST;

int main()
{

  TCase *tc_tlvs, *get_tlvs;
  Suite *s;
  SRunner *sr;
   tc_tlvs = tcase_create("tc_tlvs");
  tcase_add_test(tc_tlvs, tlv0);
   tcase_add_test(tc_tlvs, tlv1);
   tcase_add_test(tc_tlvs, tlv2);
   tcase_add_test(tc_tlvs, tlv3);
  tcase_add_test(tc_tlvs, tlv4);

 
 get_tlvs = tcase_create("get_tlvs");
   tcase_add_test(get_tlvs, get_t2);
  tcase_add_test(get_tlvs, get_t3);
 tcase_add_test(get_tlvs, get_t4);
 tcase_add_test(get_tlvs, get_t5);


  s = suite_create("tlvs");

  suite_add_tcase(s, tc_tlvs);

 suite_add_tcase(s, get_tlvs);

  sr = srunner_create(s);

  srunner_run_all(sr, CK_VERBOSE);

  printf("%d\n", srunner_ntests_failed(sr) == 0 ? 0 : 1);

  return 0;
}
