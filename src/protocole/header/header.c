
#include "header.h"


 uint64_t _getSource_id(){
    if( _source_id == 0 ){
       _setSource_id();  
    }
    return _source_id;
}

 void _setSource_id(){
    if(_source_id == 0){
    struct timeval tv;
    gettimeofday(&tv,NULL);
    _source_id= tv.tv_sec*(uint64_t)1000000+tv.tv_usec;
    }
    
}



char * smalloc(size_t size){
   char * s = malloc(size);
   if(s == NULL) {
      perror("Echec d'allocation de mémoire\n");
      exit(1);
   }
   return  s;
}
