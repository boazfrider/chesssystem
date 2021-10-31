
#include <stdlib.h>
#include "map.h"

typedef struct node_t *Node;

struct node_t{
    MapKeyElement key;
    MapDataElement value;
    Node next;
};

struct Map_t
{
    copyMapDataElements copyData; //pointer to copy function for data
    copyMapKeyElements copyKey;
    freeMapDataElements freeData;
    freeMapKeyElements freeKey;
    compareMapKeyElements compareKey;
    Node  head;
    Node iterator;

};
static Node createElement(MapDataElement value,MapKeyElement key){
    if(value == NULL || key == NULL ) {
        return NULL;
    }
    Node element=malloc(sizeof(*element));
    if( NULL == element) {
        return NULL;
    }
    element->key=key;
    element->value=value;
    return element;
}

Map mapCreate(
    copyMapDataElements copyDataElement,
    copyMapKeyElements copyKeyElement,
    freeMapDataElements freeDataElement,
    freeMapKeyElements freeKeyElement,
    compareMapKeyElements compareKeyElements
)
{
    if(copyDataElement == NULL || copyKeyElement==NULL||
    freeDataElement==NULL || freeKeyElement==NULL||compareKeyElements == NULL) {
        return NULL;
    }
    Map map = (Map) malloc(sizeof(*map));
    if(map == NULL) {
        return NULL;
    }
    map->copyData = copyDataElement;
    map->copyKey = copyKeyElement;
    map->freeData = freeDataElement;
    map->freeKey = freeKeyElement;
    map->compareKey = compareKeyElements;
    map->head = malloc(sizeof(*(map->head)));
    if(map->head == NULL){
        mapDestroy(map);
        return NULL;
    }
    (map->head)->key=NULL;
  (map->head)->value= NULL;
   (map->head)->next=NULL;
    map->iterator = NULL;
    return map;
}
void mapDestroy(Map map)
{
    if(map == NULL){
        return;
    }
    mapClear(map);
    free(map->head);
    free(map);
    return;
}
//for each element in map ,copy key and value
Map mapCopy(Map map)
{

    if(map == NULL){
        return NULL;
    }
    Map map_copy= mapCreate(map->copyData, map->copyKey, map->freeData
    , map->freeKey,map->compareKey);
    if(map_copy == NULL) {
        return NULL;
    }
    Node itr=(map->head)->next;
    Node last_copied=map_copy->head;
    while(itr != NULL)
    {
        Node new_node=createElement(map->copyData(itr->value),map->copyKey(itr->key));
        if(new_node== NULL)
        {
            mapDestroy(map_copy);
            return NULL;
        }
        last_copied->next=new_node;
        last_copied=last_copied->next;
        last_copied->next=NULL;
        itr=itr->next;
    }
    return map_copy;

}

int mapGetSize(Map map)
{
      if(map == NULL){
          return -1;
      }
    Node index = (map->head)->next;
    int size = 0;
    while(index != NULL){
        size++;
        index = index->next;
    }
    return size;
}

bool mapContains(Map map, MapKeyElement element)
{
    if (NULL == map || NULL == element)  {
        return false;
    }
    Node itr=map->iterator;
    MAP_FOREACH(MapKeyElement,checker,map)
    {
            if(map->compareKey(element,checker)==0)
            {
                map->iterator=itr;//changing back map->iterator to itr(map_Foreach intializes map->iterator.)
                map->freeKey(checker);
                return true;
            }
            map->freeKey(checker);
    }
    map->iterator=itr;
    return false; 
}

MapResult mapPut(Map map, MapKeyElement key_element, MapDataElement data_element){
    if(map == NULL || key_element == NULL || data_element == NULL)  {
        return MAP_NULL_ARGUMENT;
    }
    Node i = map->head;
    while(i->next != NULL){//comparing keys here,until we reach the stag
        int result = (map->compareKey)((i->next)->key, key_element);
        if(result < 0) {
            i = i->next;
            continue;
        }
        else if(result == 0){// if we found matchign keys.
            i = i->next;
            MapDataElement new_data = map->copyData(data_element);
            if(new_data == NULL) {
                return MAP_OUT_OF_MEMORY;
            }
            map->freeData(i->value);//oh lol i should've free here?ok..
            i->value = new_data;
            return MAP_SUCCESS;
        }
        else{
            map->iterator = NULL;
            Node new = createElement(map->copyData(data_element),
                                                     map->copyKey(key_element));
            if(new == NULL) {
                return MAP_OUT_OF_MEMORY;
            }
            new->next = i->next;
            i->next = new;
            return MAP_SUCCESS;
        }
    }
    map->iterator = NULL;
    Node new = createElement(map->copyData(data_element), map->copyKey(key_element));
    if(new == NULL) {
        return MAP_OUT_OF_MEMORY;
    }
    i->next = new;
    new->next = NULL;
    return MAP_SUCCESS;
}

MapDataElement mapGet(Map map, MapKeyElement key_element)
{
     if(map == NULL || key_element == NULL) {
         return NULL;//if im doing returns,no point in if.
     }
     if(!mapContains(map, key_element)) {
         return NULL;
     }
     Node tmp_ptr=map->iterator;
       MAP_FOREACH(MapKeyElement,checker,map)  {
            if(!(map->compareKey(checker, key_element))) //
            {
                MapDataElement value=(map->iterator)->value;
                map->iterator=tmp_ptr;//to intizliaze back
                map->freeKey(checker);
                return value;
            }
            map->freeKey(checker);
            checker=NULL;
        }
     return NULL;//unreachable code.
}
MapResult mapRemove(Map map, MapKeyElement key_element)
{
    //safety check
    if(map == NULL || key_element == NULL)   {
        return MAP_NULL_ARGUMENT;
    }
    if(mapContains(map, key_element) == false) {
        return MAP_ITEM_DOES_NOT_EXIST;
    }
    Node temp_ptr=map->head;
    MAP_FOREACH(MapKeyElement,checker,map) {
     if(!(map->compareKey(key_element, checker))) {
            map->freeData((map->iterator)->value);
            map->freeKey((map->iterator)->key);
            temp_ptr->next = (map->iterator)->next;
            free(map->iterator);
            map->iterator = NULL;
            map->freeKey(checker);
            return MAP_SUCCESS;
     }
     map->freeKey(checker);
     temp_ptr=map->iterator;
    }
return  MAP_ITEM_ALREADY_EXISTS;//unreachable
}
MapKeyElement mapGetFirst(Map map)
{
     //conditions-merge together
    if(NULL == map ||(map->head)->next == NULL) {
        return NULL;
    }
    map->iterator = (map->head)->next; 
    return map->copyKey((map->iterator)->key);
}

MapKeyElement mapGetNext(Map map)
{
    if(NULL == map || map->iterator == NULL) {
        return NULL;
    }
        map->iterator = (map->iterator)->next;
        if(map->iterator == NULL) {
            return NULL;
        }
        return map->copyKey((map->iterator)->key);
}

MapResult mapClear(Map map) {
    if (map == NULL) {
        return MAP_NULL_ARGUMENT;
    }
    Node index = (map->head)->next;
    while (index != NULL) {
        map->freeKey(index->key);
        map->freeData(index->value);
        Node temp = index;
        index = index->next;
        free(temp);
    }
    (map->head)->next = NULL;
    map->iterator = NULL;
    return MAP_SUCCESS;
}

