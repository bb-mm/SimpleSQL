#ifndef SM_MANAGER_H
#define SM_MANAGER_H
#include "pf.h"
#include "redbase.h"
#include "rm.h"
// Define the catalog entry for a relation
typedef struct RelCatEntry{
  char relName[MAXNAME + 1];
  int tupleLength;
  int attrCount;
} RelCatEntry;

// Define catalog entry for an attribute
typedef struct AttrCatEntry{
  char relName[MAXNAME + 1];
  char attrName[MAXNAME +1];
  int offset;
  AttrType attrType;
  int attrLength;
  int attrNum;
} AttrCatEntry;


   class SM_Manager {
       public:
            SM_Manager  (RM_Manager &rmm);  // Constructor
            ~SM_Manager ();                                  // Destructor
         int OpenDb      (const char *dbName);                // Open database
         int CloseDb     ();                                  // Close database
         int CreateTable (const char *relName,                // Create relation
                         int        attrCount,
                         AttrInfo   *attributes);
         int DropTable   (const char *relName);               // Destroy relation
//         int CreateIndex (const char *relName,                // Create index
//                         const char *attrName);
//         int DropIndex   (const char *relName,                // Destroy index
//                         const char *attrName);
//         int Load        (const char *relName,                // Load utility
//                         const char *fileName);
//         int Help        ();                                  // Help for database
//         int Help        (const char *relName);               // Help for relation
//         int Print       (const char *relName);               // Print relation
//         int Set         (const char *paramName,              // Set system parameter
//                         const char *value);
       private:
         bool isValidAttrType(AttrInfo attribute);
         // Inserts an entry about specified relName relation into relcat
         int InsertRelCat(const char *relName, int attrCount, int recSize);

           // Insert an entry about specified attribute into attrcat
         int InsertAttrCat(const char *relName, AttrInfo attr, int offset, int attrNum);
           // Retrieve the record and data associated with a relation entry. Return
           // error if one doesnt' exist
         int GetRelEntry(const char *relName, RM_Record &relRec, RelCatEntry *&entry);

           // Finds the entry associated with a particular attribute
         int FindAttr(const char *relName, const char *attrName, RM_Record &attrRec, AttrCatEntry *&entry);
         // Given a RelCatEntry, it populates aEntry with information about all its attribute.
           // While doing so, it also updates the attribute-to-relation mapping
         int GetAttrForRel(RelCatEntry *relEntry, AttrCatEntry *aEntry, std::map<std::string, std::set<std::string> > &attrToRel);
           // Given a list of relations, it retrieves all the relCatEntries associated with them placing
           // them in the list specified by relEntries. It also returns the total # of attributes in all the
           // relations combined, and populates the mapping from relation name to index number in relEntries
         int GetAllRels(RelCatEntry *relEntries, int nRelations, const char * const relations[],
             int &attrCount, std::map<std::string, int> &relToInt);

         RM_Manager& rmm;
         bool DBOpen;
         RM_FileHandle relafh; //relation file
         RM_FileHandle attrfh; //attributes file
     };
   void SM_PrintError(int rc);

   #define SM_CANNOTCLOSE          (START_SM_WARN + 0) // invalid RID
   #define SM_BADRELNAME           (START_SM_WARN + 1)
   #define SM_BADREL               (START_SM_WARN + 2)
   #define SM_BADATTR              (START_SM_WARN + 3)
   #define SM_INVALIDATTR          (START_SM_WARN + 4)
   #define SM_INDEXEDALREADY       (START_SM_WARN + 5)
   #define SM_NOINDEX              (START_SM_WARN + 6)
   #define SM_BADLOADFILE          (START_SM_WARN + 7)
   #define SM_BADSET               (START_SM_WARN + 8)
   #define SM_LASTWARN             SM_BADSET

   #define SM_INVALIDDB            (START_SM_ERR - 0)
   #define SM_ERROR                (START_SM_ERR - 1) // error
   #define SM_LASTERROR            SM_ERROR

#endif
