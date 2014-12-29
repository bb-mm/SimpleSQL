//
// ql.h
//   Query Language Component Interface
//

// This file only gives the stub for the QL component

#ifndef QL_H
#define QL_H


#include <string>
#include <set>
#include <map>
#include <stdlib.h>
#include <string.h>
#include "redbase.h"
#include "parser.h"
#include "rm.h"
#include "SM_Manager.h"
#include "ql_node.h"


typedef struct QO_Rel{
  int relIdx;
  int indexAttr;
  int indexCond;
} QO_Rel;

//
// QL_Manager: query language (DML)
//
class QL_Manager {
    friend class QL_Node;
    friend class Node_Rel;
    friend class QL_NodeJoin;
    friend class QL_NodeRel;
    friend class QL_NodeSel;
    friend class QL_NodeProj;
    friend class QO_Manager;
public:
    QL_Manager (SM_Manager &smm,  RM_Manager &rmm);
    ~QL_Manager();                       // Destructor

    RC Select  (int nSelAttrs,           // # attrs in select clause
        const RelAttr selAttrs[],        // attrs in select clause
        int   nRelations,                // # relations in from clause
        const char * const relations[],  // relations in from clause
        int   nConditions,               // # conditions in where clause
        const Condition conditions[]);   // conditions in where clause

    RC Insert  (const char *relName,     // relation to insert into
        int   nValues,                   // # values
        const Value values[]);           // values to insert

    RC Delete  (const char *relName,     // relation to delete from
        int   nConditions,               // # conditions in where clause
        const Condition conditions[]);   // conditions in where clause

    RC Update  (const char *relName,     // relation to update
        const RelAttr &updAttr,          // attribute to update
        const int bIsValue,              // 1 if RHS is a value, 0 if attribute
        const RelAttr &rhsRelAttr,       // attr on RHS to set LHS equal to
        const Value &rhsValue,           // or value to set attr equal to
        int   nConditions,               // # conditions in where clause
        const Condition conditions[]);   // conditions in where clause

private:



  RM_Manager &rmm;
  SM_Manager &smm;

  // helpers for query parsing
  // maps from relation name to its index number in relEntries
  std::map<std::string, int> relToInt;
  // maps from attribute name to a set of its relation names
  std::map<std::string, std::set<std::string> > attrToRel;
  // maps from relation name to the index of its first relation
  std::map<std::string, int> relToAttrIndex;
  // maps from condition to the relation it's meant to be grouped with
  std::map<int, int> conditionToRel;

  // Holds the relation relCat entries
  RelCatEntry *relEntries;
  // Holds the attribute attrCat entries. The attributes are ordered in the
  // appropriate order following the relation, in the order of the relations
  AttrCatEntry *attrEntries;

  // number of attributes, relations and conditions
  int nAttrs;
  int nRels;
  int nConds;
  // whether it's an update or not (whether to use indices or not)
  bool isUpdate;

  // pointer to the condition list
  const Condition *condptr;


};

//
// Print-error function
//
void QL_PrintError(RC rc);

#define QL_BADINSERT            (START_QL_WARN + 0) // Bad insert
#define QL_DUPRELATION          (START_QL_WARN + 1) // Duplicate relation
#define QL_BADSELECTATTR        (START_QL_WARN + 2) // Bad select attribute
#define QL_ATTRNOTFOUND         (START_QL_WARN + 3) // Attribute not found
#define QL_BADCOND              (START_QL_WARN + 4) // Bad condition
#define QL_BADCALL              (START_QL_WARN + 5) // Bad/invalid call
#define QL_CONDNOTMET           (START_QL_WARN + 6) // Condition has not been met
#define QL_BADUPDATE            (START_QL_WARN + 7) // Bad update statement
#define QL_EOI                  (START_QL_WARN + 8) // End of iterator
#define QO_BADCONDITION         (START_QL_WARN + 9)
#define QO_INVALIDBIT           (START_QL_WARN + 10)
#define QL_LASTWARN             QL_EOI

#define QL_INVALIDDB            (START_QL_ERR - 0)
#define QL_ERROR                (START_QL_ERR - 1) // error
#define QL_LASTERROR            QL_ERROR



#endif
