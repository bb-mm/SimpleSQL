#ifndef SM_MANAGER_H
#define SM_MANAGER_H
#include "pf.h"
#include "redbase.h"
#include "rm.h"
// Used by SM_Manager::CreateTable
   struct AttrInfo {
      char     *attrName;           // Attribute name
      AttrType attrType;            // Type of attribute
      int      attrLength;          // Length of attribute
   };
   typedef struct RelaEntry {
	   char relName[MAXNAME + 1];
	     int tupleLength;
	     int attrCount;
	     int numTuples;
   } RelaEntry;
   typedef struct AttrEntry {
	   char relName[MAXNAME + 1];
	     char attrName[MAXNAME +1];
	     int offset;
	     AttrType attrType;
	     int attrLength;
	     int attrNum;
	     int numDistinct;
	     float maxValue;
	     float minValue;
   }AttrEntry;
   // Used by Printer class
   struct DataAttrInfo {
      char     relName[MAXNAME+1];  // Relation name
      char     attrName[MAXNAME+1]; // Attribute name
      int      offset;              // Offset of attribute
      AttrType attrType;            // Type of attribute
      int      attrLength;          // Length of attribute
      //int      indexNo;             // Attribute index number
   };

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
         int CreateIndex (const char *relName,                // Create index
                         const char *attrName);
         int DropIndex   (const char *relName,                // Destroy index
                         const char *attrName);
         int Load        (const char *relName,                // Load utility
                         const char *fileName);
         int Help        ();                                  // Help for database
         int Help        (const char *relName);               // Help for relation
         int Print       (const char *relName);               // Print relation
         int Set         (const char *paramName,              // Set system parameter
                         const char *value);
       private:

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
