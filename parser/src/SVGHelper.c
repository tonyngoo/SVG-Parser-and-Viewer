#ifndef SVGHELPER_FILE
#define SVGHELPER_FILE


#include <ctype.h>
#include "SVGParser.h"
#include "SVGHelper.h"
#include "LinkedListAPI.h"
#include <strings.h>
#include <string.h>
#include <math.h>

//Helper function which is the main parser used for createSVG
void parseXMLTree (SVG * newSVG, xmlNode *root_element) {
    //Loop through all of the nodes inside the SVG
    for (xmlNode *node = root_element; node != NULL; node = node->next) {
        if (node->type == XML_ELEMENT_NODE) {
            //If the node element contains the SVG tag, then copy it into the new SVG namespace
            if (strcasecmp((char*)node->name, "svg") == 0) {
                char * namespace = malloc(xmlStrlen(node->ns->href) * (sizeof(char) + 1));
                strcpy(namespace, (char*)node->ns->href);
                //Error checking if the namespace exceeds the 256 character limit, truncate if exceeded
                if (strlen(namespace) > 256) {
                    namespace[255] = '\0';
                }
                //Copy into the SVG struct and free the namespace allocated
                strcpy(newSVG->namespace, namespace);

                xmlAttr* attribute = NULL;
                attribute = node->properties;

                //Search for all attributes and store it in otherAttributes of the SVG struct
                while(attribute) {
                    const char* attrName = (char*)attribute->name;
                    char* attrValue = (char*)xmlNodeListGetString(node->doc, attribute->children, 1);
                    Attribute *otherAttr = malloc(sizeof(Attribute) + strlen(attrValue) * sizeof(char) + 1);
                    otherAttr->name = malloc(strlen(attrName) * sizeof(char) + 1);


                    strcpy(otherAttr->name, attrName);
                    strcpy(otherAttr->value, attrValue);
                    
                    insertBack(newSVG->otherAttributes, otherAttr);
                    attribute = attribute->next;
                    free(attrValue);
                }
                free(namespace);
            }
            //If the tag name is title
            else if (strcasecmp((char*)node->name, "title") == 0) {
                char* title = malloc(sizeof(char) * (xmlStrlen(node->children->content) + 1));
                strcpy(title, (char*)node->children->content);
                if (strlen(title) > 256) {
                    title[255] = '\0';
                }
                strcpy(newSVG->title, title);
                free(title);
            }
            //If the tag name is for the SVG description
            else if (strcasecmp((char*)node->name, "desc") == 0) {
                //Allocate space to hold the description content and get the content using xmlNodeGetContent()
                char* desc = malloc(sizeof(char) * (xmlStrlen(node->children->content) + 1));
                strcpy(desc, (char*)node->children->content);
                if (strlen(desc) > 256) {
                    desc[255] = '\0';
                }
                strcpy(newSVG->description, desc);
                free(desc);
            }
            //If the tag name is for "rect" or rectangle
            else if (strcasecmp((char*)node->name, "rect") == 0) {
                insertBack(newSVG->rectangles, createRect(node));
            }
            //If the tag name is for "circle"
            else if (strcasecmp((char*)node->name, "circle") == 0) {
                insertBack(newSVG->circles, createCircle(node));
            }
            //If the tag name is for "path"
            else if (strcasecmp((char*)node->name, "path") == 0) {
                insertBack(newSVG->paths, createPath(node));
            }
            //If the tag name is for "g" or group
            else if (strcasecmp((char*)node->name, "g") == 0) {
                Group * newGroup = malloc(sizeof(Group));
                //Initializing lists in the Group struct

                newGroup->rectangles = initializeList(&rectangleToString, &deleteRectangle, &compareRectangles);
                newGroup->circles = initializeList(&circleToString, &deleteCircle, &compareCircles);
                newGroup->paths = initializeList(&pathToString, &deletePath, &comparePaths);
                newGroup->groups = initializeList(&groupToString, &deleteGroup, &compareGroups);
                newGroup->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
                xmlAttr* attribute = NULL;
                attribute = node->properties;

                while(attribute) { 
                    const char* attrName = (char*)attribute->name;
                    char* attrValue = (char*)xmlNodeListGetString(node->doc, attribute->children, 1);

                    Attribute *otherAttr = malloc(sizeof(Attribute) + strlen(attrValue) * sizeof(char) + 1);

                    //Allocate space for the elements inside Attribute struct
                            
                    otherAttr->name = malloc(strlen(attrName) * sizeof(char) + 1);

                    strcpy(otherAttr->name, "");

                    //Try to malloc for the ->name to attrName
                    strcpy(otherAttr->name, (char*)attrName);
                    strcpy(otherAttr->value, (char*)attrValue);

                    insertBack(newGroup->otherAttributes, otherAttr);
                    attribute = attribute->next;
                    free(attrValue);
                }
                insertBack(newSVG->groups, createGroup(newGroup, node->children));
                continue;
            }
        }
        parseXMLTree(newSVG, node->children);
    }
}

//Function that creates a Rectangle list and returns it so it can be inserted 
Rectangle * createRect (xmlNode * node) {
    Rectangle *newRect = malloc(sizeof(Rectangle));

    newRect->x = 0;
    newRect->y = 0;
    newRect->width = 0;
    newRect->height = 0;
    strcpy(newRect->units, "");
    newRect->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);

    xmlAttr* attribute = NULL;
    attribute = node->properties;

    while(attribute) { 
        const char* attrName = (char*)attribute->name;
        char* attrValue = (char*)xmlNodeListGetString(node->doc, attribute->children, 1);
        if (strcasecmp((char*)attrName, "x") == 0) {
            //If it contains units, then find the units and remove the unit from the value and then put into newRect
            if (containsUnits(attrValue)) {
                strcpy(newRect->units, findUnits(attrValue));
                removeUnits(attrValue);
                newRect->x = atof(attrValue);
            }
            else if (!containsUnits(attrValue)) {
                newRect->x = atof(attrValue);
            }
        }
        else if (strcasecmp((char*)attrName, "y") == 0) {
            //If it contains units, then find the units and remove the unit from the value and then put into newRect
            if (containsUnits(attrValue)) {
                strcpy(newRect->units, findUnits(attrValue));
                removeUnits(attrValue);
                newRect->y = atof(attrValue);
            }
            else if (!containsUnits(attrValue)) {
                newRect->y = atof(attrValue);
            }
        }
        else if (strcasecmp((char*)attrName, "width") == 0) {
            //If it contains units, then find the units and remove the unit from the value and then put into newRect
            if (containsUnits(attrValue)) {
                strcpy(newRect->units, findUnits(attrValue));
                removeUnits(attrValue);
                newRect->width = atof(attrValue);
            }
            else if (!containsUnits(attrValue)) {
                newRect->width = atof(attrValue);
            }
        }
        else if (strcasecmp((char*)attrName, "height") == 0) {
            //If it contains units, then find the units and remove the unit from the value and then put into newRect
            if (containsUnits(attrValue)) {
                strcpy(newRect->units, findUnits(attrValue));
                removeUnits(attrValue);
                newRect->height = atof(attrValue);
            }
            else if (!containsUnits(attrValue)) {
                newRect->height = atof(attrValue);
            }
        }
        //If the attribute name is not part of the Rectangle struct, then insert as otherAttribute
        else {
            Attribute *otherRectAttr = malloc(sizeof(Attribute) + strlen(attrValue) * sizeof(char) + 1);
            otherRectAttr->name = malloc(strlen(attrName) * sizeof(char) + 1);

            strcpy(otherRectAttr->name, attrName);
            strcpy(otherRectAttr->value, attrValue);
            
            insertBack(newRect->otherAttributes, otherRectAttr);
        }
        attribute = attribute->next;
        free(attrValue);
    }
    return (newRect);
}

//Function that creates a Circle list which is returned so it can be inserted afterwards
Circle * createCircle (xmlNode * node) {
    Circle *newCircle = malloc(sizeof(Circle));
                
    newCircle->cx = 0;
    newCircle->cy = 0;
    newCircle->r = 0;
    strcpy(newCircle->units, "");
    newCircle->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);

    xmlAttr* attribute = node->properties;

    //Traversing through all the circle's attributes
    while(attribute) { 
        const char* attrName = (char*)attribute->name;
        char* attrValue = (char*)xmlNodeListGetString(node->doc, attribute->children, 1);

        //If attribute is cx, then check for units and then store afterwards
        if (strcasecmp((char*)attrName, "cx") == 0) {
            if (containsUnits(attrValue)) {
                strcpy(newCircle->units, findUnits(attrValue));
                removeUnits(attrValue);
                newCircle->cx = atof(attrValue);
            }
            else if (!containsUnits(attrValue)) {
                newCircle->cx = atof(attrValue);
            }
        }
        //If attribute is cy, then check for units and then store it afterwards
        else if (strcasecmp((char*)attrName, "cy") == 0) {
            if (containsUnits(attrValue)) {
                strcpy(newCircle->units, findUnits(attrValue));
                removeUnits(attrValue);
                newCircle->cy = atof(attrValue);
            }
            else if (!containsUnits(attrValue)) {
                newCircle->cy = atof(attrValue);
            }
        }
        //If attribute is r, then check for units and then store it afterwards
        else if (strcasecmp((char*)attrName, "r") == 0) {
            if (containsUnits(attrValue)) {
                strcpy(newCircle->units, findUnits(attrValue));
                removeUnits(attrValue);
                newCircle->r = atof(attrValue);
            }
            else if (!containsUnits(attrValue)) {
                newCircle->r = atof(attrValue);
            }
        }
        //If attribute is not inside Circle struct, then insert as an otherAttribute
        else {
            Attribute *otherCircAttr = malloc(sizeof(Attribute) + strlen(attrValue) * sizeof(char) + 1);

            //Allocate space for the elements inside Attribute struct
                    
            otherCircAttr->name = malloc(strlen(attrName) * sizeof(char) + 1);

            strcpy(otherCircAttr->name, "");

            //Copying the name and values into the struct otherAttributes and then insert into its List
            strcpy(otherCircAttr->name, (char*)attrName);
            strcpy(otherCircAttr->value, (char*)attrValue);

            insertBack(newCircle->otherAttributes, otherCircAttr);
        }
        attribute = attribute->next;
        free(attrValue);
    }
    return (newCircle);
}

//Function that creates a Path and then returns it so it can be inserted
Path * createPath (xmlNode * node) {
    xmlAttr* attribute = node->properties;
    Path *newPath = malloc(sizeof(Path) + (strlen((char*)attribute->children->content) + 1) * sizeof(char));

    newPath->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);

    //Traversing through the Path attributes
    while(attribute) { 
        const char* attrName = (char*)attribute->name;
        char* attrValue = (char*)xmlNodeListGetString(node->doc, attribute->children, 1);

        //If attribute is data, then store it
        if (strcasecmp((char*)attrName, "d") == 0) {
            newPath = realloc(newPath, sizeof(Path) + (strlen((char*)attribute->children->content) + 1) * sizeof(char));
            strcpy(newPath->data, (char*)attrValue);
        }
        //If attribute is not data, then store it as otherAttribute
        else {
            Attribute *otherPathAttr = malloc(sizeof(Attribute) + strlen(attrValue) * sizeof(char) + 1);
            otherPathAttr->name = malloc(strlen(attrName) * sizeof(char) + 1);

            strcpy(otherPathAttr->name, (char*)attrName);
            strcpy(otherPathAttr->value, (char*)attrValue);

            insertBack(newPath->otherAttributes, otherPathAttr);

        }
        attribute = attribute->next;
        free(attrValue);
    }
    return (newPath);
}

//Helper function to create a group List and return it so it can be inserted back
Group * createGroup (Group * grp, xmlNode * group_node) {
    //Loop through all of the nodes inside the Group
    for (xmlNode *node = group_node; node != NULL; node = node->next) {
        if (node->type == XML_ELEMENT_NODE) {
            //If the tag name is for "rect" or rectangle
            if (strcasecmp((char*)node->name, "rect") == 0) {
                insertBack(grp->rectangles, createRect(node));
            }
            //If the tag name is for "circle"
            else if (strcasecmp((char*)node->name, "circle") == 0) {
                insertBack(grp->circles, createCircle(node));
            }
            //If the tag name is for "path"
            else if (strcasecmp((char*)node->name, "path") == 0) {
                insertBack(grp->paths, createPath(node));
            }
            //If the tag name is for "g" or groups
            else if (strcasecmp((char*)node->name, "g") == 0) {
                //Create a new group which will be inserted as the groups->group
                Group * newGroup = malloc(sizeof(Group));
                //Initializing lists in the Group struct

                newGroup->rectangles = initializeList(&rectangleToString, &deleteRectangle, &compareRectangles);
                newGroup->circles = initializeList(&circleToString, &deleteCircle, &compareCircles);
                newGroup->paths = initializeList(&pathToString, &deletePath, &comparePaths);
                newGroup->groups = initializeList(&groupToString, &deleteGroup, &compareGroups);
                newGroup->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
                xmlAttr* attribute = NULL;
                attribute = node->properties;

                //Storing other attributes
                while(attribute) { 
                    const char* attrName = (char*)attribute->name;
                    char* attrValue = (char*)xmlNodeListGetString(node->doc, attribute->children, 1);

                    Attribute *otherAttr = malloc(sizeof(Attribute) + strlen(attrValue) * sizeof(char) + 1);

                    //Allocate space for the elements inside Attribute struct
                            
                    otherAttr->name = malloc(strlen(attrName) * sizeof(char) + 1);

                    strcpy(otherAttr->name, "");

                    //Copying the name and value for otherAttributes
                    strcpy(otherAttr->name, (char*)attrName);
                    strcpy(otherAttr->value, (char*)attrValue);

                    insertBack(newGroup->otherAttributes, otherAttr);
                    attribute = attribute->next;
                    free(attrValue);
                }
                insertBack(grp->groups, createGroup(newGroup, node->children));
                continue;
            }
        }
    }
    return (grp);
}

//Helper function to remove the units from given string
void removeUnits (char *string) {
	int length = 0, i = 0, j = 0, k = 0;

	length = strlen(string);

	for(i = 0; i < length; i++) {
		for(j = 0; j < length; j++) {
			if((string[j] >= 'a' && string[j] <= 'z') || (string[j] >= 'A' && string[j] <= 'Z')) {
				for(k = j; k < length; k++) {
					string[k] = string[k + 1];
				}
				length--;
			}
		}
	}
}

//Helper function to find the units and to return it to be stored in the shape struct
char * findUnits (char *string) {
    /* The supported length unit identifiers in SVG are: 
    /  em, ex, px, pt, pc, cm, mm, in, and percentages.
    */
    if (strstr(string, "em") != NULL) {
        return ("em");
    }
    else if (strstr(string, "ex") != NULL) {
        return ("ex");
    }
    else if (strstr(string, "px") != NULL) {
        return ("px");
    }
    else if (strstr(string, "pt") != NULL) {
        return ("pt");
    }
    else if (strstr(string, "pc") != NULL) {
        return ("pc");
    }
    else if (strstr(string, "cm") != NULL) {
        return ("cm");
    }
    else if (strstr(string, "mm") != NULL) {
        return ("mm");
    }
    else if (strstr(string, "%%") != NULL) {
        return ("%%");
    }
    else {
        return ("");
    }
}

//Helper function to check if the shape attributes contain units
bool containsUnits (char* attrValue) {
    //Loop through the attribute string and check if alphabetical
    for (int i = 0; i < strlen(attrValue); i++) {
        //If a unit is found, then returns true
        if (isalpha(attrValue[i]) != 0) {
            return 1;
        }
    }
    //If finished looping without returning, then it contains no units
    return 0;
}

//Helper function which checks if the value is positive or negative
bool isPositive(float value) {
    if (value >= 0) {
        return 1;
    }
    return 0;    
}


// ----------------------------------------------------- Module 2 ------------------------------------------------

//Helper function to recursively search for all rectangles inside groups
void getGrpRects (Group * grp, List * list) {
    //Return back to getRects if list or group is null (an error)
    if (grp == NULL || list == NULL) {
        return;
    }

    //Set the node to the groups rectangle
    Node * node = grp->rectangles->head;

    //Traverse through the rectangles and insert them into the list
    while (node != NULL) {
        Rectangle * rect = (Rectangle*)node->data;
        insertBack(list, rect);
        node = node->next;
    }

    //Create another node to search for more groups inside the grp
    Node * otherNode = grp->groups->head;

    //Traverse and recursively call if an inner group exists
    while (otherNode != NULL) {
        Group * rectGrp = otherNode->data;
        getGrpRects(rectGrp, list);
        otherNode = otherNode->next;
    }
}

//Helper function to recursively call for all circles inside groups
void getGrpCircs (Group * grp, List * list) {
    //If the group or list is null, then return back to getCircles (an error)
    if (grp == NULL || list == NULL) {
        return;
    }

    //Create a node to search through circles
    Node * node = grp->circles->head;

    //Traverse through circles and insert them into the list
    while (node != NULL) {
        Circle * circ = (Circle*)node->data;
        insertBack(list, circ);
        node = node->next;
    }

    //Create a node to search recursively through groups
    Node * otherNode = grp->groups->head;

    //Traverse through groups and recursively call the function if inner group exists
    while (otherNode != NULL) {
        Group * circGrp = otherNode->data;
        getGrpCircs(circGrp, list);
        otherNode = otherNode->next;
    }
}

//Helper function which will recursively call to find all Paths inside groups
void getGrpPaths (Group * grp, List * list) {
    //If group or list is null then return back to getPaths (an error occurred)
    if (grp == NULL || list == NULL) {
        return;
    }

    //Create a node to search through all paths in grp
    Node * node = grp->paths->head;

    //Traverse through all Path inside grp and insert them into list
    while (node != NULL) {
        Path * paths = (Path*)node->data;
        insertBack(list, paths);
        node = node->next;
    }

    //Create a node to search for inner groups
    Node * otherNode = grp->groups->head;

    //Traverse and recursively call if an inner group exists
    while (otherNode != NULL) {
        Group * pathGrp = otherNode->data;
        getGrpPaths(pathGrp, list);
        otherNode = otherNode->next;
    }
}

//Helper function which will recursively call to find all Groups inside grp
void getGrpGroups(Group * grp, List * list) {
    //If grp or list is null, then return (an error occurred or they are empty)
    if (grp == NULL || list == NULL) {
        return;
    }

    //Create a node to search through inner grooups
    Node * node = grp->groups->head;

    //Traverse and recursively call if inner groups exist
    while (node != NULL) {
        Group * group = (Group*)node->data;
        insertBack(list, group);
        getGrpGroups(group, list);
        node = node->next;
    }
}

//A dummy delete function which is used inside of the getShape() functions, to avoid freeList() memory leaks and errors
void dummyDelete(void* dummy) {
}

// --------------------------------------------------- Module 2 Helper Functions -------------------------------------------

bool validateXMLSchema(xmlDoc *doc, const char *schemaFile) {
    // Error checking the Schema file name
    if (schemaFile == NULL || (strcmp(schemaFile, "") == 0)) {
        fprintf(stderr, "ERROR: Empty/NULL Schema File Name\n");
        return(FALSE);
    }

    if (doc == NULL) {
        fprintf(stderr, "ERROR: Invalid xml Tree\n");
        return(FALSE);
    }

    // Variables for Schema File
    xmlSchemaPtr schemaPtr = NULL;
    xmlSchemaParserCtxtPtr contextPtr;

    // Enabling line numbers in elements contents
    xmlLineNumbersDefault(1);

    // Creating an XML Schemas parse context pointer to validate the XML file and setting callback functions for errors in the context pointer
    contextPtr = xmlSchemaNewParserCtxt(schemaFile);
    xmlSchemaSetParserErrors(contextPtr, (xmlSchemaValidityErrorFunc)fprintf, (xmlSchemaValidityWarningFunc)fprintf, stderr);
    
    // If the contextPtr is NULL, xmlSchemaNewParserCtxt meaning an invalid Schema file
    if (contextPtr == NULL) {
        xmlSchemaFreeParserCtxt(contextPtr);
        xmlSchemaCleanupTypes();
        return(FALSE);
    }

    // Building an XML Schema structure storing inside schemaPtr to validate the XML file
    schemaPtr = xmlSchemaParse(contextPtr);
    // Freeing the context pointer
    xmlSchemaFreeParserCtxt(contextPtr);

    // Variables used to validate the XML file
    int validationReturnValue = 0;
    xmlSchemaValidCtxtPtr validContextPointer = xmlSchemaNewValidCtxt(schemaPtr);

    // Setting callback errors and warnings in the validContext pointer
    xmlSchemaSetValidErrors(validContextPointer, (xmlSchemaValidityErrorFunc)fprintf, (xmlSchemaValidityWarningFunc)fprintf, stderr);
    // Setting the return value to be the validation between the xmlDoc pointer and validContextPointer
    validationReturnValue = xmlSchemaValidateDoc(validContextPointer, doc);

    // If the SchemaPtr isn't NULL, frees the pointer
    if (schemaPtr != NULL) {
        xmlSchemaFree(schemaPtr);
    }

    // Checks to make sure validation worked, any value other than 0 means the validation failed
    if (validationReturnValue != 0) {
        fprintf(stderr, "xmlTree failed to validate with Schema file: %s\n", schemaFile);
        xmlSchemaCleanupTypes();
        xmlSchemaFreeValidCtxt(validContextPointer);
        return(FALSE);
    }

    // Freeing the valid context pointer, cleaning the Schema and XML parsers and dumping the memory
    xmlSchemaCleanupTypes();
    xmlSchemaFreeValidCtxt(validContextPointer);

    // Returns TRUE for a valid xmlTree
    return(TRUE);
}

xmlDoc * SVGtoXML (const SVG * img) {
    if (img == NULL) {
        return NULL;
    }

    if (img->rectangles == NULL || img->circles == NULL || img->paths == NULL || img->groups == NULL || img->otherAttributes == NULL) {
        return NULL;
    }

    xmlDocPtr doc = NULL;
    xmlNodePtr root_node = NULL;

    doc = xmlNewDoc(BAD_CAST "1.0");
    root_node = xmlNewNode(NULL, BAD_CAST "svg");
    xmlDocSetRootElement(doc, root_node);

    // Create namespace and add it as an attribute of the SVG
    xmlNs * nsPtr = xmlNewNs(root_node, BAD_CAST img->namespace, NULL);
    xmlSetNs(root_node, nsPtr);

    // Set title and description inside the SVG node
    if (strcmp(img->title, "") != 0) {
        xmlNewChild(root_node, NULL, BAD_CAST "title", BAD_CAST img->title);
    }
    if (strcmp(img->description, "") != 0) {
        xmlNewChild(root_node, NULL, BAD_CAST "desc", BAD_CAST img->description);
    }
    
    addListOfOtherAttrToParent(root_node, img->otherAttributes);

    addRectToNode(root_node, img->rectangles, "rect");
    addCircleToNode(root_node, img->circles, "circle");
    addPathToNode(root_node, img->paths, "path");
    addGroupToNode(root_node, img->groups);
    
    return (doc);
}

void addRectToNode(xmlNodePtr parent_node, List * list, char *nodeName) {

    // If list length is empty, then just ignore this function
    if (list->length <= 0) {
        return;
    }

    // Traversing the list of Rectangles in the Rectangles List
    void *rectElement;
    ListIterator rectIterator = createIterator(list);
    while ((rectElement = nextElement(&rectIterator)) != NULL) {

        // Creating a Rectangle node for the current Rectangle struct and making it a child of the parent node
        xmlNodePtr rect_node = xmlNewChild(parent_node, NULL, BAD_CAST nodeName, BAD_CAST "");

        // Getting the Rectangle struct for the current Rectangle element in the list of Rectangles
        Rectangle *rect = (Rectangle*)rectElement;

        // Creating variables to hold the x and y values in the Rectangle struct
        char x[1000] = "";
        char y[1000] = "";
        char width[1000] = "";
        char height[1000] = "";
        char units[256] = "";

        sprintf(x, "%f", rect -> x);
        sprintf(y, "%f", rect -> y);
        sprintf(width, "%f", rect -> width);
        sprintf(height, "%f", rect -> height);
        sprintf(units, "%s", rect -> units);

        if (strcmp(units, "") != 0) {
            strcat(x, units);
            strcat(y, units);
            strcat(width, units);
            strcat(height, units);
        }

        // Setting the x and y values to attributes in the Rectangle node
        xmlNewProp(rect_node, BAD_CAST "x", BAD_CAST x);
        xmlNewProp(rect_node, BAD_CAST "y", BAD_CAST y);
        xmlNewProp(rect_node, BAD_CAST "width", BAD_CAST width);
        xmlNewProp(rect_node, BAD_CAST "height", BAD_CAST height);

        // Adds the list of otherAttribute in the rect to the rect_node
        addListOfOtherAttrToParent(rect_node, rect -> otherAttributes);
    }
}

void addCircleToNode(xmlNodePtr parent_node, List * list, char *nodeName) {

    // If list length is empty, then just ignore this function
    if (list->length <= 0) {
        return;
    }

    // Traversing the list of Circle in the Circle List
    void *circElement;
    ListIterator circIterator = createIterator(list);
    while ((circElement = nextElement(&circIterator)) != NULL) {

        // Creating a Circle node for the current Circle struct and making it a child of the parent node
        xmlNodePtr circ_node = xmlNewChild(parent_node, NULL, BAD_CAST nodeName, BAD_CAST "");

        // Getting the Circle struct for the current Circle element in the list of Circles
        Circle *circ = (Circle*)circElement;

        // Creating variables to hold the cx, cy and r values in the Circle struct
        char cx[1000] = "";
        char cy[1000] = "";
        char r[1000] = "";
        char units[256] = "";

        sprintf(cx, "%f", circ -> cx);
        sprintf(cy, "%f", circ -> cy);
        sprintf(r, "%f", circ -> r);
        sprintf(units, "%s", circ -> units);

        if (strcmp(units, "") != 0) {
            strcat(cx, units);
            strcat(cy, units);
            strcat(r, units);
        }

        // Setting the x and y values to attributes in the Circle node
        xmlNewProp(circ_node, BAD_CAST "cx", BAD_CAST cx);
        xmlNewProp(circ_node, BAD_CAST "cy", BAD_CAST cy);
        xmlNewProp(circ_node, BAD_CAST "r", BAD_CAST r);

        // Adds the list of otherAttribute in the circ to the rect_node
        addListOfOtherAttrToParent(circ_node, circ -> otherAttributes);
    }
}

void addPathToNode(xmlNodePtr parent_node, List * list, char *nodeName) {

    // If list length is empty, then just ignore this function
    if (list->length <= 0) {
        return;
    }

    // Traversing the list of Paths in the Path List
    void * pathElement;
    ListIterator pathIterator = createIterator(list);
    while ((pathElement = nextElement(&pathIterator)) != NULL) {

        // Creating a Path node for the current Path struct and making it a child of the parent node
        xmlNodePtr path_node = xmlNewChild(parent_node, NULL, BAD_CAST nodeName, BAD_CAST "");

        // Getting the Path struct for the current Path element in the list of Paths
        Path * newPath = (Path*)pathElement;

        int size = strlen(newPath->data) + 150;

        // Creating a variable to hold the data element in the Path struct
        char data[size];
        strcpy(data, "");

        sprintf(data, "%s", newPath -> data);

        // Setting the data inside the Path tag created
        xmlNewProp(path_node, BAD_CAST "d", BAD_CAST data);

        // Adds the list of otherAttributes to the path
        addListOfOtherAttrToParent(path_node, newPath -> otherAttributes);
    }
}

void addGroupToNode(xmlNodePtr root_node, List * grp) {

    // If there were no groups inside the group list, then just ignore performing this function
    if (grp->length <= 0) {
        return;
    }
    // Traversing the list of Groups in the Group List
    void * grpElement;
    ListIterator grpIterator = createIterator(grp);
    while ((grpElement = nextElement(&grpIterator)) != NULL) {
        Group * group = (Group*)grpElement;

        xmlNodePtr group_node = xmlNewChild(root_node, NULL, BAD_CAST "g", BAD_CAST "");

        addRectToNode(group_node, group->rectangles, "rect");
        addCircleToNode(group_node, group->circles, "circle");
        addPathToNode(group_node, group->paths, "path");
        addGroupToNode(group_node, group->groups);
        addListOfOtherAttrToParent(group_node, group->otherAttributes);
    }
}

void addListOfOtherAttrToNode(xmlNodePtr parent_node, List *otherAttrList) {

    // If list length is empty, then just ignore this function
    if (otherAttrList->length <= 0) {
        return;
    }

    // Traversing through the list of otherAttribute
    void *attrElement;
    ListIterator attrIterator = createIterator(otherAttrList);
    while ((attrElement = nextElement(&attrIterator)) != NULL) {

        // Getting the Attribute struct for the current otherAttribute element
        Attribute *attr = (Attribute*)attrElement;

        // Adding the Attribute struct data as a child of the parent node
        xmlNewChild(parent_node, NULL, BAD_CAST attr -> name, BAD_CAST attr -> value);
    }
}

void addListOfOtherAttrToParent(xmlNodePtr parent_node, List *otherAttrList) {

    // If list length is empty, then just ignore this function
    if (otherAttrList->length <= 0) {
        return;
    }

    if (otherAttrList == NULL) {
        return;
    }

    // Traversing through the list of otherAttribute
    void *attrElement;
    ListIterator attrIterator = createIterator(otherAttrList);
    while ((attrElement = nextElement(&attrIterator)) != NULL) {

        // Getting the Attribute struct for the current otherAttribute element
        Attribute *attr = (Attribute*)attrElement;

        if (attr->name == NULL || attr->value == NULL) {
            continue;
        }

        // Adding the Attribute struct data as a child of the parent node
        xmlNewProp(parent_node, BAD_CAST attr -> name, BAD_CAST attr -> value);
    }
}

bool validRectangle (List * rectangle) {
    if (rectangle == NULL) {
        return (FALSE);
    }
    // Traverse list of rectangles
    void * rectElement;
    ListIterator rectIterator = createIterator(rectangle);
    while ((rectElement = nextElement(&rectIterator)) != NULL) {
        // Cast the current element as a Rectangle
        Rectangle * rect = (Rectangle*)rectElement;

        // Error check for NULL unit or otherAttributes (both may be empty still)
        if (rect -> units == NULL || rect -> otherAttributes == NULL) {
            return (FALSE);
        }

        // Error check if the width and height are negatives
        if (rect -> width < 0 || rect -> height < 0) {
            return (FALSE);
        }

        // Error checking the list of otherAttributes
        if (validAttribute(rect->otherAttributes) == 0) {
            return (FALSE);
        }
    }
    return (TRUE);
}

bool validCircle (List * circle) {
    if (circle == NULL) {
        return (FALSE);
    }
    // Traverse list of circles
    void * circElement;
    ListIterator circIterator = createIterator(circle);
    while ((circElement = nextElement(&circIterator)) != NULL) {
        // Cast the current element as a Circle
        Circle * circ = (Circle*)circElement;

        // Error check for NULL unit or otherAttributes
        if (circ -> units == NULL || circ -> otherAttributes == NULL) {
            return (FALSE);
        }

        // Error check if the radius is negative
        if (circ -> r < 0) {
            return (FALSE);
        }

        // Error checking the list of otherAttributes
        if (validAttribute(circ->otherAttributes) == 0) {
            return (FALSE);
        }
    }
    return (TRUE);
}

bool validPath (List * path) {
    if (path == NULL) {
        return (FALSE);
    }
    // Traverse list of circles
    void * pathElement;
    ListIterator pathIterator = createIterator(path);
    while ((pathElement = nextElement(&pathIterator)) != NULL) {
        // Cast the current element as a Path
        Path * newPath = (Path*)pathElement;

        // Error check for NULL otherAttributes or path data
        if (newPath -> otherAttributes == NULL || newPath -> data == NULL) {
            return (FALSE);
        }

        // Error checking the list of otherAttributes
        if (validAttribute(newPath->otherAttributes) == 0) {
            return (FALSE);
        }
    }
    return (TRUE);
}

bool validGroup (List * grp) {
    if (grp == NULL) {
        return (FALSE);
    }
    void * grpElement;
    ListIterator grpIterator = createIterator(grp);
    while ((grpElement = nextElement(&grpIterator)) != NULL) {
        Group * group = (Group*)grpElement;

        // Error checking if Group attributes are NULL
        if (group->rectangles == NULL || group->circles == NULL || group->paths == NULL || group->groups == NULL || group->otherAttributes == NULL) {
            return (FALSE);
        }

        // Checking the list of Rectangles in the given Group for invalids
        if (validRectangle(group->rectangles) != 1) {
            return (FALSE);
        }

        // Check the list of Circles in the given Group for invalids
        if (validCircle(group->circles) != 1) {
            return (FALSE);
        }

        // Check the list of Paths in the given Group for invalids
        if (validPath(group->paths) != 1) {
            return (FALSE);
        }

        // Check the list of inner Groups (the group's group) for invalids
        if (validGroup(group->groups) != 1) {
            return (FALSE);
        }
    }
    return (TRUE);
}

bool validAttribute(List *attribute) {
    if (attribute == NULL) {
        return (FALSE);
    }
    if (getLength(attribute) > 0) {
        // Traversing through the list of otherAttributes
        void *attrElement;
        ListIterator attrIterator = createIterator(attribute);
        while ((attrElement = nextElement(&attrIterator)) != NULL) {
            // Getting the Attribute struct for the current otherAttributes element
            Attribute * attr = (Attribute*)attrElement;

            // Error checking for NULL attribute members
            if (attr->name == NULL) {
                return (FALSE);
            }
            // Error checking for empty name string, as SVGParser.h only states it CAN'T be NULL
            if (strcmp(attr->name, "") == 0) {
                return (FALSE);
            }
        }
    }

    // Returns TRUE if the list of otherAttribute meets all the constraints of the SVGParser.h file
    return(TRUE);
}

bool setRectangle(List * list, int index, Attribute * newAttribute) {
    int counter = 0;

    // Traverse list of rectangles
    void * rectElement;
    ListIterator rectIterator = createIterator(list);
    while ((rectElement = nextElement(&rectIterator)) != NULL) {
        if (counter != index) {
            counter++;
            continue;
        }
        // Cast the current element as a Rectangle
        Rectangle * rect = (Rectangle*)rectElement;

        if (strcasecmp(newAttribute->name, "x") == 0) {
            // If units are found inside the value, just remove it
            if (containsUnits(newAttribute->value)) {
                removeUnits(newAttribute->value);
                rect->x = atof(newAttribute->value);
            }
            else if (!containsUnits(newAttribute->value)) {
                rect->x = atof(newAttribute->value);
            }
            free(newAttribute->name);
            free(newAttribute);
            return (TRUE);
        }
        else if (strcasecmp(newAttribute->name, "y") == 0) {
            // If units are found inside the value, just remove it
            if (containsUnits(newAttribute->value)) {
                removeUnits(newAttribute->value);
                rect->y = atof(newAttribute->value);
            }
            else if (!containsUnits(newAttribute->value)) {
                rect->y = atof(newAttribute->value);
            }
            free(newAttribute->name);
            free(newAttribute);
            return (TRUE);
        }
        else if (strcasecmp(newAttribute->name, "width") == 0) {
            // If units are found inside the value, just remove it
            if (containsUnits(newAttribute->value)) {
                removeUnits(newAttribute->value);
                if (atof(newAttribute->value) >= 0) {
                    rect->width = atof(newAttribute->value);
                    free(newAttribute->name);
                    free(newAttribute);
                    return (TRUE);
                }
                else {
                    return (FALSE);
                }
            }
            else if (!containsUnits(newAttribute->value)) {
                if (atof(newAttribute->value) >= 0) {
                    rect->width = atof(newAttribute->value);
                    free(newAttribute->name);
                    free(newAttribute);
                    return (TRUE);
                }
                else {
                    return (FALSE);
                }
            }
        }
        else if (strcasecmp(newAttribute->name, "height") == 0) {
            // If units are found inside the value, just remove it
            if (containsUnits(newAttribute->value)) {
                removeUnits(newAttribute->value);
                if (atof(newAttribute->value) >= 0) {
                    rect->height = atof(newAttribute->value);
                    free(newAttribute->name);
                    free(newAttribute);
                    return (TRUE);
                }
                else {
                    return (FALSE);
                }
            }
            else if (!containsUnits(newAttribute->value)) {
                if (atof(newAttribute->value) >= 0) {
                    rect->height = atof(newAttribute->value);
                    free(newAttribute->name);
                    free(newAttribute);
                    return (TRUE);
                }
                else {
                    return (FALSE);
                }
            }
        }
        // If helper function finds that the attribute name exists, then it will change it
        else if (checkOtherAttributes(rect->otherAttributes, newAttribute) == true) {
            free(newAttribute->name);
            free(newAttribute);
            return (TRUE);
        }
        // If attribute did not exist, then add it
        else {
            Attribute *otherRectAttr = malloc(sizeof(Attribute) + strlen(newAttribute->value) * sizeof(char) + 1);
            otherRectAttr->name = malloc(strlen(newAttribute->name) * sizeof(char) + 1);

            strcpy(otherRectAttr->name, newAttribute->name);
            strcpy(otherRectAttr->value, newAttribute->value);
            
            insertBack(rect->otherAttributes, otherRectAttr);
            return (TRUE);
        }
        counter++;
    }
    return (TRUE);
}

bool setCircle(List * list, int index, Attribute * newAttribute) {

    int counter = 0;

    // Traverse list of circles
    void * circElement;
    ListIterator circIterator = createIterator(list);
    while ((circElement = nextElement(&circIterator)) != NULL) {
        if (counter != index) {
            counter++;
            continue;
        }
        // Cast the current element as a Circle
        Circle * circ = (Circle*)circElement;

        if (strcasecmp(newAttribute->name, "cx") == 0) {
            // If units are found inside the value, just remove it
            if (containsUnits(newAttribute->value)) {
                removeUnits(newAttribute->value);
                circ->cx = atof(newAttribute->value);
            }
            else if (!containsUnits(newAttribute->value)) {
                circ->cx = atof(newAttribute->value);
            }
            free(newAttribute->name);
            free(newAttribute);
            return (TRUE);
        }
            
        else if (strcasecmp(newAttribute->name, "cy") == 0) {
            // If units are found inside the value, just remove it
            if (containsUnits(newAttribute->value)) {
                removeUnits(newAttribute->value);
                circ->cy = atof(newAttribute->value);
            }
            else if (!containsUnits(newAttribute->value)) {
                circ->cy = atof(newAttribute->value);
            }
            free(newAttribute->name);
            free(newAttribute);
            return (TRUE);
        }
    
        else if (strcasecmp(newAttribute->name, "r") == 0) {
            // If units are found inside the value, just remove it
            if (containsUnits(newAttribute->value)) {
                removeUnits(newAttribute->value);
                if (atof(newAttribute->value) >= 0) {
                    circ->r = atof(newAttribute->value);
                    free(newAttribute->name);
                    free(newAttribute);
                    return (TRUE);
                }
                else {
                    return (FALSE);
                }
            }
            else if (!containsUnits(newAttribute->value)) {
                if (atof(newAttribute->value) >= 0) {
                    circ->r = atof(newAttribute->value);
                    free(newAttribute->name);
                    free(newAttribute);
                    return (TRUE);
                }
                else {
                    return (FALSE);
                }
            }
        }

        // If helper function finds that the attribute name exists, then it will change it
        else if (checkOtherAttributes(circ->otherAttributes, newAttribute) == true) {
            free(newAttribute->name);
            free(newAttribute);
            return (TRUE);
        }
        // If attribute did not exist, then add it
        else {
            Attribute *otherCircAttr = malloc(sizeof(Attribute) + strlen(newAttribute->value) * sizeof(char) + 1);
            otherCircAttr->name = malloc(strlen(newAttribute->name) * sizeof(char) + 1);

            strcpy(otherCircAttr->name, newAttribute->name);
            strcpy(otherCircAttr->value, newAttribute->value);
            
            insertBack(circ->otherAttributes, otherCircAttr);
            return (TRUE);
        }
        counter++;
    }
    return (TRUE);
}

void setPath(List * list, int index, Attribute * newAttribute) {

    int counter = 0;

    // Traverse list of Paths
    void * pathElement;
    ListIterator pathIterator = createIterator(list);
    while ((pathElement = nextElement(&pathIterator)) != NULL) {
        if (counter != index) {
            counter++;
            continue;
        }
        // Cast the current element as a Path type
        Path * newPath = (Path*)pathElement;

        // If the attribute is an existing Path element, then change it
        if (strcasecmp(newAttribute->name, "d") == 0) {
            strcpy(newPath->data, newAttribute->value);
            free(newAttribute->name);
            free(newAttribute);
            return;
        }

        // If helper function finds that the attribute name exists, then it will change it
        else if (checkOtherAttributes(newPath->otherAttributes, newAttribute) == true) {
            free(newAttribute->name);
            free(newAttribute);
            return;
        }
        // If attribute did not exist, then add it
        else {
            Attribute *otherPathAttr = malloc(sizeof(Attribute) + strlen(newAttribute->value) * sizeof(char) + 1);
            otherPathAttr->name = malloc(strlen(newAttribute->name) * sizeof(char) + 1);

            strcpy(otherPathAttr->name, newAttribute->name);
            strcpy(otherPathAttr->value, newAttribute->value);
            
            insertBack(newPath->otherAttributes, otherPathAttr);
            return;
        }
        counter++;
    }
}

void setGroup(List * list, int index, Attribute * newAttribute) {

    int counter = 0;

    // Create the element and traverse the list of groups
    void * grpElement;
    ListIterator grpIterator = createIterator(list);
    while ((grpElement = nextElement(&grpIterator)) != NULL) {
        if (counter != index) {
            counter++;
            continue;
        }
        Group * group = (Group*)grpElement;

        // If helper function finds that the attribute name exists, then it will change it
        if (checkOtherAttributes(group->otherAttributes, newAttribute) == true) {
            free(newAttribute->name);
            free(newAttribute);
            return;
        }
        // If attribute did not exist, then add it
        else {
            Attribute *otherGroupAttr = malloc(sizeof(Attribute) + strlen(newAttribute->value) * sizeof(char) + 1);
            otherGroupAttr->name = malloc(strlen(newAttribute->name) * sizeof(char) + 1);

            strcpy(otherGroupAttr->name, newAttribute->name);
            strcpy(otherGroupAttr->value, newAttribute->value);
            
            insertBack(group->otherAttributes, otherGroupAttr);
        }
        counter++;
    }
}

void setAttrInSVG(SVG * img, Attribute * newAttribute) {
    /*
    if (strcasecmp(newAttribute->name, "xmlns") == 0) {
        strcpy(img->namespace, newAttribute->value);
        free(newAttribute->name);
        free(newAttribute);
        return;
    }
    */
    // If attribute was title, then change it. If it exceeds char limit then truncate it
    if (strcasecmp(newAttribute->name, "title") == 0) {
        char* title = malloc(sizeof(char) * (strlen(newAttribute->value) + 1));
        strcpy(title, (char*)newAttribute->value);
        if (strlen(title) > 256) {
            title[255] = '\0';
        }
        strcpy(img->title, title);
        free(title);
        free(newAttribute->name);
        free(newAttribute);
        return;
    }
    // If attribute was descr, then change it. If it exceeds char limit then truncate it
    if (strcasecmp(newAttribute->name, "desc") == 0) {
        char* descr = malloc(sizeof(char) * (strlen(newAttribute->value) + 1));
        strcpy(descr, (char*)newAttribute->value);
        if (strlen(descr) > 256) {
            descr[255] = '\0';
        }
        strcpy(img->description, descr);
        free(descr);
        free(newAttribute->name);
        free(newAttribute);
        return;
    }

    // If helper function finds that the attribute name exists, then it will change it
    if (checkOtherAttributes(img->otherAttributes, newAttribute) == true) {
        free(newAttribute->name);
        free(newAttribute);
        return;
    }
    // If attribute did not exist, then add it
    else {
        Attribute *otherSVGAttr = malloc(sizeof(Attribute) + strlen(newAttribute->value) * sizeof(char) + 1);
        otherSVGAttr->name = malloc(strlen(newAttribute->name) * sizeof(char) + 1);

        strcpy(otherSVGAttr->name, newAttribute->name);
        strcpy(otherSVGAttr->value, newAttribute->value);
            
        insertBack(img->otherAttributes, otherSVGAttr);
        return;
    }
}

bool checkOtherAttributes (List * list, Attribute * newAttribute) {
    // Traversing through the list of otherAttributes
    void *attrElement;
    ListIterator attrIterator = createIterator(list);
    while ((attrElement = nextElement(&attrIterator)) != NULL) {
        // Getting the Attribute struct for the current otherAttributes element
        Attribute * attr = (Attribute*)attrElement;

        // If the attribute name exists, then change its value
        if (strcmp(attr->name, newAttribute->name) == 0) {
            strcpy(attr->value, newAttribute->value);
            return (TRUE);
        }
    }
    // If the newAttribute wasn't found, then just return FALSE
    return (FALSE);
}

// ---------------------------------------------- A3 Helper Functions ---------------------------------------------------------

/*
char * SVGFiletoJSON (char * filename, char * schemaFile) {
    SVG * image = createValidSVG(filename, "parser/src/svg.xsd");

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
    char fileStr[1024];
    char title[256];
    char description[256];
    int tempSize = findFileSize(filename);

    // Create the Lists for the svg struct and then find the length of each
    List * rects = getRects(image);
    List * circs = getCircles(image);
    List * paths = getPaths(image);
    List * groups = getGroups(image);

    sprintf(rectStr, "%d", getLength(rects));
    sprintf(circStr, "%d", getLength(circs));
    sprintf(pathStr, "%d", getLength(paths));
    sprintf(grpStr, "%d", getLength(groups));
    sprintf(fileStr, "%d", tempSize);
    strcat(fileStr, "KB");
    sprintf(title, "%s", image->title);
    sprintf(description, "%s", image->description);

    // Allocate space and then save the string desired into str, which is returned
    length = strlen(fileStr) + strlen(rectStr) + strlen(circStr) + strlen(pathStr) + strlen(grpStr) + strlen(title) + strlen(description) + 150;
    str = (char*)malloc(sizeof(char) * length);
    sprintf(str, "{\"title\":\"%s\",\"description\":\"%s\",\"fileSize\":\"%s\",\"numRect\":%s,\"numCirc\":%s,\"numPaths\":%s,\"numGroups\":%s}", title, description, fileStr, rectStr, circStr, pathStr, grpStr);

    freeList(rects);
    freeList(circs);
    freeList(paths);
    freeList(groups);

    return str;
}
*/

char * SVGFiletoJSON (char * filename, char * schemaFile) {
    SVG * image = createValidSVG(filename, "parser/src/svg.xsd");

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
    char fileStr[1024];
    int tempSize = findFileSize(filename);

    // Create the Lists for the svg struct and then find the length of each
    List * rects = getRects(image);
    List * circs = getCircles(image);
    List * paths = getPaths(image);
    List * groups = getGroups(image);

    sprintf(rectStr, "%d", getLength(rects));
    sprintf(circStr, "%d", getLength(circs));
    sprintf(pathStr, "%d", getLength(paths));
    sprintf(grpStr, "%d", getLength(groups));
    sprintf(fileStr, "%d", tempSize);
    strcat(fileStr, "KB");

    // Allocate space and then save the string desired into str, which is returned
    length = strlen(fileStr) + strlen(rectStr) + strlen(circStr) + strlen(pathStr) + strlen(grpStr) + 100;
    str = (char*)malloc(sizeof(char) * length);
    sprintf(str, "{\"fileSize\":\"%s\",\"numRect\":%s,\"numCirc\":%s,\"numPaths\":%s,\"numGroups\":%s}", fileStr, rectStr, circStr, pathStr, grpStr);

    freeList(rects);
    freeList(circs);
    freeList(paths);
    freeList(groups);

    return str;
}

char * SVGDataToRectList (char * filename, char * schemaFile) {
    SVG * svg = createValidSVG(filename, "parser/src/svg.xsd");

    if (svg == NULL) {
        char *empty = malloc(3 * sizeof(char));
        strcpy(empty, "[]");
        return empty;
    }

    char * str = rectListToJSON(svg->rectangles);
    deleteSVG(svg);

    return str;
}

char * SVGDataToCircleList (char * filename, char * schemaFile) {
    SVG * svg = createValidSVG(filename, "parser/src/svg.xsd");

    if (svg == NULL) {
        char *empty = malloc(3 * sizeof(char));
        strcpy(empty, "[]");
        return empty;
    }

    char * str = circListToJSON(svg->circles);
    deleteSVG(svg);

    return str;
}

char * SVGDataToPathList (char * filename, char * schemaFile) {
    SVG * svg = createValidSVG(filename, "parser/src/svg.xsd");

    if (svg == NULL) {
        char *empty = malloc(3 * sizeof(char));
        strcpy(empty, "[]");
        return empty;
    }
    char * str = pathListToJSON(svg->paths);
    deleteSVG(svg);

    return str;
}

char * SVGDataToGroupList (char * filename, char * schemaFile) {
    SVG * svg = createValidSVG(filename, "parser/src/svg.xsd");

    if (svg == NULL) {
        char *empty = malloc(3 * sizeof(char));
        strcpy(empty, "[]");
        return empty;
    }

    char * str = groupListToJSON(svg->groups);
    deleteSVG(svg);

    return str;
}

char * convertAttributeToJSON (char * string, char * schemaFile) {
    if (string == NULL || (strcmp(string, "") == 0)) {
        char * empty = malloc(2);
        strcpy(empty, "");
        return string;
    }
    SVG * svg = createValidSVG(string, "parser/src/svg.xsd");

    char titleStr[1024];
    char descStr[1024];
    char * str;

    sprintf(titleStr, "%s", svg->title);
    sprintf(descStr, "%s", svg->description);

    int length = strlen(titleStr) + strlen(descStr) + 15;
    str = (char*)malloc(sizeof(char) * length);
    sprintf(str, "{\"title\":\"%s\",\"description\":\"%s\"", titleStr, descStr);

    return str;
}

char * convertTitleToJSON(char * string, char * schemaFile) {
    if (string == NULL || (strcmp(string, "") == 0)) {
        char * empty = malloc(2);
        strcpy(empty, "");
        return string;
    }
    SVG * svg = createValidSVG(string, "parser/src/svg.xsd");

    char * str = "";
    str = (char*)malloc((sizeof(char)+5) * strlen(svg->title));
    sprintf(str, "%s", svg->title);

    return str;
}

char * convertDescToJSON(char * string, char * schemaFile) {
    if (string == NULL || (strcmp(string, "") == 0)) {
        char * empty = malloc(2);
        strcpy(empty, "");
        return string;
    }
    SVG * svg = createValidSVG(string, "parser/src/svg.xsd");

    char * str = "";
    str = (char*)malloc((sizeof(char)+5) * strlen(svg->description));
    sprintf(str, "%s", svg->description);

    return str;
}

int findFileSize(char * filename) {
    FILE* fp = fopen(filename, "r");
  
    // checking if the file exist or not
    if (fp == NULL) {
        return 0;
    }
  
    fseek(fp, 0L, SEEK_END);
  
    long double size = ftell(fp);
    long double kilo = 1024;
  
    long double next = (size/kilo);
    int final = round(next);
    fclose(fp);
  
    return final;
}
  
bool validateSVGinJSON(char * filename, const char* schemaFile) {
    SVG * img = createSVG(filename);

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

bool JSONcreateSVG(char * JSONStr, char * filename) {
    SVG * svg = JSONtoSVG(JSONStr);

    if (validateSVG(svg, "parser/src/svg.xsd") == TRUE) {
        bool validity = writeSVG(svg, filename);

        if (validity == FALSE) {
            deleteSVG(svg);
            return FALSE;
        }
    }
    else {
        deleteSVG(svg);
        return FALSE;
    }

    deleteSVG(svg);
    return TRUE;
}

bool JSONeditSVG(char * filename, char * editTitle, char * editDesc) {
    SVG * svg = createValidSVG(filename, "parser/src/svg.xsd");

    // Check if empty
    if ((strcasecmp(editTitle, "") == 0) && (strcasecmp(editDesc, "") == 0)) {
        return FALSE;
    }
    // If title wasn't empty, then edit it
    if (strcasecmp(editTitle, "") != 0) {
        char * tempTitle = malloc(sizeof(char) * strlen(editTitle) + 1);
        strcpy(tempTitle, (char*)editTitle);
        if (strlen(tempTitle) > 256) {
            tempTitle[255] = '\0';
        }
        strcpy(svg->title,  tempTitle);
        free(tempTitle);
    }
    // If description wasn't empty then edit it
    if (strcasecmp(editDesc, "") != 0) {
        char * tempDesc = malloc(sizeof(char) * strlen(editDesc) + 1);
        strcpy(tempDesc, (char*)editDesc);
        if (strlen(tempDesc) > 256) {
            tempDesc[255] = '\0';
        }
        strcpy(svg->description,  tempDesc);
        free(tempDesc);
    }
    bool validity = writeSVG(svg, filename);

    if (validity == FALSE) {
        deleteSVG(svg);
        return FALSE;
    }


    deleteSVG(svg);
    return TRUE;
}

char * SVGRectsAttrListToJSON(char * filename) {
    SVG * svg = createValidSVG(filename, "parser/src/svg.xsd");

    char * str = malloc(2);
    strcpy(str, "[");

    if (validateSVG(svg, "parser/src/svg.xsd") == TRUE) {

        void * rectElement;
        ListIterator rectIterator = createIterator((List*)svg->rectangles);
        while ((rectElement = nextElement(&rectIterator)) != NULL) {
            Rectangle * rect = (Rectangle*)rectElement;

            char * attrString = attrListToJSON(rect->otherAttributes);
            str = realloc(str, strlen(str) + 1 + strlen(attrString) + 5);

            strcat(str, attrString);
            strcat(str, ",");

            free(attrString);
        }
        str[strlen(str) - 1] = ']';
    }   
    else {
        str = malloc(3);
        strcpy(str, "[]");
    }
    deleteSVG(svg);
    return str;
}

char * SVGCircleAttrListToJSON(char * filename) {
    SVG * svg = createValidSVG(filename, "parser/src/svg.xsd");

    char * str = malloc(2);
    strcpy(str, "[");

    if (validateSVG(svg, "parser/src/svg.xsd") == TRUE) {

        void * circElement;
        ListIterator circIterator = createIterator((List*)svg->circles);
        while ((circElement = nextElement(&circIterator)) != NULL) {
            Circle * circ = (Circle*)circElement;

            char * attrString = attrListToJSON(circ->otherAttributes);
            str = realloc(str, strlen(str) + 1 + strlen(attrString) + 5);

            strcat(str, attrString);
            strcat(str, ",");

            free(attrString);
        }
        str[strlen(str) - 1] = ']';
    }   
    else {
        str = malloc(3);
        strcpy(str, "[]");
    }
    deleteSVG(svg);
    return str;
}

char * SVGPathAttrListToJSON(char * filename) {
    SVG * svg = createValidSVG(filename, "parser/src/svg.xsd");

    char * str = malloc(2);
    strcpy(str, "[");

    if (validateSVG(svg, "parser/src/svg.xsd") == TRUE) {

        void * pathElement;
        ListIterator pathIterator = createIterator((List*)svg->paths);
        while ((pathElement = nextElement(&pathIterator)) != NULL) {
            Path * path = (Path*)pathElement;

            char * attrString = attrListToJSON(path->otherAttributes);
            str = realloc(str, strlen(str) + 1 + strlen(attrString) + 5);

            strcat(str, attrString);
            strcat(str, ",");

            free(attrString);
        }
        str[strlen(str) - 1] = ']';
    }   
    else {
        str = malloc(3);
        strcpy(str, "[]");
    }
    deleteSVG(svg);
    return str;
}

char * SVGGroupAttrListToJSON(char * filename) {
    SVG * svg = createValidSVG(filename, "parser/src/svg.xsd");

    char * str = malloc(2);
    strcpy(str, "[");

    if (validateSVG(svg, "parser/src/svg.xsd") == TRUE) {

        void * groupElement;
        ListIterator groupIterator = createIterator((List*)svg->groups);
        while ((groupElement = nextElement(&groupIterator)) != NULL) {
            Group * grp = (Group*)groupElement;

            char * attrString = attrListToJSON(grp->otherAttributes);
            str = realloc(str, strlen(str) + 1 + strlen(attrString) + 5);

            strcat(str, attrString);
            strcat(str, ",");

            free(attrString);
        }
        str[strlen(str) - 1] = ']';
    }   
    else {
        str = malloc(3);
        strcpy(str, "[]");
    }
    deleteSVG(svg);
    return str;
}

bool JSONeditAttrSVG(char * filename, char * elemType, int elemIndex, char * attrString, char * valString) {
    SVG * svg = createValidSVG(filename, "parser/src/svg.xsd");


    if (validateSVG(svg, "parser/src/svg.xsd") == TRUE) {
        Attribute * temp = malloc(sizeof(Attribute) + strlen(valString) * sizeof(char) + 1);
        temp->name = malloc(strlen(attrString) * sizeof(char) + 1);

        strcpy(temp->name, (char*)attrString);
        strcpy(temp->value, (char*)valString);

        if ((strcasecmp(elemType, "Rectangle") == 0) || (strcasecmp(elemType, "rect") == 0)) {
            if (setAttribute(svg, RECT, elemIndex, temp) == FALSE) {
                deleteSVG(svg);
                return (FALSE);
            }
        }
        else if ((strcasecmp(elemType, "Circle") == 0) || (strcasecmp(elemType, "circ") == 0)) {
            if (setAttribute(svg, CIRC, elemIndex, temp) == FALSE) {
                deleteSVG(svg);
                return (FALSE);
            }
        }
        else if (strcasecmp(elemType, "Path") == 0) {
            if (setAttribute(svg, PATH, elemIndex, temp) == FALSE) {
                deleteSVG(svg);
                return (FALSE);
            }
        }
        else if (strcasecmp(elemType, "Group") == 0) {
            if (setAttribute(svg, GROUP, elemIndex, temp) == FALSE) {
                deleteSVG(svg);
                return (FALSE);
            }
        }
        else if ((strcasecmp(elemType, "SVG_IMG") == 0) || (strcasecmp(elemType, "SVG") == 0)) {
            if (setAttribute(svg, SVG_IMG, elemIndex, temp) == FALSE) {
                deleteSVG(svg);
                return (FALSE);
            }
        }
    }
    else {
        deleteSVG(svg);
        return (FALSE);
    }
    bool validity = writeSVG(svg, filename);

    if (validity == FALSE) {
        deleteSVG(svg);
        return FALSE;
    }
    deleteSVG(svg);
    return TRUE;
}

bool AddRectangleToJSON(char * filename, char * xString, char * yString, char * width, char * height, char * units) {
    // Check if empty
    if ((strcasecmp(width, "") == 0) || (strcasecmp(height, "") == 0) || (strcasecmp(xString, "") == 0) || (strcasecmp(yString, "") == 0)) {
        return FALSE;
    }

    SVG * svg = createValidSVG(filename, "parser/src/svg.xsd");

    Rectangle * rect = malloc(sizeof(Rectangle));

    rect->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
    rect->x = atof(xString);
    rect->y = atof(yString);
    rect->width = atof(width);
    rect->height = atof(height);
    strcpy(rect->units, units);

    addComponent(svg, RECT, rect);
    bool validity = writeSVG(svg, filename);

    if (validity == FALSE) {
        deleteSVG(svg);
        return FALSE;
    }

    deleteSVG(svg);
    return TRUE;
}


bool AddCircleToJSON(char * filename, char * cxString, char * cyString, char * radius, char * units) {
    // Check if empty
    if ((strcasecmp(radius, "") == 0) || (strcasecmp(cxString, "") == 0) || (strcasecmp(cyString, "") == 0)) {
        return FALSE;
    }

    if (!validUnits(units)) {
        return FALSE;
    }
    
    SVG * svg = createValidSVG(filename, "parser/src/svg.xsd");

    Circle * circ= malloc(sizeof(Circle));
    
    circ->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
    circ->cx = atof(cxString);
    circ->cy = atof(cyString);
    circ->r = atof(radius);
    strcpy(circ->units, units);

    addComponent(svg, CIRC, circ);
    bool validity = writeSVG(svg, filename);

    if (validity == FALSE) {
        deleteSVG(svg);
        return FALSE;
    }

    deleteSVG(svg);
    return TRUE;
}

//Helper function to check for valid units
bool validUnits (char *string) {
    /* The supported length unit identifiers in SVG are: 
    /  em, ex, px, pt, pc, cm, mm, in, and percentages.
    */
    if (strstr(string, "em") != NULL) {
        return (TRUE);
    }
    else if (strstr(string, "ex") != NULL) {
        return (TRUE);
    }
    else if (strstr(string, "px") != NULL) {
        return (TRUE);
    }
    else if (strstr(string, "pt") != NULL) {
        return (TRUE);
    }
    else if (strstr(string, "pc") != NULL) {
        return (TRUE);
    }
    else if (strstr(string, "cm") != NULL) {
        return (TRUE);
    }
    else if (strstr(string, "mm") != NULL) {
        return (TRUE);
    }
    else if (strstr(string, "%%") != NULL) {
        return (TRUE);
    }
    else {
        return (FALSE);
    }
}

bool scaleShapes(char * filename, char * rectScale, char * circScale) {
    SVG * svg = createValidSVG(filename, "parser/src/svg.xsd");

    int rectNum = atoi(rectScale);
    int circNum = atoi(circScale);

    List * rects = getRects(svg);
    List * circs = getCircles(svg);

    // Traversing the list of Rectangles in the Rectangles List
    void *rectElement;
    ListIterator rectIterator = createIterator(rects);
    while ((rectElement = nextElement(&rectIterator)) != NULL) {
        Rectangle * rect = (Rectangle*)rectElement;

        rect->width = rect->width * rectNum;
        rect->height = rect->height * rectNum;
    }

    void *circElement;
    ListIterator circIterator = createIterator(circs);
    while ((circElement = nextElement(&circIterator)) != NULL) {
        Circle * circle = (Circle*)circElement;

        circle->r = circle->r * circNum;
    }

    if (validateSVG(svg, "parser/src/svg.xsd") == TRUE) {
        bool validity = writeSVG(svg, filename);

        if (validity == FALSE) {
            deleteSVG(svg);
            return FALSE;
        }
    }
    else {
        deleteSVG(svg);
        return (FALSE);
    }

    deleteSVG(svg);
    return (TRUE);
}
#endif

