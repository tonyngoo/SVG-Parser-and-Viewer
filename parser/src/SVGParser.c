#ifndef PARSER_FILE
#define PARSER_FILE

#include "SVGParser.h"
#include "SVGHelper.h"
#include "LinkedListAPI.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <strings.h>

/*
    Author: Tony Ngo    Student ID: 1142414
    Email: tngo07@uoguelph.ca
    Professor: Denis Nikitenko
    References: Parts of the code were implemented in this file from Professor Denis Nikitenko's example code.
    The example code is from: http://www.xmlsoft.org/examples/tree1.c
*/

// ---------------------------------- A1 Module 1 -------------------------------------------

SVG* createSVG (const char* fileName) {
    //Error checking if filename is empty string or NULL
    if (fileName == NULL || (strcmp(fileName, "") == 0)) {
        return NULL;
    }

    //Using fopen() to check if the file exists in the current directory
    FILE *file = fopen(fileName, "r");
    
    if (file == NULL) {
        return NULL;
    }

    //Allocating SVG struct
    SVG* newSVG = malloc(sizeof(SVG));
    strcpy(newSVG->namespace, "");
    strcpy(newSVG->title, "");
    strcpy(newSVG->description, "");
    
    //Initializing lists in the SVG struct
    newSVG->rectangles = initializeList(&rectangleToString, &deleteRectangle, &compareRectangles);
    newSVG->circles = initializeList(&circleToString, &deleteCircle, &compareCircles);
    newSVG->paths = initializeList(&pathToString, &deletePath, &comparePaths);
    newSVG->groups = initializeList(&groupToString, &deleteGroup, &compareGroups);
    newSVG->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
    

    //Variables to read the SVG file
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    //Parsing the file
    doc = xmlReadFile(fileName, NULL, 0);
    root_element = xmlDocGetRootElement(doc);
    
    //Call the helper parser to parse the entire SVG
    parseXMLTree(newSVG, root_element);

    //Free, cleanup and close the file
    xmlFreeDoc(doc);
    xmlCleanupParser();
    fclose(file);

    return newSVG;
}

char* SVGToString (const SVG* img) {
    //If the SVG passed in was empty, then return
    if (img == NULL) {
        return NULL;
    }

    //Initializing variables
    int len = 0;
    char * rectList = toString(img->rectangles);
    char * circList = toString(img->circles);
    char * pathList = toString(img->paths);
    char * groupList = toString(img->groups);
    char * otherList = toString(img->otherAttributes);

    //Calculating length and allocating space for str, then using sprintf to store the str
    len = strlen(img->namespace) + strlen(img->title) + strlen(img->description) + strlen(rectList) + strlen(circList) + strlen(pathList) + strlen(groupList) + strlen(otherList) + 200;
    char * str = (char*)malloc(sizeof(char) * (len + 200));
    sprintf(str, "Namespace: %s\nTitle: %s\nDescription: %s\n%s\n%s\n%s\n%s\n%s\n", img->namespace, img->title, img->description, rectList, circList, pathList, groupList, otherList);

    //Freeing memory and then returning str afterwards
    free(rectList);
    free(circList);
    free(pathList);
    free(groupList);
    free(otherList);

    return(str);
}

void deleteSVG(SVG* img) {
    //If SVG passed in was empty then return
    if(img == NULL) {
        return;
    }

    //Free all the lists for the img struct and then freeing the struct itself
    freeList(img->otherAttributes);
    freeList(img->rectangles);
    freeList(img->circles);
    freeList(img->paths);
    freeList(img->groups);
    free(img);
}

void deleteAttribute(void* data) {
    Attribute * attr;
    //If data is empty, then return
    if (data == NULL) {
        return;
    }
    //Freeing the attr
    attr = (Attribute*)data;
    free(attr->name);
    free(attr);
}

char* attributeToString(void* data) {
    char * tempStr;
    Attribute * attr;
    int length;

    //If data is empty, then return
    if (data == NULL) {
        return NULL;
    }

    attr = (Attribute*)data;

    //Calculate length for tempStr and then storing tempStr with the attributes
    length = strlen(attr->name) + strlen(attr->value) + 20;
    tempStr = (char*)malloc(sizeof(char)*length);
    sprintf(tempStr, "\nName: %s\nValue: %s\n", attr->name, attr->value);

    return tempStr;
}

int compareAttributes(const void *first, const void *second) {
    return 0;
}

void deleteGroup(void* data) {
    //If data is empty, then return
    if (data == NULL) {
        return;
    }
    //Free the group Lists and then free the group struct itself
    Group * group = (Group*)data;
    freeList(group->rectangles);
    freeList(group->circles);
    freeList(group->paths);
    freeList(group->groups);
    freeList(group->otherAttributes);
    free(group);
}

char* groupToString(void* data) {
    char * tempStr;
    char * rect;
    char * circ;
    char * paths;
    char * groups;
    char * otherAttr;
    Group * group;
    int length;

    //If data is empty, then return
    if (data == NULL) {
        return NULL;
    }

    group = (Group*)data;

    rect = toString(group->rectangles);
    circ = toString(group->circles);
    paths = toString(group->paths);
    groups = toString(group->groups);
    otherAttr = toString(group->otherAttributes);

    //Calculate the length of tempStr and then store the shapes inside it
    length = strlen(rect) + strlen(circ) + strlen(paths) + strlen(groups) + strlen(otherAttr) + 250;
    tempStr = (char*)malloc(sizeof(char)*length);
    sprintf(tempStr, "\nRectangle List: %s\nCircles List: %s\nPaths List: %s\nGroups List: %s\nOther Group Attributes: %s\n", rect, circ, paths, groups, otherAttr);

    //Freeing memory and then returning the tempStr
    free(rect);
    free(circ);
    free(paths);
    free(groups);
    free(otherAttr);

    return tempStr;
}

int compareGroups(const void *first, const void *second) {
    return 0;
}

void deleteRectangle (void* data) {
    Rectangle * rect;
    //If data is empty, then return
    if (data == NULL) {
        return;
    }
    //Free the memory for rect
    rect = (Rectangle*)data;

    freeList(rect->otherAttributes);
    free(rect);
}

char* rectangleToString(void* data) {
    char * tempStr;
    char * otherAttr;
    Rectangle *rect;
    int length;

    //If data is empty, then return
    if (data == NULL) {
        return NULL;
    }

    rect = (Rectangle*)data;

    otherAttr = toString(rect->otherAttributes);

    //Calculate length for tempStr and then store the elements of the rectangle in it
    length = ((sizeof(float))*4) + strlen(otherAttr) + strlen(rect->units) + 100; 
    tempStr = (char*)malloc(sizeof(char)*length);
    sprintf(tempStr, "x: %f | y: %f | Width: %f | Height: %f | Units: %s\nOther Rectangle Attributes: %s\n", rect->x, rect->y, rect->width, rect->height, rect->units, otherAttr);

    //Free memory and return tempStr
    free(otherAttr);

    return tempStr;
}

int compareRectangles(const void *first, const void *second) {
    return 0;
}

void deleteCircle(void* data) {
    //If data is empty, then return
    if (data == NULL) {
        return;
    }
    //Free memory for circ
    Circle * circ = (Circle*)data;
    freeList(circ->otherAttributes);
    free(circ);
}

char* circleToString(void* data) {
    char * tempStr;
    char * otherAttr;
    Circle *circ;
    int length;

    //If data is empty, then return
    if (data == NULL) {
        return NULL;
    }

    circ = (Circle*)data;
    otherAttr = toString(circ->otherAttributes);

    //Calculate length for tempStr and then store the elements of the circle inside it
    length = (sizeof(float)*3) + strlen(otherAttr) + strlen(circ->units) + 80; 
    tempStr = (char*)malloc(sizeof(char)*length);

    sprintf(tempStr, "x: %.2f | y: %.2f | Radius: %.2f | Units: %s\nOther Circle Attributes: %s\n", circ->cx, circ->cy, circ->r, circ->units, otherAttr);

    //Free memory for the otherAttributes and then return the tempStr
    free(otherAttr);

    return tempStr;
}

int compareCircles(const void *first, const void *second) {
    return 0;
}

void deletePath(void* data) {   
    Path *path;
    //If data is empty, then return
    if(data == NULL) {
        return;
    }

    //Free memory for the path to delete it
    path = (Path*)data;

    freeList(path->otherAttributes);
    free(path);  
}

char* pathToString(void* data) {
    char * tempStr;
    char * otherAttr;
    Path* tempPath;
    int length;

    //If data is empty, then return
    if (data == NULL) {
        return NULL;
    }

    tempPath = (Path*)data;

    otherAttr = toString(tempPath->otherAttributes);

    //Calculate length for the tempStr and then store the elements of the path in it
    length = strlen(tempPath->data) + strlen(otherAttr) + 40;
    tempStr = (char*)malloc(sizeof(char)*length);

    sprintf(tempStr, "Data: %s\nOther Path Attributes: %s\n", tempPath->data, otherAttr);

    //Free memory for otherAttr and then return the tempStr
    free(otherAttr);

    return tempStr;
}

int comparePaths(const void *first, const void *second) {
    return 0;
}

// ------------------------------------------------- A1 Module 2 -----------------------------------------------------

// Function that returns a list of all rectangles in the struct
List *getRects (const SVG* img) {
    List * list = initializeList(&rectangleToString, &dummyDelete, &compareRectangles);

    //Return an empty list if the img is empty
    if (img == NULL) {
        return list;
    }
 
    //Point the node to rectangles head
    Node * node = img->rectangles->head;

    //Traverse through all rectangles in img and then insert them into the list
    while (node != NULL) {
        Rectangle * rect = (Rectangle*)node->data;
        insertBack(list, rect);
        node = node->next;
    }

    //Pointing another node to the groups head and then traversing through all rectangles inside the group
    Node * otherNode = img->groups->head;
    while (otherNode != NULL) {
        Group * grp = (Group*)otherNode->data;
        getGrpRects(grp, list);
        otherNode = otherNode->next;

    }
    
    return list;
}

//Function that returns the list of all circles in img
List *getCircles (const SVG* img) {
    List * list = initializeList(&circleToString, &dummyDelete, &compareCircles);

    //If img is empty then return an empty list
    if (img == NULL) {
        return list;
    }
 
    //Point node to circles head
    Node * node = img->circles->head;

    //Traverse through all circles inside img and then insert them into list
    while (node != NULL) {
        Circle * circ = (Circle*)node->data;
        insertBack(list, circ);
        node = node->next;
    }

    //Create another node and point to groups
    Node * otherNode = img->groups->head;

    //Traverse through all groups and find all circles inside the groups
    while (otherNode != NULL) {
        Group * grp = (Group*)otherNode->data;
        getGrpCircs(grp, list);
        otherNode = otherNode->next;
    }
    
    return list;
}

//Function that returns the list of all Paths inside img
List *getPaths (const SVG* img) {
    List * list = initializeList(&pathToString, &dummyDelete, &comparePaths);

    //If img is empty, then return an empty list
    if (img == NULL) {
        return list;
    }
 
    //Pointing node to paths
    Node * node = img->paths->head;

    //Traversing through all paths inside img and then inserting them into list
    while (node != NULL) {
        Path * path = (Path*)node->data;
        insertBack(list, path);
        node = node->next;
    }

    //Creating another node to search through groups for paths and then storing them into list
    Node * otherNode = img->groups->head;
    while (otherNode != NULL) {
        Group * grp = (Group*)otherNode->data;
        getGrpPaths(grp, list);
        otherNode = otherNode->next;
    }
    
    return list;
}

//Function that will get all groups inside the img
List *getGroups (const SVG* img) {
    List * list = initializeList(&groupToString, &dummyDelete, &compareGroups);

    //If img is empty then return an empty list
    if (img == NULL) {
        return list;
    }
 
    //Pointing node to groups
    Node * node = img->groups->head;

    //Traversing through all groups inside the img and then inserting them into the list
    while (node != NULL) {
        Group * group = (Group*)node->data;
        insertBack(list, group);
        getGrpGroups(group, list);
        node = node->next;
    }
    return list;
}

//Function returns the number of rectangles with specific given area
int numRectsWithArea (const SVG* img, float area) {
    //If img is empty or area is negative, return none
    if (img == NULL || area < 0) {
        return 0;
    }
    //Initialize variables and ceil() the area given
    int numCounter = 0;
    int currArea = 0;
    int compareArea = ceil(area);
    List * rect = getRects(img);
    void * elem;

    ListIterator iter = createIterator(rect);

    //Traversing through all the Rectangles inside img
    while ((elem = nextElement(&iter)) != NULL) {
        Rectangle * currRect = (Rectangle*)elem;
        
        //Calculate the area of the rectangle
        currArea = ceil(currRect->width * currRect->height);

        //If the area matches, then increment the counter
        if (currArea == compareArea) {
            numCounter++;
        }
        //Resetting variables to be reused in the next iteration
        currArea = 0;
    }
    freeList(rect);
    return numCounter;
}

//Function returns the number of circles with the specific given area
int numCirclesWithArea (const SVG* img, float area) {
    //If img is empty or area is negative, then return none
    if (img == NULL || area < 0) {
        return 0;
    }
    //Initializing variables and using ceil() on given area
    float pi = 3.141592653589793238;
    int numCounter = 0;
    int currArea = 0;
    int compareArea = ceil(area);
    List * circle = getCircles(img);
    void * elem;

    ListIterator iter = createIterator(circle);

    //Traversing through all Circles inside img
    while ((elem = nextElement(&iter)) != NULL) {
        Circle * currCirc = (Circle*)elem;

        //Calculate the area of the circle
        currArea = ceil(pi* (currCirc->r * currCirc->r));

        //If the area matches, then increment the counter
        if (currArea == compareArea) {
            numCounter++;
        }
        //Resetting area to be reused in the next iteration
        currArea = 0;
    }
    freeList(circle);
    return numCounter;
}

//Function returns the number of Paths with same data inside
int numPathsWithdata(const SVG* img, const char* data) {
    //If data or img is empty, then return none
    if (img == NULL || data == NULL) {
        return 0;
    }
    //Initializing variables
    int numCounter = 0;
    List * path = getPaths(img);
    void * elem;

    ListIterator iter = createIterator(path);

    //Traversing through all paths inside img
    while ((elem = nextElement(&iter)) != NULL) {
        
        //Cast the element to a Path type list
        Path * currPath = (Path*)elem;

        //If the data matches, then increment the counter by 1
        if ((strcmp(currPath->data, data) == 0)) {
            numCounter++;
        }
    }
    //Free the list and then return the counter
    freeList(path);
    return numCounter;
}

//Function returns the number of groups with the specific given length
int numGroupsWithLen(const SVG* img, int len) {
    //If img is empty or length is negative, then return none
    if (img == NULL || len < 0) {
        return 0;
    }
    //Initializing variables
    int numCounter = 0;
    List * grp = getGroups(img);
    void * elem;
    int rectLen = 0;
    int circLen = 0;
    int pathsLen = 0;
    int grpLen = 0;
    int sum = 0;

    ListIterator iter = createIterator(grp);

    //Traversing through all the groups inside img
    while ((elem = nextElement(&iter)) != NULL) {
        
        //Cast the element to a Group type list and then calculate length for each shape
        Group * currGrp = (Group*)elem;
        rectLen = currGrp->rectangles->length;
        circLen = currGrp->circles->length;
        pathsLen = currGrp->paths->length;
        grpLen = currGrp->groups->length;

        //Calculate total length of that specific group
        sum = rectLen + circLen + pathsLen + grpLen;

        //If group length equals given length, increment
        if (sum == len) {
            numCounter++;
        }

        //Resetting variables
        rectLen = 0;
        circLen = 0;
        pathsLen = 0;
        grpLen = 0;
        sum = 0;
    }
    //Free the list and then return the counter
    freeList(grp);
    return numCounter;
}

//Function returns the number of Attributes inside img
int numAttr (const SVG* img) {
    //If img is empty, return none
    if (img == NULL) {
        return 0;
    }
    
    //Initialize variables and gather lists
    int numCounter = 0;
    void * rectElem;
    void * circElem;
    void * pathElem;
    void * grpElem;
    List * rect = getRects(img);
    List * circ = getCircles(img);
    List * path = getPaths(img);
    List * grp = getGroups(img);

    //Create list iterators for all shapes
    ListIterator rectIter = createIterator(rect);
    ListIterator circIter = createIterator(circ);
    ListIterator pathIter = createIterator(path);
    ListIterator grpIter = createIterator(grp);

    //Incrementing the counter for all otherAttributes inside the img base
    numCounter += getLength(img->otherAttributes);

    //Iterating for rectangle list and incrementing for all otherAttributes inside
    while ((rectElem = nextElement(&rectIter)) != NULL) {
        Rectangle * rectangle = (Rectangle*)rectElem;
        numCounter += getLength(rectangle->otherAttributes);        
    }
    //Iterating for circle list and incrementing for all otherAttributes inside
    while ((circElem = nextElement(&circIter)) != NULL) {
        Circle * circle = (Circle*)circElem;
        numCounter += getLength(circle->otherAttributes);        
    }
    //Iterating for path list and incrementing for all otherAttributes inside
    while ((pathElem = nextElement(&pathIter)) != NULL) {
        Path * currPath = (Path*)pathElem;
        numCounter += getLength(currPath->otherAttributes);        
    }
    //Iterating for group list and incrementing for all otherAttributes inside
    while ((grpElem = nextElement(&grpIter)) != NULL) {
        Group * currGrp = (Group*)grpElem;
        numCounter += getLength(currGrp->otherAttributes);        
    }

    //Freeing memory for the lists used and then returning numCounter
    freeList(rect);
    freeList(circ);
    freeList(path);
    freeList(grp);
    return numCounter;
}

//----------------------------------- A2 Module 1 -------------------------------------------

SVG* createValidSVG(const char* fileName, const char* schemaFile) {
    //Error checking if filename is empty string or NULL
    if (fileName == NULL || (strcmp(fileName, "") == 0)) {
        return NULL;
    }

    if (schemaFile == NULL || (strcmp(schemaFile, "") == 0)) {
        return NULL;
    }

    //Using fopen() to check if the file exists in the current directory
    FILE *file = fopen(fileName, "r");
    
    if (file == NULL) {
        return NULL;
    }

    //Allocating SVG struct
    SVG* newSVG = malloc(sizeof(SVG));
    strcpy(newSVG->namespace, "");
    strcpy(newSVG->title, "");
    strcpy(newSVG->description, "");
    
    //Initializing lists in the SVG struct
    newSVG->rectangles = initializeList(&rectangleToString, &deleteRectangle, &compareRectangles);
    newSVG->circles = initializeList(&circleToString, &deleteCircle, &compareCircles);
    newSVG->paths = initializeList(&pathToString, &deletePath, &comparePaths);
    newSVG->groups = initializeList(&groupToString, &deleteGroup, &compareGroups);
    newSVG->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
    
    //Variables to read the SVG file
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    //Parsing the file, returning it to an xmlDoc pointer
    doc = xmlReadFile(fileName, NULL, 0);

    // Validating the SVG file using a helper function called validateXMLSchema. This function returns 1 if the file was valid and 0 if invalid. If 0 then it returns NULL.
    if (validateXMLSchema(doc, schemaFile) == 0) {
        freeList(newSVG->rectangles);
        freeList(newSVG->circles);
        freeList(newSVG->paths);
        freeList(newSVG->groups);
        freeList(newSVG->otherAttributes);
        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
        return NULL;
    }

    // If it was valid, then proceed with creating the SVG struct
    root_element = xmlDocGetRootElement(doc);
    
    //Call the helper parser to parse the entire SVG
    parseXMLTree(newSVG, root_element);

    //Free, cleanup and close the file
    xmlFreeDoc(doc);
    xmlCleanupParser();
    fclose(file);

    return newSVG;
}

bool writeSVG(const SVG* img, const char* fileName) {
    if (img == NULL) {
        return (FALSE);
    }

    if (fileName == NULL || (strcmp(fileName, "") == 0)) {
        return (FALSE);
    }

    // Searching for an occurrence of a period to ensure that the file extension is given
    if (strrchr(fileName, '.') == NULL) {
        return (FALSE);
    }

    // Error checking to make sure the filename has the .svg file extension
    if (strcasecmp(strrchr(fileName, '.'), ".svg") != 0) {
        return(FALSE);
    }

    // Turn the img SVG struct into an xmlTree
    xmlDoc * xmlTree = SVGtoXML(img);

    xmlSaveFormatFileEnc(fileName, xmlTree, "UTF-8", 1);

    xmlFreeDoc(xmlTree);
    xmlCleanupParser();
    xmlMemoryDump();

    return (TRUE);
}

bool validateSVG(const SVG* img, const char* schemaFile) {
    // Error checking if SVG isn't empty or NULL
    if (img == NULL) {
        return (FALSE);
    }

    // Error checking if schemaFile is empty or NULL
    if (schemaFile == NULL || strcmp(schemaFile, "") == 0) {
        return (FALSE);
    }

    FILE * fp;
    // Error checking if the file exists and is readable
    if (!(fp = fopen(schemaFile, "r"))) {
        fclose(fp);
        return (FALSE);
    }
    fclose(fp);

    // Error checking if namespace is NULL
    if (img->namespace == NULL || strcmp(img->namespace, "") == 0) {
        return (FALSE);
    }

    // Error checking if list attributes are NULL
    if (img->rectangles == NULL || img->circles == NULL || img->paths == NULL || img->groups == NULL || img->otherAttributes == NULL) {
        return (FALSE);
    }

    // Checking the list of Rectangles in the given SVG for invalids
    if (validRectangle(img->rectangles) != 1) {
        return (FALSE);
    }

    // Check the list of Circles in the given SVG for invalids
    if (validCircle(img->circles) != 1) {
        return (FALSE);
    }

    // Check the list of Paths in the given SVG for invalids
    if (validPath(img->paths) != 1) {
        return (FALSE);
    }

    // Check the list of Groups for invalids
    if (validGroup(img->groups) != 1) {
        return (FALSE);
    }
    if (validAttribute(img->otherAttributes) != 1) {
        return (FALSE);
    }
    
    xmlDoc * xmlTree = SVGtoXML(img);

    // Call helper function which checks validity of the tree with the schemaFile. If it returns false, then it will return false in the validateSVG function.
    if (validateXMLSchema(xmlTree, schemaFile) == 0) {
        xmlFreeDoc(xmlTree);
        xmlCleanupParser();
        return (FALSE);
    }

    //After testing the xmlTree validity, clean the parser since the other checks won't require it
    xmlFreeDoc(xmlTree);
    xmlCleanupParser();

    // Returns TRUE if the SVG meets requirements from SVGParser.h file and also contains a valid schemaFile
    return (TRUE);

}

// ---------------------------------------------- A2 Module 2 ----------------------------------------------
bool setAttribute(SVG* img, elementType elemType, int elemIndex, Attribute* newAttribute) {
    if (img == NULL || newAttribute == NULL) {
        return (FALSE);
    }
    
    if (newAttribute->name == NULL || strcmp(newAttribute->value, "") == 0) {
        return (FALSE);
    }

    // Error checking if elemIndex is greater than or equal to 0
    if (elemType != SVG_IMG && elemIndex < 0) {
        return (FALSE);
    }
    
    // Error checking if the element type passed exists in the SVG struct
    if ((elemType != CIRC) && (elemType != SVG_IMG) && (elemType != RECT) && (elemType != PATH) && (elemType != GROUP)) {
        return (FALSE);
    }
    

    // If element type to be modified is a Rectangle
    if (elemType == RECT) {
        if (setRectangle(img->rectangles, elemIndex, newAttribute) != 0) {
            return (TRUE);
        }
        else {
            return (FALSE);
        }

    }
    else if (elemType == CIRC) {
        if (setCircle(img->circles, elemIndex, newAttribute) != 0) {
            return (TRUE);
        }
        else {
            return (FALSE);
        }
    }
    else if (elemType == PATH) {
        setPath(img->paths, elemIndex, newAttribute);
    }
    else if (elemType == GROUP) {
        setGroup(img->groups, elemIndex, newAttribute);
    }
    else if (elemType == SVG_IMG) {
        setAttrInSVG(img, newAttribute);
    }
    return (TRUE);
}

void addComponent(SVG* img, elementType type, void* newElement) {
    if (img == NULL || newElement == NULL) {
        return;
    }

    if (type != RECT && type != CIRC && type != PATH) {
        return;
    }

    if (type == RECT) {
        Rectangle * rect = (Rectangle*)newElement;
        if (rect->width < 0 || rect->height < 0 || rect->otherAttributes == NULL) {
            return;
        }
        insertBack(img->rectangles, rect);
    }
    else if (type == CIRC) {
        Circle * circ = (Circle*)newElement;
        if (circ->r < 0 || circ->otherAttributes == NULL) {
            return;
        }
        insertBack(img->circles, circ);
    }
    else if (type == PATH) {
        Path * path = (Path*)newElement;
        if (path->data == NULL || path->otherAttributes == NULL) {
            return;
        }
        insertBack(img->paths, path);
    }
    else {
        return;
    }
}

// ----------------------------------------------------- A2 Module 3 --------------------------------------------


char * attrToJSON (const Attribute * a) {
    // If argument a is NULL, then give empty attribute as stated
    if (a == NULL) {
        char * empty = malloc(sizeof(char)*3);
        strcpy(empty, "{}");
        return empty;
    }

    char * str;
    char * name = (char*)a->name;
    char * value = (char*)a->value;
    int length;

    // Checking for newline and removing it if found
    if (value[strlen(value) - 1] == '\n') {
        value[strlen(value) - 1] = '\0';
    }

    length = strlen(name) + strlen(value) + 100;
    str = (char*)malloc(sizeof(char) * length);
    sprintf(str, "{\"name\":\"%s\",\"value\":\"%s\"}", name, value);

    return str;
}

char * circleToJSON (const Circle * c) {
    // If empty circle , then give empty circle as stated
    if (c == NULL) {
        char * empty = malloc(sizeof(char)*3);
        strcpy(empty, "{}");
        return empty;
    }

    char * str;
    char unit[1024];
    if (strcmp(c->units, "") == 0) {
        strcpy(unit, "");
    }
    else {
        strcpy(unit, c->units);
    }

    char xVal[1024];
    char yVal[1024];
    char rVal[1024];
    char numAttr[1024]; // Variable will hold the number of attributes
    int length = 0;
    int attVal = 0; // Variable used to call getLength
    attVal = getLength(c->otherAttributes);
    sprintf(numAttr, "%d", attVal); // Converting the integer into a string to be used in sprintf later

    sprintf(xVal, "%.2f", c->cx);
    sprintf(yVal, "%.2f", c->cy);
    sprintf(rVal, "%.2f", c->r);

    length = strlen(unit) + strlen(xVal) + strlen(yVal) + strlen(rVal) + strlen(numAttr) + 100;
    str = (char*)malloc(sizeof(char) * length);
    sprintf(str, "{\"cx\":%s,\"cy\":%s,\"r\":%s,\"numAttr\":%s,\"units\":\"%s\"}", xVal, yVal, rVal, numAttr, unit);

    return str;
}

char * rectToJSON (const Rectangle * r) {
    // If empty rectangle, then give empty Rectangle as stated
    if (r == NULL) {
        char * empty = malloc(sizeof(char)*3);
        strcpy(empty, "{}");
        return empty;
    }

    char * str;
    char unit[1024];
    if (strcmp(r->units, "") == 0) {
        strcpy(unit, "");
    }
    else {
        strcpy(unit, r->units);
    }

    char xVal[1024];
    char yVal[1024];
    char wVal[1024];
    char hVal[1024];
    char numAttr[1024]; // Variable will hold the number of attributes
    int length = 0;
    int attVal = 0; // Variable used to call getLength
    attVal = getLength(r->otherAttributes);
    sprintf(numAttr, "%d", attVal); // Converting the integer into a string to be used in sprintf later

    sprintf(xVal, "%.2f", r->x);
    sprintf(yVal, "%.2f", r->y);
    sprintf(wVal, "%.2f", r->width);
    sprintf(hVal, "%.2f", r->height);

    length = strlen(unit) + strlen(xVal) + strlen(yVal) + strlen(wVal) + strlen(hVal) + strlen(numAttr) + 100;
    str = (char*)malloc(sizeof(char) * length);
    sprintf(str, "{\"x\":%s,\"y\":%s,\"w\":%s,\"h\":%s,\"numAttr\":%s,\"units\":\"%s\"}", xVal, yVal, wVal, hVal, numAttr, unit);

    return str;
}

char* pathToJSON(const Path *p) {
    if(p == NULL || p->otherAttributes == NULL) {
        char *empty = malloc(sizeof(char) * 3);
        strcpy(empty, "{}");
        return empty;
    }

    char data[64];
    snprintf(data, sizeof(data) + 1, p->data);

    int length = strlen(p->data) + 100;
    char *json = malloc(sizeof(char) * (length + 1));
    snprintf(json, length, "{\"d\":\"%s\",\"numAttr\":%d}", data, p->otherAttributes->length);

    return json;
}

char * groupToJSON(const Group * g) {
    // If empty, then create empty JSON string
    if (g == NULL) {
        char * empty = malloc(sizeof(char)*3);
        strcpy(empty, "{}");
        return empty;
    }
    // Initialize variables and then find total number of childs and attributes
    char * str;
    char childStr[1024];
    char attrStr[1024];

    int childCounter = 0;
    int attVal = 0;
    int length = 0;

    childCounter += getLength(g->rectangles);
    childCounter += getLength(g->circles);
    childCounter += getLength(g->paths);
    childCounter += getLength(g->groups);

    attVal += getLength(g->otherAttributes);

    sprintf(childStr, "%d", childCounter);
    sprintf(attrStr, "%d", attVal);

    // Allocating space for the string and then saving it into str, after that return it
    length = strlen(childStr) + strlen(attrStr) + 100;
    str = (char*)malloc(sizeof(char) * length);
    sprintf(str, "{\"children\":%s,\"numAttr\":%s}", childStr, attrStr);

    return str;
}

char * SVGtoJSON (const SVG * image) {
    // If svg is NULL then create empty JSON string
    if (image == NULL) {
        char * empty = malloc(sizeof(char)*3);
        strcpy(empty, "{}");
        return empty;
    }

    int length = 0;

    char * str;
    char rectStr[1024];
    char circStr[1024];
    char pathStr[1024];
    char grpStr[1024];

    // Create the Lists for the svg struct and then find the length of each
    List * rects = getRects(image);
    List * circs = getCircles(image);
    List * paths = getPaths(image);
    List * groups = getGroups(image);

    sprintf(rectStr, "%d", getLength(rects));
    sprintf(circStr, "%d", getLength(circs));
    sprintf(pathStr, "%d", getLength(paths));
    sprintf(grpStr, "%d", getLength(groups));

    // Allocate space and then save the string desired into str, which is returned
    length = strlen(rectStr) + strlen(circStr) + strlen(pathStr) + strlen(grpStr) + 100;
    str = (char*)malloc(sizeof(char) * length);
    sprintf(str, "{\"numRect\":%s,\"numCirc\":%s,\"numPaths\":%s,\"numGroups\":%s}", rectStr, circStr, pathStr, grpStr);

    freeList(rects);
    freeList(circs);
    freeList(paths);
    freeList(groups);

    return str;
}

char * attrListToJSON (const List * list) {
    // If list is NULL or empty then create empty List
    if (list == NULL || getLength((List*)list) == 0) {
        char * empty = malloc(sizeof(char) * 3);
        strcpy(empty, "[]");
        return empty;
    }

    char * str = (char*)malloc(sizeof(char)*2);
    strcpy(str, "[");

    void * attrElement;
    ListIterator attrIterator = createIterator((List*)list);
    // Traverse the list and iteratively add concatenate onto the string, the new attributes
    while ((attrElement = nextElement(&attrIterator)) != NULL) {
        Attribute * attr = (Attribute*)attrElement;

        char * attrString = attrToJSON(attr);
        
        str = realloc(str, strlen(str) + 1 + strlen(attrString) + 1 + 2);

        strcat(str, attrString);
        strcat(str, ",");

        free(attrString);
    }

    // Changing the final comma from the loop, to an ']' to end the string
    str[strlen(str) - 1] = ']';

    return (str);
}

char * circListToJSON(const List *list) {
    // If list is NULL or empty, create empty list
    if (list == NULL || getLength((List*)list) == 0) {
        char * empty = malloc(sizeof(char) * 3);
        strcpy(empty, "[]");
        return empty;
    }

    char * str = malloc(2);
    strcpy(str, "[");

    void * circElement;
    ListIterator circIterator = createIterator((List*)list);
    // Traverse list and iteratively concatenate on the string
    while ((circElement = nextElement(&circIterator)) != NULL) {
        Circle * circ = (Circle*)circElement;

        char * circString = circleToJSON(circ);
        
        str = realloc(str, strlen(str) + 1 + strlen(circString) + 1 + 2);

        strcat(str, circString);
        strcat(str, ",");

        free(circString);
    }

    // Changing the final comma from the loop, to an ']' to end the string
    str[strlen(str) - 1] = ']';

    return (str);
}

char * rectListToJSON(const List *list) {
    // If List is NULL then create empty list
    if (list == NULL || getLength((List*)list) == 0) {
        char * empty = malloc(sizeof(char) * 3);
        strcpy(empty, "[]");
        return empty;
    }

    char * str = malloc(2);
    strcpy(str, "[");

    void * rectElement;
    ListIterator rectIterator = createIterator((List*)list);
    // Traverse and iteratively concatenate the List
    while ((rectElement = nextElement(&rectIterator)) != NULL) {
        Rectangle * rect = (Rectangle*)rectElement;

        char * rectString = rectToJSON(rect);
        
        str = realloc(str, strlen(str) + 1 + strlen(rectString) + 1 + 2);

        strcat(str, rectString);
        strcat(str, ",");

        free(rectString);
    }

    // Changing the final comma from the loop, to an ']' to end the string
    str[strlen(str) - 1] = ']';

    return (str);
}

char* pathListToJSON(const List *list) {
    if(list == NULL || list->length < 1) {
        char *empty = malloc(sizeof(char) * 3);
        strcpy(empty, "[]");
        return empty;
    }

    int length = 2;
    char *json = malloc(sizeof(char) * length);
    strcpy(json, "[");

    Node *cur = list->head;
    
    while(cur) {
        Path* path = (Path*)cur->data;
        char *pathJson = pathToJSON(path);
        length += strlen(pathJson) + 2;

        json = realloc(json, sizeof(char) * length);
        strcat(json, pathJson);
        if(cur->next != NULL) strcat(json, ",");
        
        free(pathJson);
        cur = cur->next;
    }

    json = realloc(json, sizeof(char) * length + 1);
    strcat(json, "]");
    
    return json;
}

char * groupListToJSON(const List *list) {
    // If List is NULL or empty, then return empty list
    if (list == NULL || getLength((List*)list) == 0) {
        char * empty = malloc(sizeof(char) * 3);
        strcpy(empty, "[]");
        return empty;
    }

    char * str = malloc(2);
    strcpy(str, "[");

    void * grpElement;
    ListIterator grpIterator = createIterator((List*)list);
    // Traverse list and iteratively concatenate the string
    while ((grpElement = nextElement(&grpIterator)) != NULL) {
        Group * grp = (Group*)grpElement;

        char * grpString = groupToJSON(grp);
        
        str = realloc(str, strlen(str) + 1 + strlen(grpString) + 1 + 2);

        strcat(str, grpString);
        strcat(str, ",");

        free(grpString);
    }

    // Changing the final comma from the loop, to an ']' to end the string
    str[strlen(str) - 1] = ']';

    return (str);
}

// ----------------------------------------------- A2 Module 3 Bonus Functions ----------------------------------------------

SVG * JSONtoSVG (const char * svgString) {
    if (svgString == NULL) {
        return NULL;
    }

    // Creating the new SVG
    SVG * newSVG = (SVG*)malloc(sizeof(SVG));

    char * title = malloc(1);
    char * descr = malloc(1);

    // Converting the JSON string into strings that are readable
    char * tempTitle = strchr(svgString, ':');
    char * tempDescr = strrchr(tempTitle, ':');

    // Allocate memory and copy the string into title and descr
    int size = 0;
    for (int i = 2; tempTitle[i] != '\"'; i++) {
        size++;
        title = (char*)realloc(title, size + 1);
        title[size - 1] = tempTitle[i];
    }

    title[size] = '\0';

    size = 0;
    for (int i = 2; tempDescr[i] != '\"'; i++) {
        size++;
        descr = (char*)realloc(descr, size + 1);
        descr[size - 1] = tempDescr[i];
    }
    descr[size] = '\0';

    // After getting the title and descr, then copy it into the SVG struct and then intitialize its lists

    strcpy(newSVG->namespace, "http://www.w3.org/2000/svg");
    strcpy(newSVG->title, title);
    strcpy(newSVG->description, descr);

    newSVG->rectangles = initializeList(&rectangleToString, &deleteRectangle, &compareRectangles);
    newSVG->circles = initializeList(&circleToString, &deleteCircle, &compareCircles);
    newSVG->paths = initializeList(&pathToString, &deletePath, &comparePaths);
    newSVG->groups = initializeList(&groupToString, &deleteGroup, &compareGroups);
    newSVG->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);

    // Freeing memory
    free(title);
    free(descr);

    return newSVG;
}

Rectangle * JSONtoRect (const char * svgString) {
    if (svgString == NULL) {
        return NULL;
    }

    // Creating the Rectangle to copy the JSON and also initializing variables
    Rectangle * r = (Rectangle*)malloc(sizeof(Rectangle));

    int counter = 0;
    int size = 0;
    char * units = malloc(1);
    char * xVal = malloc(1);
    char * yVal = malloc(1);
    char * wVal = malloc(1);
    char * hVal = malloc(1);

    char tempX[1024];
    char tempY[1024];
    char tempW[1024];
    char tempH[1024];
    char tempUnits[1024];

    // Loop to copy the string in order to parse it later
    char * val = strstr(svgString, ":");
    while (val) {
        if (counter == 0) {
            strcpy(tempX, val);
        }
        else if (counter == 1) {
            strcpy(tempY, val);
        }
        else if (counter == 2) {
            strcpy(tempW, val);
        }
        else if (counter == 3) {
            strcpy(tempH, val);
        }
        else if (counter == 4) {
            strcpy(tempUnits, val);
        }
        counter++;
        val = strstr(val + 1, ":");
    }

    // After getting each attribute's string, then copy them into respective variables
    size = 0;
    for (int i = 1; tempX[i] != ','; i++) {
        size++;
        xVal = (char*)realloc(xVal, size + 1);
        xVal[size - 1] = tempX[i];
    }
    xVal[size] = '\0';

    size = 0;
    for (int i = 1; tempY[i] != ','; i++) {
        size++;
        yVal = (char*)realloc(yVal, size + 1);
        yVal[size - 1] = tempY[i];
    }
    yVal[size] = '\0';

    size = 0;
    for (int i = 1; tempW[i] != ','; i++) {
        size++;
        wVal = (char*)realloc(wVal, size + 1);
        wVal[size - 1] = tempW[i];
    }
    wVal[size] = '\0';

    size = 0;
    for (int i = 1; tempH[i] != ','; i++) {
        size++;
        hVal = (char*)realloc(hVal, size + 1);
        hVal[size - 1] = tempH[i];
    }
    hVal[size] = '\0';

    size = 0;
    for (int i = 2; tempUnits[i] != '\"'; i++) {
        size++;
        units = (char*)realloc(units, size + 1);
        units[size - 1] = tempUnits[i];
    }
    units[size] = '\0';

    //After they are parsed correctly, place them into the Rectangle struct and then intiialize the list for otherAttributes
    r->x = atof(xVal);
    r->y = atof(yVal);
    r->width = atof(wVal);
    r->height = atof(hVal);
    strcpy(r->units, units);
    r->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);

    // Free memory and return
    free(xVal);
    free(yVal);
    free(wVal);
    free(hVal);
    free(units);
    free(val);
    
    return r;
}

Circle * JSONtoCircle (const char * svgString) {
    if (svgString == NULL) {
        return NULL;
    }

    // Create the circle to copy the JSON and initialize variables
    Circle * c = (Circle*)malloc(sizeof(Circle));

    int counter = 0;
    int size = 0;
    char * units = malloc(1);
    char * cxVal = malloc(1);
    char * cyVal = malloc(1);
    char * rVal = malloc(1);

    char tempX[1024];
    char tempY[1024];
    char tempR[1024];
    char tempUnits[1024];

    // Copy the strings into their respective variables
    char * val = strstr(svgString, ":");
    while (val) {
        if (counter == 0) {
            strcpy(tempX, val);
        }
        else if (counter == 1) {
            strcpy(tempY, val);
        }
        else if (counter == 2) {
            strcpy(tempR, val);
        }
        else if (counter == 3) {
            strcpy(tempUnits, val);
        }
        counter++;
        val = strstr(val + 1, ":");
    }

    // After copied, then parse them to remove misc characters
    size = 0;
    for (int i = 1; tempX[i] != ','; i++) {
        size++;
        cxVal = (char*)realloc(cxVal, size + 1);
        cxVal[size - 1] = tempX[i];
    }
    cxVal[size] = '\0';

    size = 0;
    for (int i = 1; tempY[i] != ','; i++) {
        size++;
        cyVal = (char*)realloc(cyVal, size + 1);
        cyVal[size - 1] = tempY[i];
    }
    cyVal[size] = '\0';

    size = 0;
    for (int i = 1; tempR[i] != ','; i++) {
        size++;
        rVal = (char*)realloc(rVal, size + 1);
        rVal[size - 1] = tempR[i];
    }
    rVal[size] = '\0';

    size = 0;
    for (int i = 2; tempUnits[i] != '\"'; i++) {
        size++;
        units = (char*)realloc(units, size + 1);
        units[size - 1] = tempUnits[i];
    }
    units[size] = '\0';

    // After parsing, copy them into Circle struct attributes and initialize the List
    c->cx = atof(cxVal);
    c->cy = atof(cyVal);
    c->r = atof(rVal);
    strcpy(c->units, units);
    c->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);

    // Free the memory and then return
    free(cxVal);
    free(cyVal);
    free(rVal);
    free(units);
    free(val);
    
    return c;
}
#endif