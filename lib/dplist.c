/**
 * \author Chrysovalantis Psilos
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "dplist.h"

/*
 * definition of error codes
 * */
#define DPLIST_NO_ERROR 0
#define DPLIST_MEMORY_ERROR 1 // error due to mem alloc failure
#define DPLIST_INVALID_ERROR 2 //error due to a list operation applied on a NULL list 

#ifdef DEBUG
#define DEBUG_PRINTF(...) 									                                        \
        do {											                                            \
            fprintf(stderr,"\nIn %s - function %s at line %d: ", __FILE__, __func__, __LINE__);	    \
            fprintf(stderr,__VA_ARGS__);								                            \
            fflush(stderr);                                                                         \
                } while(0)
#else
#define DEBUG_PRINTF(...) (void)0
#endif


#define DPLIST_ERR_HANDLER(condition, err_code)                         \
    do {                                                                \
            if ((condition)) DEBUG_PRINTF(#condition " failed\n");      \
            assert(!(condition));                                       \
        } while(0)


/*
 * The real definition of struct list / struct node
 */

struct dplist_node {
    dplist_node_t *prev, *next;
    void *element;
};

struct dplist {
    dplist_node_t *head;
    void *(*element_copy)(void *src_element);
    void (*element_free)(void **element);
    int (*element_compare)(void *x, void *y);
};


dplist_t *dpl_create(// callback functions
        void *(*element_copy)(void *src_element),
        void (*element_free)(void **element),
        int (*element_compare)(void *x, void *y)
) {
    dplist_t *list;
    list = malloc(sizeof(struct dplist));
    DPLIST_ERR_HANDLER(list == NULL, DPLIST_MEMORY_ERROR);
    list->head = NULL;
    list->element_copy = element_copy;
    list->element_free = element_free;
    list->element_compare = element_compare;
    return list;
}

void dpl_free(dplist_t **list, bool free_element) {
	if(*list==NULL){
	    	return ;
	}else{
		dplist_node_t* list_node;
	    list_node =(*list)->head;
	 
	    for(int i=0; i<dpl_size((*list));i++){
	      if(free_element==true){
	    	(*list)->element_free(&list_node->element);
	    }
	    	free(list_node->prev);
	    	
	    }
	  free(*list);
	  *list=NULL;
	}
}

dplist_t *dpl_insert_at_index(dplist_t *list, void *element, int index, bool insert_copy) {
	dplist_node_t *ref_at_index, *list_node;
	if (list == NULL) return NULL;
	list_node = malloc(sizeof(dplist_node_t));
	DPLIST_ERR_HANDLER(list_node == NULL, DPLIST_MEMORY_ERROR);
	///insert_copy==true --> element_copy() to make copy of the element. ELSE use the orginal 
	if(insert_copy == true){
		list_node ->element = list->element_copy(element);
	}
	// pointer drawing breakpoint
	  if (list->head == NULL) { // covers case 1
	      list_node->prev = NULL;
	      list_node->next = NULL;
	      list->head = list_node;
	      // pointer drawing breakpoint
	  } else if (index <= 0) { // covers case 2
	      list_node->prev = NULL;
	      list_node->next = list->head;
	      list->head->prev = list_node;
	      list->head = list_node;
	      // pointer drawing breakpoint
	  } else {
	      ref_at_index = dpl_get_reference_at_index(list, index);
	      assert(ref_at_index != NULL);
	      // pointer drawing breakpoint
	      if (index < dpl_size(list)) { // covers case 4
	          list_node->prev = ref_at_index->prev;
	          list_node->next = ref_at_index;
	          ref_at_index->prev->next = list_node;
	          ref_at_index->prev = list_node;
	          // pointer drawing breakpoint
	      } else { // covers case 3
	          assert(ref_at_index->next == NULL);
	          list_node->next = NULL;
	          list_node->prev = ref_at_index;
	          ref_at_index->next = list_node;
	          // pointer drawing breakpoint
	      }
	  }
	   return list;
}

dplist_t *dpl_remove_at_index(dplist_t *list, int index, bool free_element) {
    dplist_node_t* list_node, *store_prev, *ref_at_index, *store_next;

    if(list->head==NULL){
    	return list;}
    if(list==NULL){
    	return NULL;
    }
    //remove at the begining of the list
     if(index<=0){
     	if(free_element==true){
     		list_node = list->head;
     		list-> element_free(&list_node->element);
     	}
    	list_node = list->head;
    	list->head=list->head->next;
    	list->head->prev=NULL;
    	free(list_node);
    //remove at the middle of the list
    }else{
    	ref_at_index = dpl_get_reference_at_index(list,index); //sends us automatically to the requested node
    	assert(ref_at_index!=NULL);
    	if(index>0 && index<dpl_size(list)){
    		if(free_element==true){
    			list->element_free(&ref_at_index->element);
    		}
    		store_next = ref_at_index->next;
    		store_prev = ref_at_index->prev;
    		store_next -> prev = store_prev;
    		store_prev ->next = store_next;
    		free(ref_at_index);
    	} else if(index>=dpl_size(list)) {
    		if(free_element==true){
    			list->element_free(&ref_at_index->element);
    		}
    		store_prev = ref_at_index->prev;
    		free(ref_at_index);
    		store_prev->next =NULL;
    	}	
	}
	return list;
}

int dpl_size(dplist_t *list) { //DONE
	dplist_node_t* node;
	node = list->head;
	int counter=0;
	while(node!=NULL){
		counter++;
		node=node->next;
	}
	return counter;
}

dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index) { //DONE
	int count;
	dplist_node_t *dummy;
	DPLIST_ERR_HANDLER(list == NULL, DPLIST_INVALID_ERROR);
	if (list->head == NULL) return NULL;
	for (dummy = list->head, count = 0; dummy->next != NULL; dummy = dummy->next, count++) {
		if (count >= index) return dummy;
	}
	    return dummy;
}

void *dpl_get_element_at_index(dplist_t *list, int index) { //DONE
	//dplist_node_t* list_node;	
	//list_node=list->head;
	if(list==NULL){
		return 0;
	}
	dplist_node_t* list_node=list->head;
    if(index<=0){
		index=0;
		list_node = dpl_get_reference_at_index(list,index);	
	} 
	if(index>0 && index < dpl_size(list)){
		list_node = dpl_get_reference_at_index(list,index);
	}
    if(index>=dpl_size(list)){
		index = dpl_size(list) ;
		list_node = dpl_get_reference_at_index(list,index);
	}
	return list_node->element;
}

int dpl_get_index_of_element(dplist_t *list, void *element) { //DONE
	dplist_node_t* list_node;
	list_node=list->head;
	int index=0;
	if(list_node==NULL){
		return-1;
	}
	while(list_node!=NULL){
		if(list_node->element==element){
			return index;
		}else if(element !=list_node->element && index==dpl_size(list)-1){
			return -1;
		}
		index++;
		list_node=list_node->next;
	}
	return index;
}
