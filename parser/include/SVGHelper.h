#ifndef SVGHELPER_HEADER
#define SVGHELPER_HEADER

#include <stdio.h>
#include <stdlib.h>
#include "SVGParser.h"
#include "LinkedListAPI.h"

//--------------------------------------- Module 1 Helper Functions -------------------------------------------
void parseXMLTree(SVG * newSVG, xmlNode * root_element);
Rectangle * createRect(xmlNode * node);
Circle *createCircle(xmlNode * node);
Path * createPath(xmlNode * node);
Group * createGroup (Group * grp, xmlNode * node);
bool containsUnits (char* attrValue);
bool isPositive (float value);
void removeUnits (char *string);
char * findUnits (char * string);

//--------------------------------------- Module 2 Helper Functions ------------------------------------------
void getGrpRects (Group * group, List * list);
void getGrpCircs (Group * group, List * list);
void getGrpPaths (Group * group, List * list);
void getGrpGroups (Group * group, List * list);
void dummyDelete (void * dummy);

//--------------------------------------- A2 Module 1 Helper Funcitons ----------------------------------------
bool validateXMLSchema(xmlDoc * doc, const char *schemaFile);
xmlDoc * SVGtoXML (const SVG * img);
void addRectToNode (xmlNodePtr parent_node, List * list, char * node_name);
void addCircleToNode(xmlNodePtr parent_node, List * list, char *nodeName);
void addPathToNode(xmlNodePtr parent_node, List * list, char *nodeName);
void addGroupToNode(xmlNodePtr root_node, List * group);

void addListOfOtherAttrToNode(xmlNodePtr parent_node, List *otherAttrList);
void addListOfOtherAttrToParent(xmlNodePtr parent_node, List *otherAttrList);
bool validRectangle (List * rectangle);
bool validCircle (List * circle);
bool validPath (List * path);
bool validGroup (List * group);
bool validAttribute (List * attribute);
bool setRectangle(List * list, int index, Attribute * attribute);
bool setCircle(List * list, int index, Attribute * newAttribute);
void setPath(List * list, int index, Attribute * newAttribute);
void setGroup(List * list, int index, Attribute * newAttribute);
void setAttrInSVG(SVG * img, Attribute * newAttribute);
bool checkOtherAttributes (List * list, Attribute * newAttribute);

//--------------------------------------------- A3 Helper Functions -----------------------------------------
char * SVGFiletoJSON (char * filename, char * schemaFile);
char * SVGDataToRectList (char * filename, char * schemaFile);
char * SVGDataToCircleList (char * filename, char * schemaFile);
char * SVGDataToPathList (char * filename, char * schemaFile);
char * SVGDataToGroupList (char * filename, char * schemaFile);
char * convertAttributeToJSON (char * string, char * schemaFile);
char * convertTitleToJSON(char * string, char * schemaFile);
char * convertDescToJSON(char * string, char * schemaFile);
bool validateSVGinJSON(char * filename, const char* schemaFile);
int findFileSize(char * filename);
bool JSONcreateSVG(char * JSONStr, char * filename);
bool JSONeditSVG(char * filename, char * editTitle, char * editDesc);
char * SVGRectsAttrListToJSON(char * filename);
char * SVGCircleAttrListToJSON(char * filename);
char * SVGPathAttrListToJSON(char * filename);
char * SVGGroupAttrListToJSON(char * filename);
bool JSONeditAttrSVG(char * filename, char * elemType, int elemIndex, char * attrString, char * valString);
bool AddRectangleToJSON(char * filename, char * xString, char * yString, char * width, char * height, char * units);
bool AddCircleToJSON(char * filename, char * cxString, char * cyString, char * radius, char * units);
bool validUnits (char *string);
bool scaleShapes(char * filename, char * rectScale, char * circScale);

#endif