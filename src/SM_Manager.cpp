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
/*
 * This function returns true if the attribute type
 * and attribute length are compatible
 */
bool SM_Manager::isValidAttrType(AttrInfo attribute){

  AttrType type = attribute.attrType;
  int length = attribute.attrLength;
  if(type == INT && length == 4)
    return true;
  if(type == FLOAT && length == 4)
    return true;
  if(type == STRING && (length > 0) && length < MAXSTRINGLEN)
    return true;

  return false;
}
/*
 * This function inserts a relation entry into relcat.
 */
int SM_Manager::InsertRelCat(const char *relName, int attrCount, int recSize){
  int rc = 0;

  RelCatEntry* rEntry = (RelCatEntry *) malloc(sizeof(RelCatEntry));
  memset((void*)rEntry, 0, sizeof(*rEntry));
  *rEntry = (RelCatEntry) {"\0", 0, 0, 0, 0};
  memcpy(rEntry->relName, relName, MAXNAME + 1); // name
  rEntry->tupleLength = recSize;                 // record size
  rEntry->attrCount = attrCount;                 // # of attributes

  // Insert into relcat
  RID relRID;
  rc = relafh.InsertRec((char *)rEntry, relRID);
  free(rEntry);

  return rc;
}
/*
 * This function inserts an attribute into attrcat
 */
int SM_Manager::InsertAttrCat(const char *relName, AttrInfo attr, int offset, int attrNum){
  int rc = 0;

  AttrCatEntry *aEntry = (AttrCatEntry *)malloc(sizeof(AttrCatEntry));
  memset((void*)aEntry, 0, sizeof(*aEntry));
  *aEntry = (AttrCatEntry) {"\0", "\0", 0, INT, 0, 0, 0};
  memcpy(aEntry->relName, relName, MAXNAME + 1);        // relation anme
  memcpy(aEntry->attrName, attr.attrName, MAXNAME + 1); // attribute name
  aEntry->offset = offset;                // attribute offset
  aEntry->attrType = attr.attrType;       // type
  aEntry->attrLength = attr.attrLength;   // length
  aEntry->attrNum = attrNum;              // attribute # in sequence for this relation

  // Do insertion
  RID attrRID;
  rc = attrfh.InsertRec((char *)aEntry, attrRID);
  free(aEntry);


  return rc;

}
int SM_Manager::CreateTable(const char *relName,
                           int        attrCount,
                           AttrInfo   *attributes)
{
  cout << "CreateTable\n"
    << "   relName     =" << relName << "\n"
    << "   attrCount   =" << attrCount << "\n";
  for (int i = 0; i < attrCount; i++)
    cout << "   attributes[" << i << "].attrName=" << attributes[i].attrName
        << "   attrType="
        << (attributes[i].attrType == INT ? "INT" :
            attributes[i].attrType == FLOAT ? "FLOAT" : "STRING")
        << "   attrLength=" << attributes[i].attrLength << "\n";

  int rc = 0;
  set<string> relAttributes;

  // Check for appropraite # of attributes
  if(attrCount > MAXATTRS || attrCount < 1){
    printf("reaches here\n");
    return (SM_BADREL);
  }
  if(strlen(relName) > MAXNAME) // Check for valid relName size
    return (SM_BADRELNAME);

  // Check the attribute specifications
  int totalRecSize = 0;
  for(int i = 0; i < attrCount; i++){
    if(strlen(attributes[i].attrName) > MAXNAME) // check name size
      return (SM_BADATTR);
    if(! isValidAttrType(attributes[i])) // check type
      return (SM_BADATTR);
    totalRecSize += attributes[i].attrLength;
    string attrString(attributes[i].attrName); // check attribute dups
    bool exists = (relAttributes.find(attrString) != relAttributes.end());
    if(exists)
      return (SM_BADREL);
    else
      relAttributes.insert(attrString);
  }

  // Passed error check. Now information in relcat and attrcat

  // Create a file for this relation. This will check for duplicate tables
  // of the same name.
  if((rc = rmm.CreateFile(relName, totalRecSize)))
    return (SM_BADRELNAME);

  // For each attribute, insert into attrcat:
  RID rid;
  int currOffset = 0;
  for(int i = 0; i < attrCount; i++){
    AttrInfo attr = attributes[i];
    if((rc = InsertAttrCat(relName, attr, currOffset, i)))
      return (rc);
    currOffset += attr.attrLength;
  }

  // Insert into RelCat
  if((rc = InsertRelCat(relName, attrCount, totalRecSize)))
    return (rc);

  // Make sure changes to attrcat and relcat are reflected
  if((rc = attrfh.ForcePages()) || (rc = relafh.ForcePages()))
    return (rc);

  return (0);
}
/*
 * This destroys a table, all its indices, and removes the contents
 * from relcat and attrcat
 */
int SM_Manager::DropTable(const char *relName)
{
  cout << "DropTable\n   relName=" << relName << "\n";
  int rc = 0;

  if(strlen(relName) > MAXNAME) // check for whether this is a valid name
    return (SM_BADRELNAME);
  // Try to destroy the table. This should detect whether the file is there
  if((rc = rmm.DestroyFile(relName))){
    return (SM_BADRELNAME);
  }

  // Retrieve the record associated with the relation
  RM_Record relRec;
  RelCatEntry *relEntry;
  if((rc = GetRelEntry(relName, relRec, relEntry)))
    return (rc);
  int numAttr = relEntry->attrCount;

  // Retrieve all its attributes
//  SM_AttrIterator attrIt;
//  if((rc = attrIt.OpenIterator(attrcatFH, const_cast<char*>(relName))))
//    return (rc);
  RM_FileScan fs;
  if((rc = fs.OpenScan(attrfh, STRING, MAXNAME+1, 0, EQ_OP, const_cast<char*>(relName))))
      return (rc);
  AttrCatEntry *attrEntry;
  RM_Record attrRec;
  for(int i=0; i < numAttr; i++){
    // Get the next attribute
//    if((rc = attrIt.GetNextAttr(attrRec, attrEntry))){
//      return (rc);
//    }
	  if((rc = fs.GetNextRec(attrRec)))
	      return (rc);
	  if((rc = attrRec.GetData((char *&)attrEntry)))
	      return (rc);
    // Delete that attribute record
    RID attrRID;
    if((rc = attrRec.GetRid(attrRID)) || (rc = attrfh.DeleteRec(attrRID)))
      return (rc);

  }
//  if((rc = attrIt.CloseIterator()))
//    return (rc);
  if((rc = fs.CloseScan()))
      return (rc);

  // Delete the record associated with the relation
  RID relRID;
  if((rc = relRec.GetRid(relRID)) || (rc = relafh.DeleteRec(relRID)))
    return (rc);

  return (0);
}
/*
 * Retrieves the record and data associated with a specific relation in relcat
 */
int SM_Manager::GetRelEntry(const char *relName, RM_Record &relRec, RelCatEntry *&entry){
  int rc = 0;
  // Use a scan to search for it
  RM_FileScan fs;
  if((rc = fs.OpenScan(relafh, STRING, MAXNAME+1, 0, EQ_OP, const_cast<char*>(relName))))
    return (rc);
  if((rc = fs.GetNextRec(relRec))) // there should be only one entry
    return (SM_BADRELNAME);

  if((rc = fs.CloseScan()))
    return (rc);

  if((rc = relRec.GetData((char *&)entry))) // retrieve its data contents
    return (rc);

  return (0);
}
int SM_Manager::GetAllRels(RelCatEntry *relEntries, int nRelations, const char * const relations[],
  int &attrCount, map<string, int> &relToInt){
  int rc = 0;
  for(int i=0; i < nRelations; i++){
    RelCatEntry *rEntry;
    RM_Record rec;
    if((rc = GetRelEntry(relations[i], rec, rEntry))) // retrieve this entry
      return (rc);
    *(relEntries + i) = (RelCatEntry) {"\0", 0, 0, 0, 0};
    memcpy((char *)(relEntries + i), (char *)rEntry, sizeof(RelCatEntry)); // copy it into appropraite spot
    attrCount += relEntries[i].attrCount;

    // create a map from relation name to # in order
    string relString(relEntries[i].relName);
    relToInt.insert({relString, i});
  }
  return (rc);
}
/*
 * Given a RelCatEntry, it populates aEntry with information about all its attribute.
 * While doing so, it also updates the attribute-to-relation mapping
 */
int SM_Manager::GetAttrForRel(RelCatEntry *relEntry, AttrCatEntry *aEntry, std::map<std::string, std::set<std::string> > &attrToRel){
  int rc = 0;
  // Iterate through all the attributes in this relation
//  SM_AttrIterator attrIt;
//  if((rc = attrIt.OpenIterator(attrcatFH, (relEntry->relName))))
//    return (rc);
  RM_FileScan fs;
  if((rc = fs.OpenScan(attrfh, STRING, MAXNAME+1, 0, EQ_OP, const_cast<char*>(relEntry->relName))))
      return (rc);
  RM_Record attrRec;
  AttrCatEntry *attrEntry;
  for(int i = 0; i < relEntry->attrCount; i++){
    // For each attribute, get its AttrCatEntry
//    if((rc = attrIt.GetNextAttr(attrRec, attrEntry)))
//      return (rc);
	  if((rc = fs.GetNextRec(attrRec)))
	  	      return (rc);
	  if((rc = attrRec.GetData((char *&)attrEntry)))
	  	      return (rc);
    int slot = attrEntry->attrNum;
    *(aEntry + slot) = (AttrCatEntry) {"\0", "\0", 0, INT, 0, 0, 0};
    memcpy((char *)(aEntry + slot), (char *)attrEntry, sizeof(AttrCatEntry));

    // add this attribute to the mapping from attribute name to set of relations with this attribute name
    string attrString(aEntry[slot].attrName);
    string relString(relEntry->relName);
    map<string, set<string> >::iterator it = attrToRel.find(attrString);
    if(it == attrToRel.end()){ // if there isn't a set already for this attribute, create it
      set<string> relNames;
      relNames.insert(relString);
      attrToRel.insert({attrString, relNames});
    }
    else{ // otherwise, just add it.
      attrToRel[attrString].insert(relString);
    }
  }
//  if((rc = attrIt.CloseIterator())) // Done with search!
//    return (rc);
  if((rc = fs.CloseScan()))
        return (rc);
  return (0);
}
/*
 * This function returns a record and its data for a particular
 * attribute in a particular relation
 */
int SM_Manager::FindAttr(const char *relName, const char *attrName, RM_Record &attrRec, AttrCatEntry *&entry){
  int rc = 0;
  RM_Record relRec;
  RelCatEntry * rEntry;
  if((rc = GetRelEntry(relName, relRec, rEntry))) // get the relation
    return (rc);

  // Iterate through attrcat for attributes in this relation
//  SM_AttrIterator attrIt;
//  if((rc = attrIt.OpenIterator(attrcatFH, const_cast<char*>(relName))))
//    return (rc);
  RM_FileScan fs;
  if((rc = fs.OpenScan(attrfh, STRING, MAXNAME+1, 0, EQ_OP, const_cast<char*>(relEntry->relName))))
        return (rc);
  bool notFound = true;
  while(notFound){
//    if((RM_EOF == attrIt.GetNextAttr(attrRec, entry))){
//      break;
	  if((RM_EOF == fs.GetNextRec(attrRec)))
	  	  	      break;
	  if((RM_EOF == attrRec.GetData((char *&)entry)))
	  	  	      break;
    //}
    // Check if the attribute names match
    if(strncmp(entry->attrName, attrName, MAXNAME + 1) == 0){
      notFound = false;
      break;
    }
  }
//  if((rc = attrIt.CloseIterator())) // Done with search!
//    return (rc);
  if((rc = fs.CloseScan()))
        return (rc);
  if(notFound == true)  // If attribute is not found, return error
    return (SM_INVALIDATTR);
  return (rc);

}
