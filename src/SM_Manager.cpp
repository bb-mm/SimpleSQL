#include <cstdio>
#include <iostream>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include "redbase.h"
#include "SM_Manager.h"
#include "rm.h"
#include <vector>
#include <string>
#include <set>
#include "stddef.h"
#include "statistics.h"
#include <cfloat>
using namespace std;

SM_Manager::SM_Manager( RM_Manager &rmm) :  rmm(rmm){
  DBOpen = false;
}
SM_Manager::~SM_Manager()
{

}
int SM_Manager::OpenDb(const char *dbName)
{
  int error = 0;

  if(strlen(dbName) > 255){ // checks for valid dbName
    return (SM_INVALIDDB);
  }

  if(chdir(dbName) < 0){ // Checks for errors while Chdir-ing
    cerr << "Cannot chdir to " << dbName << "\n";
    return (SM_INVALIDDB);
  }

  // Open and keep the relcat and attrcat filehandles stored
  // during duration of database
  if((error = rmm.OpenFile("relcat", relafh) )){
    return (SM_INVALIDDB);
  }
  if((error = rmm.OpenFile("attrcat", attrfh))) {
    return (SM_INVALIDDB);
  }

  return 0;
}
int SM_Manager::CloseDb()
{

  int error = 0;
  if((error = rmm.CloseFile(relafh) )){
    return (error);
  }
  if((error = rmm.CloseFile(relafh))){
    return (error);
  }

  return 0;
}
