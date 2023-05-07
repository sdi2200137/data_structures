#include <stdlib.h>

#include "common_types.h"
#include "set_utils.h"
#include "ADTSet.h"

// Επιστρέφει την μοναδική τιμή του set που είναι ισοδύναμη με value,
// ή αν δεν υπάρχει, την μικρότερη τιμή του set που είναι μεγαλύτερη
// από value. Αν δεν υπάρχει καμία τότε επιστρέφει NULL.
Pointer set_find_eq_or_greater(Set set, Pointer value){
    Pointer found = set_find(set, value) ;
    if( found == NULL){
        set_insert(set, value) ;
        SetNode next = set_find_node(set, value) ;
        next = set_next(set, next) ;
        if(next == SET_EOF)
            return NULL ;
        found = set_node_value(set, next) ;
        set_remove(set, value) ;
    }
    return found ;
}

// Επιστρέφει την μοναδική τιμή του set που είναι ισοδύναμη με value,
// ή αν δεν υπάρχει, την μεγαλύτερη τιμή του set που είναι μικρότερη
// από value. Αν δεν υπάρχει καμία τότε επιστρέφει NULL.
Pointer set_find_eq_or_smaller(Set set, Pointer value){
    Pointer found = set_find(set, value) ;
    if( found == NULL){
        set_insert(set, value) ;
        SetNode previous = set_find_node(set, value) ;
        previous = set_previous(set, previous) ;
        if(previous == SET_BOF)
           return NULL ;
        found = set_node_value(set, previous) ;
        set_remove(set, value) ;
    }
    return found ;
}

