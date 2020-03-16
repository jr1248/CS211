#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
int count=0;
int fcount = 0;
int hits=0;
int miss=0;
int reads=0;
int writes=0;
int phits=0;
int pwrites=0;
int pmiss=0;
int preads=0;
typedef struct cache{
  size_t tag;
  size_t valid;
  size_t duration;
}cache;
void print_cache(int reads, int writes,int hits, int miss) {
  printf("no-prefetch\n");
  printf("Memory reads:%d\n",reads);
  printf("Memory writes:%d\n",writes);
  printf("Cache hits:%d\n", hits);
  printf("Cache misses:%d\n", miss);
}
void print_cache_prefetch(int preads, int pwrites,int phits, int pmiss) {
  printf("with-prefetch\n");
  printf("Memory reads:%d\n",preads);
  printf("Memory writes:%d\n",pwrites);
  printf("Cache hits:%d\n", phits);
  printf("Cache misses:%d\n",pmiss);
}

int check_hits(cache**cachewr,size_t tag, size_t associativity, size_t set_index){
  int is_hit=0;
  for(int i=0;i<associativity;i++){
    if(cachewr[set_index][i].tag==tag && cachewr[set_index][i].valid==1){
      is_hit=1;
      //return is_hit;
     // count++;
    //  cachewr[set_index][i].duration=count;
    }
  }
  return is_hit;
}
void replace(cache**cachewr, size_t tag,size_t associativity, size_t set_index){
  int initial=0;
  for (int j = 0; j < associativity; j++) {
    if(cachewr[set_index][j].duration<cachewr[set_index][initial].duration){
      initial=j;
    }
  }
  cachewr[set_index][initial].tag=tag;
  cachewr[set_index][initial].valid=1;
  count++;
  cachewr[set_index][initial].duration=count;
}
void write_cache(cache**cachewr, size_t tag,size_t associativity, size_t set_index){
  for(int i=0;i<associativity;i++){
    if(cachewr[set_index][i].valid==0){
      count++;
      cachewr[set_index][i].tag=tag;
      cachewr[set_index][i].valid=1;
      cachewr[set_index][i].duration=count;
	  return;
    }
  }
     replace(cachewr,tag,associativity,set_index);
}
void read(cache**cachewr, size_t tag,size_t associativity, size_t set_index){
  if(check_hits(cachewr,tag,associativity,set_index)==1){
    hits++;
  }
  if(check_hits(cachewr,tag,associativity,set_index)==0){
    miss++;
    reads++;
    write_cache(cachewr,tag,associativity,set_index);
  }
}
void write(cache**cachewr, size_t tag,size_t associativity, size_t set_index){
  if(check_hits(cachewr,tag,associativity,set_index)==1){
    hits++;
    writes++;
  }
  if(check_hits(cachewr,tag,associativity,set_index)==0){
    miss++;
    reads++;
    writes++;
    write_cache(cachewr,tag,associativity,set_index);
  }
}
void replaceP(cache**precache,size_t pretag,size_t associativity,size_t preindex){
  int initial=0;
  for (int j = 0; j < associativity; j++) {
    if(precache[preindex][j].duration<precache[preindex][initial].duration){
      initial=j;
    }
  }
  precache[preindex][initial].tag=pretag;
  precache[preindex][initial].valid=1;
  fcount++;
  precache[preindex][initial].duration=fcount;
}
void write_pcache(cache**precache,size_t pretag,size_t associativity,size_t preindex){
  for(int i=0;i<associativity;i++){
    if(precache[preindex][i].valid==0){
      fcount++;
      precache[preindex][i].tag=pretag;
      precache[preindex][i].valid=1;
      precache[preindex][i].duration=fcount;
    return;
    }
  }
  replaceP(precache,pretag,associativity,preindex);
}
void prefetch(cache**precache, size_t presize, size_t preadd, size_t associativity,size_t adjblock,size_t block_offset, size_t sets){
    size_t prefetch_address=preadd;
  for (int i = 0; i < presize; i++){
    size_t set_index_bits=log2(sets);
    size_t block_offset_bits=log2(block_offset);
    size_t mask = (1 << set_index_bits) - 1;
    prefetch_address=prefetch_address+block_offset;
    size_t preindex = (prefetch_address >> block_offset_bits) & mask;
    size_t pretag = prefetch_address >> (block_offset_bits + set_index_bits);
    if(check_hits(precache,pretag,associativity,preindex)==0){
      write_pcache(precache,pretag,associativity,preindex);
      preads++;
      printf("here\n");
    }
  }
}
void pread(cache**precache, size_t pretag,size_t associativity, size_t preindex){
  if(check_hits(precache,pretag,associativity,preindex)==1){
    phits++;
  }
  if(check_hits(precache,pretag,associativity,preindex)==0){
    pmiss++;
    preads++;
    write_pcache(precache,pretag,associativity,preindex);
  }
}
void pwrite(cache**precache, size_t pretag,size_t associativity, size_t preindex){
  if(check_hits(precache,pretag,associativity,preindex)==1){
    phits++;
    pwrites++;
  }
  if(check_hits(precache,pretag,associativity,preindex)==0){
    pmiss++;
    preads++;
    pwrites++;
    write_pcache(precache,pretag,associativity,preindex);
  }
}
int main(int argc, char** argv) {
  size_t associativity;
  size_t sets;
  size_t block_offset=atoi(argv[2]);
  size_t address;
  char cache_type;

  FILE *fp= fopen(argv[6], "r");
  if(fp==NULL){
    return 0;
  }
  if(strcmp(argv[4], "direct")==0){
    associativity = 1;
    sets=atoi(argv[1])/atoi(argv[2]);
  }
  if ((strcmp(argv[4], "assoc")==0)) {
    associativity = atoi(argv[1])/atoi(argv[2]);
    sets = 1;
  }
  if(argv[4][5] == ':'){
    associativity = atoi(&argv[4][6]);
    if((associativity & (associativity - 1))== 0){
      sets=atoi(argv[1])/(atoi(argv[2])*associativity);
    }
  }

  cache** cachewr=malloc(sets*sizeof(cache*));

 for (int i = 0; i < sets; i++) {
     *(cachewr+i)= malloc(associativity*sizeof(cache));
  }
  for (int i = 0; i < sets; i++) {
    for (int j = 0; j < associativity; j++) {
      cachewr[i][j].tag=0;
      cachewr[i][j].valid=0;
      cachewr[i][j].duration=0;
    }
  }
  cache** precache=malloc(sets*sizeof(cache*));

  for (int i = 0; i < sets; i++) {
     *(precache+i)= malloc(associativity*sizeof(cache));
  }
  for (int i = 0; i < sets; i++) {
    for (int j = 0; j < associativity; j++) {
      precache[i][j].tag=0;
      precache[i][j].valid=0;
      precache[i][j].duration=0;
    }
  }

  size_t block_offset_bits=log2(block_offset);
  size_t set_index_bits=log2(sets);
  size_t mask = (1 << set_index_bits) - 1;
  size_t presize=atoi(argv[5]);
  while(fscanf(fp,"%c %lx\n", &cache_type, &address)==2){
    size_t index = (address >> block_offset_bits) & mask;
    size_t tag = address >> (block_offset_bits + set_index_bits);
    size_t prefetch_address=address+atoi(argv[2]);
    if(strcmp(argv[3], "fifo")==0){
      if(cache_type=='W'){
        write(cachewr,tag,associativity,index);
        size_t preindex = (prefetch_address >> block_offset_bits) & mask;
        size_t pretag = prefetch_address >> (block_offset_bits + set_index_bits);
        pwrite(precache,pretag,associativity,preindex);
        prefetch(precache,presize,address,associativity,atoi(argv[2]),block_offset,sets);
      }
        if (cache_type=='R') {
          read(cachewr,tag,associativity,index);
          size_t preindex = (prefetch_address >> block_offset_bits) & mask;
          size_t pretag = prefetch_address >> (block_offset_bits + set_index_bits);
          //size_t prefetch_address=address+atoi(argv[2]);
          pread(precache,pretag,associativity,preindex);
          prefetch(precache,presize,address,associativity,atoi(argv[2]),block_offset,sets);
        }
      }
  }
  print_cache(reads,writes,hits,miss);
  //dump_cache(cachewr,sets,associativity);
  //dump_cache(precache,sets,associativity);
  print_cache_prefetch(preads,pwrites,phits,pmiss);
  //dump_cache(cachewr,sets,associativity);
  //dump_cache(precache,sets,associativity);
  return 0;
}
