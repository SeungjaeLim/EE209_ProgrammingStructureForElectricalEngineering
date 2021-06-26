/* Seungjae Lim / Assignment3 / customer_manager1.c */
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "customer_manager.h"



#define UNIT_ARRAY_SIZE 1024
/*start hash table size*/
struct UserInfo {
  char *name;                // customer name
  char *id;                  // customer id
  int purchase;              // purchase amount (> 0)
};

struct DB {
  struct UserInfo **pArray;   // pointer to the array of pointer
  int curArrSize;            // current array size (max # of elements)
  int numItems;              // # of stored items, needed to determine
			     // # whether the array should be expanded
			     // # or not
};

/*struct UserInfo* define to UserInfo_T*/
typedef struct UserInfo* UserInfo_T;

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
  d = (DB_T) calloc(1, sizeof(struct DB));
  /*allocate DB*/
  if (d == NULL) {
    /*allocating error occur*/
    fprintf(stderr, "Can't allocate a memory for DB_T\n");
    return NULL;
  }
  /*initialize DB*/
  d->curArrSize = UNIT_ARRAY_SIZE; // start with 1024 elements
  /*allocate Array*/
  d->pArray = (struct UserInfo **)calloc(d->curArrSize,
               sizeof(UserInfo_T));
  if (d->pArray == NULL) {
    /*allocating error occur*/
    fprintf(stderr, "Can't allocate a memory for array of size %d\n",
	    d->curArrSize);  
    /*free allocated data*/ 
    free(d);
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
  /*check NULL pointer*/
  if(d == NULL)
  {
    return;
  }
  /*check NULL pointer*/
  if(d->pArray)
  {
    /*search every customer for delete*/
    for(int i = 0; i < d->curArrSize; i++)
    {
      if(d->pArray[i] != NULL)
        {
          /*free allocated data*/
          free(d->pArray[i]->id);
          free(d->pArray[i]->name);
          free(d->pArray[i]);
          /*initialize to 0*/
          d->pArray[i] = NULL;
        }
    }
    /*free array*/
    free(d->pArray);
  }
  /*free DB*/
  free(d);
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
  UserInfo_T customer = (UserInfo_T)malloc(sizeof(struct UserInfo));
  /*allocate customer*/
  int idx;
  /*index of array*/
  idp = strdup(id);
  /*make own id for data*/
  namep = strdup(name);
  /*make own name for data*/
  customer->id = idp;
  customer->name = namep;
  customer->purchase = purchase;
  /*initialize customer*/
  for(idx = 0; idx < d ->curArrSize ; idx++)
  {
    /*check reduplicating name and id*/
    if(d->pArray[idx] == NULL)
    {
      /*not matched*/
      continue;
    }
    /*search reduplicating id*/
    if(strcmp(d->pArray[idx]->id, idp) == 0)
    {
      return -1;
      /*reduplicating id -> fail*/
    }
    /*search reduplicating name*/
    if(strcmp(d->pArray[idx]->name, namep) == 0)
    {
      return -1;
      /*reduplicating name -> fail*/
    }
  }
  /*dynamic array's realloc also cost time for copy, 
    so twice when half is filled is faster*/
  if(2*(d->numItems) >= d->curArrSize)
  {
    d->pArray = (struct UserInfo **)realloc(d->pArray, 
                2*(d->curArrSize)*sizeof(UserInfo_T));
    /*allocate new array*/
    d->curArrSize = 2*(d->curArrSize);
    /*update curArrSize*/
  }
  /*linear search for register*/
  for(idx = 0; idx < d->curArrSize ; idx++)
  {
    if(d->pArray[idx] == NULL)
    {
      /*find empty entry*/
      d->pArray[idx] = customer;
      d->numItems++;
      /*register customer*/
      return 0;
      /*register success*/
    }
  }
  return (-1);
  /*register fail*/
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
  int idx, isUnregister = 0;
  /*index, check unregistered*/
  /*check missed data*/
  if(d == NULL || id == NULL)
  {
    return -1;
  }
  /*search array for unregister*/
  for(idx = 0; idx < d ->curArrSize ; idx++)
  {
    if(d->pArray[idx] == NULL)
    {
      /*not matched*/
      continue;
    }
    if(strcmp(d->pArray[idx]->id, id) == 0)
    {
      /*matched*/
      isUnregister = 1;
      /*free customer*/
      free(d->pArray[idx]->id);
      free(d->pArray[idx]->name);
      free(d->pArray[idx]);
      /*delete customer*/
      d->pArray[idx] = NULL;
      d->numItems = d->numItems - 1;
    }
  }
  if(isUnregister)
  {
    return 0;
    /*unregister success*/
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
  int idx, isUnregister = 0;
  /*index, check unregistered*/
  /*check missed data*/
  if(d == NULL || name == NULL)
  {
    return -1;
  }
  /*search array for unregister*/
  for(idx = 0; idx < d ->curArrSize ; idx++)
  {
    if(d->pArray[idx] == NULL)
    {
      /*not matched*/
      continue;
    }
    if(strcmp(d->pArray[idx]->name, name) == 0)
    {
      /*matched*/
      isUnregister = 1;
      /*free customer*/
      free(d->pArray[idx]->id);
      free(d->pArray[idx]->name);
      free(d->pArray[idx]);
      /*delete customer*/
      d->pArray[idx] = NULL;
      d->numItems = d->numItems - 1;
    }
  }
  if(isUnregister)
  {
    return 0;
    /*unregister success*/
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
  int idx;
  /*search array for get data*/
  for(idx = 0; idx < d->curArrSize ; idx++)
  {
    if(d->pArray[idx] == NULL)
    {
      /*not matched*/
      continue;
    }
    if(strcmp(d->pArray[idx]->id, id) == 0)
    {
      /*matched*/
      return d->pArray[idx]->purchase;
      /*get success*/
    }
  }
  return (-1);
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
  int idx;
  /*search array for get data*/
  for(idx = 0; idx < d->curArrSize ; idx++)
  {
    if(d->pArray[idx] == NULL)
    {
      /*not matched*/
      continue;
    }
    if(strcmp(d->pArray[idx]->name, name) == 0)
    {
      /*matched*/
      return d->pArray[idx]->purchase;
      /*get success*/
    }
  }
  return (-1);
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
  int idx, sum = 0;
  /*index, sum*/
  /*search every customer in array*/
  for(idx = 0; idx < d->curArrSize; idx++)
  {
    /*check empty customer data*/
    if(d->pArray[idx] != NULL)
    {
      sum += fp(d->pArray[idx]->id, 
                d->pArray[idx]->name,
                d->pArray[idx]->purchase);
      /*sum if customer satisfy function fp*/
    }
  }
  return sum;
  /*get sum success*/
}