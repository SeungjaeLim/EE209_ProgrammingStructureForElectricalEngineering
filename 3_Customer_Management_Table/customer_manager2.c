/* Seungjae Lim / Assignment3 / customer_manager2.c */
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "customer_manager.h"



#define UNIT_ARRAY_SIZE 1024
/*start hash table size*/
enum {HASH_MULTIPLIER = 65599};
/*constant for hashing*/

/*hash function*/
static int hash_function(const char *pcKey, int iBucketCount);

/* extand table d size twice */
void Table_Expansion(DB_T d);

/*
struct UserInfo
  Node of doubly linked list 
  for each hash table's id-list and name-list about UserInfo
*/
struct UserInfo {
  char *name;                // customer name
  char *id;                  // customer id
  int purchase;              // purchase amount (> 0)
  struct UserInfo* next_id;  //next node id-list 
  struct UserInfo* prev_id;  //prev node id-list 
  struct UserInfo* next_name;//next node name-list 
  struct UserInfo* prev_name;//prev node name-list 
};

struct DB {
  struct UserInfo **hashtable_name;   // hash table about name
  struct UserInfo **hashtable_id;     // hash table about id
  int numBuckets;  // current hash table size (max # of elements)
  int numItems;    // # of stored items, needed to determine
			             // # whether the array should be expanded
			             // # or not
};

/*struct UserInfo* define to UserInfo_T*/
typedef struct UserInfo* UserInfo_T;

/*--------------------------------------------------------------------*/
/*
function Table_Expansion
  Expand each hash table twice
parameter
  DB_T d - current DB
return
  void
*/
void Table_Expansion(DB_T d)
{
  /*check NULL pointer*/
  if(d == NULL)
  {
    return;
  }
  /*create temporary new DB*/
  DB_T new_d;
  new_d = (DB_T)calloc(1,sizeof(struct DB));
  new_d->numBuckets = 2*(d->numBuckets);
  new_d->numItems = d->numItems;
  /*check calloc'ed success*/
  new_d->hashtable_id = (struct UserInfo **)calloc(new_d->numBuckets,
               sizeof(UserInfo_T));
  if (new_d->hashtable_id == NULL) {
    /*Error occur*/
    fprintf(stderr, "Can't allocate a memory for array of size %d\n",
	    new_d->numBuckets);   
    /*free allocated data*/
    new_d->numBuckets = 0;
    new_d->numItems = 0;
    free(new_d);
    new_d = NULL;
    /*Initialize to 0 to freed data*/
    return;
  }
  /*check calloc'ed success*/
  new_d->hashtable_name = (struct UserInfo **)calloc(new_d->numBuckets,
               sizeof(UserInfo_T));
  if (new_d->hashtable_name == NULL) {
    /*Error occur*/
    fprintf(stderr, "Can't allocate a memory for array of size %d\n",
	    new_d->numBuckets);
    /*free allocated data*/   
    if(new_d->hashtable_id != NULL)
    {
      free(new_d->hashtable_id);
      /*Initialize to 0 to freed data*/
      new_d->hashtable_id = NULL;
    }
    new_d->numBuckets = 0;
    new_d->numItems = 0;
    free(new_d);
    new_d = NULL;
    /*Initialize to 0 to freed data*/
    return;
  }
  UserInfo_T p; /*Node of current customer*/
  UserInfo_T nextp; /*Node of next customer*/
  int b; /*hash table index*/
  /*search for every customer*/
  for(b = 0; b < d->numBuckets; b++)
  {
    for(p = d->hashtable_id[b]; p != NULL; p = nextp)
    {
      nextp = p->next_id;
      /*create customer data in extanded hash table*/
      RegisterCustomer(new_d, p->id, p->name, p->purchase);
      /*free customer data in before hash table*/
      free(p->id);
      free(p->name);
      /*Initialize to 0 to freed data*/
      p->id = NULL;
      p->name = NULL;
      p->next_id = NULL;
      p->next_name = NULL;
      p->prev_id = NULL;
      p->prev_name = NULL;
      p->purchase = 0;
      free(p);
    }
    /*reset data in before hash table*/
    d->hashtable_name[b] = NULL;
    d->hashtable_id[b] = NULL;
    /*Initialize to 0 to freed data*/
  }
  /*free before hash table*/
  free(d->hashtable_id);
  d->hashtable_id = NULL;
  free(d->hashtable_name);
  d->hashtable_name = NULL;
  /*Initialize to 0 to freed data*/
  d->numBuckets = 0;
  d->numItems = 0;
  /*update DB to new DB*/
  d->hashtable_id = new_d -> hashtable_id;
  d->hashtable_name = new_d -> hashtable_name;
  d->numBuckets = new_d -> numBuckets;
  d->numItems = new_d -> numItems;
  /*we should change content because call-by-value*/
  free(new_d);
  /*free temporary DB to prevent memory leak*/
  return;
}
/*--------------------------------------------------------------------*/
/*
function hash_function
  Return a hash code for pcKey that is between 0 and iBucketCount-1,
  inclusive
parameter
  const char *pcKey - hashing string
  int iBucketCount - # of Bucket
return
  int - hash code   
*/
static int hash_function(const char *pcKey, int iBucketCount)
{
  int i; /*index of string*/
  unsigned int uiHash = 0U; /*initialize hashcode*/
  /*hashing*/
  for (i = 0; pcKey[i] != '\0'; i++)
    uiHash = uiHash * (unsigned int)HASH_MULTIPLIER
             + (unsigned int)pcKey[i];
  /*return hashcode*/
  return (int)(uiHash % (unsigned int)iBucketCount);
}

/*--------------------------------------------------------------------*/
/*
function CreateCustomerDB
  create new DB
parameter
  void
return
  DB_T - pointer to DB
*/
DB_T
CreateCustomerDB(void)
{
  DB_T d;
  /*pointing DB*/
  d = (DB_T)calloc(1,sizeof(struct DB));
  /*allocate DB*/
  if (d == NULL) {
    /*allocating error occur*/
    fprintf(stderr, "Can't allocate a memory for DB_T\n");
    return NULL;
  }
  /*initialize DB*/
  d->numBuckets = UNIT_ARRAY_SIZE; // start with 1024 elements
  /*allocate hashtable for id*/
  d->hashtable_id = (struct UserInfo **)calloc(d->numBuckets,
               sizeof(UserInfo_T));
  if (d->hashtable_id == NULL) {
    /*allocating error occur*/
    fprintf(stderr, "Can't allocate a memory for array of size %d\n",
	    d->numBuckets);
    /*free allocated data and initialize to 0*/   
    d->numBuckets = 0;
    d->numItems = 0;
    free(d);
    d = NULL;
    return NULL;
  }
  /*allocate hashtable for name*/
  d->hashtable_name = (struct UserInfo **)calloc(d->numBuckets,
               sizeof(UserInfo_T));
  if (d->hashtable_name == NULL) {
    /*allocating error occur*/
    fprintf(stderr, "Can't allocate a memory for array of size %d\n",
	    d->numBuckets);
    /*free allocated data and initialize to 0*/    
    if(d->hashtable_id != NULL)
    {
      free(d->hashtable_id);
      d->hashtable_id = NULL;
    }
    d->numBuckets = 0;
    d->numItems = 0;
    free(d);
    d = NULL;
    return NULL;
  }
  /*return created DB's pointer*/
  return d;
}
/*--------------------------------------------------------------------*/
/*
function DestroyCustomerDB
  destroy DB
parameter
  DB_T DB of customer
return
  void
*/
void
DestroyCustomerDB(DB_T d)
{
  UserInfo_T p; /*Node for current data*/
  UserInfo_T nextp; /*Node next data*/
  int b; /*index of hashtable*/
  if(d == NULL)
  {
    /*check NULL pointer*/
    return;
  }
  /*check hashtable was created*/
  if(d->hashtable_id != NULL)
  {
    /*search every data of customer by ID*/
    for(b = 0; b < d->numBuckets; b++)
    {
      for(p = d->hashtable_id[b]; p != NULL; p = nextp)
      {
        /*keep search next data*/
        nextp = p -> next_id;
        /*free allocated data*/
        free(p->id);
        free(p->name);
        /*initialize to 0*/
        p->id = NULL;
        p->name = NULL;
        p->next_id = NULL;
        p->next_name = NULL;
        p->prev_id = NULL;
        p->prev_name = NULL;
        p->purchase = 0;
        free(p);
      }
      /*initialize to 0*/
      d->hashtable_name[b] = NULL;
      d->hashtable_id[b] = NULL;
    }
    /*free hashtable and initailze to 0*/
    free(d->hashtable_id);
    d->hashtable_id = NULL;
  }
  /*check hashtable was created*/
  if(d->hashtable_name != NULL)
  {
    /*free hashtable and initailze to 0*/
    free(d->hashtable_name);
    d->hashtable_name = NULL;
  }
  d->numBuckets = 0;
  d->numItems = 0;
  /*free DB*/
  free(d);
  d = NULL;
  return;
}
/*--------------------------------------------------------------------*/
/*
function RegisterCustomer
  Register New Node of customer
parameter
  DB_T d - DB
  const char *id - customer id
	const char *name - customer name
  const int purchase - customer's purchase
return
  int - 0 for success, -1 for fail
*/
int
RegisterCustomer(DB_T d, const char *id,
		 const char *name, const int purchase)
{
  /*check NULL pointer or missed data*/
  if(d == NULL || id == NULL || name == NULL)
  {
    return -1;
  }
  /*purchase must greater than 0*/
  if(purchase<=0)
  {
    return -1;
  }
  char *idp, *namep;
  /*pointer of owner key*/
  UserInfo_T customer = (UserInfo_T)calloc(1,
                        sizeof(struct UserInfo));
  /*allocate customer Node*/
  idp = strdup(id);
  /*make own id for data*/
  namep = strdup(name);
  /*make own name for data*/
  customer->id = idp;
  customer->name = namep;
  customer->purchase = purchase;
  /*initialize customer node*/

  /*check reduplicating name and id*/
  UserInfo_T p_tmp;
  /*Node for searching*/
  int h_name = hash_function(namep,d->numBuckets);
  /*hashing with name*/
  int h_id = hash_function(idp,d->numBuckets);
  /*hashing with id*/
  /*search reduplicating name*/
  for(p_tmp = d->hashtable_name[h_name]; 
      p_tmp != NULL; 
      p_tmp = p_tmp->next_name)
  {
    if(strcmp(p_tmp->name, name) == 0)
    {
      /*reduplicating name -> fail*/
      return -1;
    }
  }
  /*search reduplicating id*/
  for(p_tmp = d->hashtable_id[h_id];
      p_tmp != NULL; 
      p_tmp = p_tmp -> next_id)
  {
    if(strcmp(p_tmp->id, id) == 0)
    {
      /*reduplicating id -> fail*/
      return -1;
    }
  }
  /*hash table reaches 75% of the number of buckets, 
    expand the hash table to double*/
  if(d->numItems >= (d->numBuckets) * 3 / 4 
    && 1048576 > d->numBuckets)
  {
    /*expand table*/
    Table_Expansion(d);
    /*change hashcode because # of Bucket is changed */
    h_name = hash_function(namep,d->numBuckets);
    h_id = hash_function(idp,d->numBuckets);
  }
  /*Link customer with recent Node with id*/
  if(d->hashtable_id[h_id] != NULL)
  {
    d->hashtable_id[h_id]->prev_id = customer;
  }
  /*Link customer with recent Node with name*/
  if(d->hashtable_name[h_name] != NULL)
  {
    d->hashtable_name[h_name]->prev_name = customer;
  }
  /*Link customer with recent Node with name*/
  customer->next_name = d->hashtable_name[h_name];
  customer->prev_name = NULL;
  /*Link customer with recent Node with id*/
  customer->next_id = d->hashtable_id[h_id];
  customer->prev_id = NULL;
  /*assign new node to hash table*/
  d->hashtable_name[h_name] = customer;
  d->hashtable_id[h_id] = customer;
  d->numItems++;
  /*success register*/
  return 0;
}
/*--------------------------------------------------------------------*/
/*
function UnregisterCustomerByID
  Unregister node with id
parameter
  DB_T d - DB
  const char *id - customer id
return
  int - 0 for success, -1 for fail
*/
int
UnregisterCustomerByID(DB_T d, const char *id)
{
  /*check missed data*/
  if(d == NULL || id == NULL)
  {
    return -1;
  }
  UserInfo_T p, nextp_id;
  /*Node for searching*/
  int h_id = hash_function(id,d->numBuckets);
  /*get id's hashcode*/
  /*seach hashtable of id*/
  if(d->hashtable_id[h_id] != NULL)
  {
    /*check head of list*/
    if(strcmp(d->hashtable_id[h_id]->id, id) == 0)
    {
      /*matched*/
      p = d->hashtable_id[h_id];
      d->hashtable_id[h_id] = d->hashtable_id[h_id]->next_id;
      /*make hashtable assign next node*/
      if(d->hashtable_id[h_id] != NULL)
      {
        /*is there new head*/
        d->hashtable_id[h_id]->prev_id = NULL;
        /*updated node is head*/
      }
      if(p->prev_name != NULL)
      {
        /*if Node isn't head of name's list*/
        p->prev_name->next_name = p->next_name;
        /*update node's list of name*/
      }
      else 
      {
        /*if Node is head of name's list*/
        int h_name = hash_function(p->name, d->numBuckets);
        d->hashtable_name[h_name] = p->next_name;
        /*update hashtable of name*/
      }
      if(p->next_name != NULL)
      {
        p->next_name->prev_name = p->prev_name;
        /*update node's list of name*/
      }
      /*free allocated node*/
      free(p -> id);
      free(p -> name);
      /*initialize to 0*/
      p->id = NULL;
      p->name = NULL;
      p->next_id = NULL;
      p->next_name = NULL;
      p->prev_id = NULL;
      p->prev_name = NULL;
      p->purchase = 0; 
      /*delete node*/
      free(p);
      d->numItems--;
      /*unregister success*/
      return 0;
    }
  }
  /*search through list of hash table with id*/
  for(p = d->hashtable_id[h_id]; p!= NULL;)
  {
    if(p->next_id && strcmp(p->next_id->id, id) == 0)
    {
      /*matched*/
      nextp_id = p -> next_id;
      /*will delete nextp_id*/
      if(nextp_id->prev_name != NULL)
      {
        /*if Node isn't head of name's list*/
        nextp_id->prev_name->next_name = nextp_id->next_name;
        /*update node's list of name*/
      }
      else
      {
        /*if Node is head of name's list*/
        int h_name = hash_function(nextp_id->name, d->numBuckets);
        d->hashtable_name[h_name] = nextp_id->next_name;
        /*update hashtable of name*/
      }
      if(nextp_id->next_name != NULL)
      {
        nextp_id->next_name->prev_name = nextp_id->prev_name;
        /*next's prev is current's prev*/
      }
      p->next_id = nextp_id->next_id;
      /*link to next next node*/
      if(p->next_id != NULL)
      {
        p->next_id->prev_id = p;
        /*next's prev is current*/
      }
      /*free allocated node*/
      free(nextp_id -> id);
      free(nextp_id -> name);
      /*initialize to 0*/
      nextp_id->id = NULL;
      nextp_id->name = NULL;
      nextp_id->next_id = NULL;
      nextp_id->next_name = NULL;
      nextp_id->prev_id = NULL;
      nextp_id->prev_name = NULL;
      nextp_id->purchase = 0;
      /*delete node*/
      free(nextp_id);
      d->numItems--;
      /*unregister success*/
      return 0;
    }
    else
    {
      /*keep search list*/
      p = p -> next_id;
    }
  }
  return -1;
  /*unregister fail*/
}

/*--------------------------------------------------------------------*/
/*
function UnregisterCustomerByName
  Unregister node with name
parameter
  DB_T d - DB
  const char *name - customer name
return
  int - 0 for success, -1 for fail
*/
int
UnregisterCustomerByName(DB_T d, const char *name)
{
  /*check missed data*/
  if(d == NULL || name == NULL)
  {
    return -1;
  }
  UserInfo_T p, nextp_name;
  /*Node for searching*/
  int h_name = hash_function(name,d->numBuckets);
  /*get name's hashcode*/
  /*seach hashtable of name*/
  if(d->hashtable_name[h_name] != NULL)
  {
    /*check head of list*/
    if(strcmp(d->hashtable_name[h_name]->name, name) == 0)
    {
      /*matched*/
      p = d->hashtable_name[h_name];
      d->hashtable_name[h_name] = d->hashtable_name[h_name]
                                   ->next_name;
      /*make hashtable assign next node*/
      if(d->hashtable_name[h_name] != NULL)
      {
        /*is there new head*/
        d->hashtable_name[h_name]->prev_name = NULL;
        /*updated node is head*/
      }
      if(p->prev_id != NULL)
      {
        /*if Node isn't head of id's list*/
        p->prev_id->next_id = p->next_id;
        /*update node's list of name*/
      }
      else 
      {
        /*if Node is head of id's list*/
        int h_id = hash_function(p->id, d->numBuckets);
        d->hashtable_id[h_id] = p->next_id;
        /*update hashtable of id*/
      }
      if(p->next_id != NULL)
      {
        p->next_id->prev_id = p->prev_id;
        /*update node's list of id*/
      }
      /*free allocated node*/
      free(p -> id);
      free(p -> name);
      /*initialize to 0*/
      p->id = NULL;
      p->name = NULL;
      p->next_id = NULL;
      p->next_name = NULL;
      p->prev_id = NULL;
      p->prev_name = NULL;
      p->purchase = 0; 
      /*delete node*/
      free(p);
      d->numItems--;
      /*unregister success*/
      return 0;
    }
  }
  /*search through list of hash table with name*/
  for(p = d->hashtable_name[h_name]; p!= NULL;)
  {
    if(p->next_name 
       && strcmp(p->next_name->name, name) == 0)
    {
      /*matched*/
      nextp_name = p -> next_name;
      /*will delete nextp_name*/
      if(nextp_name->prev_id != NULL)
      {
        /*if Node isn't head of id's list*/
        nextp_name->prev_id->next_id = nextp_name->next_id;
        /*update node's list of id*/
      }
      else
      {
        /*if Node is head of id's list*/
        int h_id = hash_function(nextp_name->id, d->numBuckets);
        d->hashtable_id[h_id] = nextp_name->next_id;
        /*update hashtable of id*/
      }
      if(nextp_name->next_id != NULL)
      {
        nextp_name->next_id->prev_id = nextp_name->prev_id;
        /*next's prev is current's prev*/
      }
      p->next_name = nextp_name->next_name;
      /*link to next next node*/
      if(p->next_name != NULL)
      {
        p->next_name->prev_name = p;
        /*next's prev is current*/
      }
      /*free allocated node*/
      free(nextp_name->id);
      free(nextp_name -> name);
      /*initialize to 0*/
      nextp_name->id = NULL;
      nextp_name->name = NULL;
      nextp_name->next_id = NULL;
      nextp_name->next_name = NULL;
      nextp_name->prev_id = NULL;
      nextp_name->prev_name = NULL;
      nextp_name->purchase = 0; 
      /*delete node*/
      free(nextp_name);
      d->numItems--;
      /*unregister success*/
      return 0;
    }
    else
    {
      /*keep search list*/
      p = p -> next_name;
    }
  }
  return -1;
  /*unregister fail*/
}
/*--------------------------------------------------------------------*/
/*
function GetPurchaseByID
  Get customer's purchase by id
parameter
  DB_T d - DB
  const char *id - customer id
return
  int - purchase of customer, -1 for fail
*/
int
GetPurchaseByID(DB_T d, const char* id)
{
  /*check missed data*/
  if(d == NULL || id == NULL)
  {
    return -1;
  }
  UserInfo_T p;
  /*Node for searching*/
  int h_id = hash_function(id,d->numBuckets);
  /*get id's hashcode*/
  /*seach hashtable of id*/
  for(p = d->hashtable_id[h_id]; p!=NULL; p = p->next_id)
  {
    if(strcmp(p->id, id) == 0)
    {
      /*matched*/
      return p->purchase;
      /*get success*/
    }
  }
  return -1;
  /*get fail*/
}
/*--------------------------------------------------------------------*/
/*
function GetPurchaseByName
  Get customer's purchase by name
parameter
  DB_T d - DB
  const char *name - customer name
return
  int - purchase of customer, -1 for fail
*/
int
GetPurchaseByName(DB_T d, const char* name)
{
  /*check missed data*/
  if(d == NULL || name == NULL)
  {
    return -1;
  }
  UserInfo_T p;
  /*Node for searching*/
  int h_name = hash_function(name,d->numBuckets);
  /*get name's hashcode*/
  /*seach hashtable of name*/
  for(p = d->hashtable_name[h_name]; p!=NULL; p = p->next_name)
  {
    if(strcmp(p->name, name) == 0)
    {
      /*matched*/
      return p->purchase;
      /*get success*/
    }
  }
  return -1;
  /*get fail*/
}
/*--------------------------------------------------------------------*/
/*
function GetSumCustomerPurchase
  Get sum of customer's purchase with fp
parameter
  DB_T d - DB
  FUNCPTR_T fp - function pointer
                int(id,name,purchase)
return
  int - sum of purchase of customer satisfy fp, -1 for fail
*/
int
GetSumCustomerPurchase(DB_T d, FUNCPTR_T fp)
{
  /*check missed data*/
  if(d == NULL || fp == NULL)
  {
    return -1;
  }
  int b, sum = 0;
  /*index of hashtable, sum*/
  UserInfo_T p;
  /*Node for searching*/
  /*search every customer with id*/
  for(b = 0; b < d->numBuckets; b++)
  {
    for(p = d->hashtable_id[b]; p != NULL; p = p->next_id)
    {
      sum += fp(p->id,p->name,p->purchase);
      /*sum if customer satisfy function fp*/
    }
  }
  return sum;
  /*get sum success*/
}