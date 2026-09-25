///////////////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT CompTree μέσω RecTree
//
///////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include "ADTRecTree.h"
#include "ADTCompTree.h"
#include "ADTRecTree_utils.h"


// Δημιουργεί και επιστρέφει ένα νέο δέντρο, με τιμή (στη ρίζα) value και υποδέντρα left και right.

CompTree comptree_create(Pointer value, CompTree left, CompTree right){
    RecTree tree = rectree_create(value, (RecTree)left, (RecTree)right);
    return (CompTree)tree;
}

// Επιστρέφει τον αριθμό στοιχείων που περιέχει το δέντρο (0 για κενά δέντρα).

int comptree_size(CompTree tree){
    RecTree rectree = (RecTree)tree;
    return rectree_size(rectree);
}

// Ελευθερώνει τη μνήμη που δεσμεύει το δέντρο tree (ΔΕΝ καταστρέφει τα υποδέντρα του).

void comptree_destroy(CompTree tree){
    RecTree rectree = (RecTree)tree;
    rectree_destroy(rectree);
}

// Επιστρέφουν την τιμή (στη ρίζα), το αριστερό και το δεξί υποδέντρο του δέντρου tree.

Pointer comptree_value(CompTree tree){
    RecTree rectree = (RecTree)tree;
    return rectree_value(rectree);
}

CompTree comptree_left(CompTree tree){
    RecTree rectree = (RecTree)tree;
    return (CompTree)rectree_left(rectree);
}

CompTree comptree_right(CompTree tree){
    RecTree rectree = (RecTree)tree;
    return (CompTree)rectree_right(rectree);
}

// Δημιουργεί και επιστρέφει ένα νέο δέντρο που προκύπτει από το tree μετά την προσθήκη της
// τιμής value στο "τέλος" του δέντρου (ώστε να παραμείνει complete). Τυχόν υποδέντρα που
// "μεταβάλλονται" κατά την προσθήκη αυτή καταστρέφονται αυτόματα.

CompTree comptree_insert_last(CompTree tree, Pointer value){
    RecTree rectree = (RecTree)tree;
    RecTree subtree = rectree_create(value, REC_TREE_EMPTY, REC_TREE_EMPTY);
    return (CompTree)rectree_replace_subtree(rectree, rectree_size(rectree), subtree);
}

// Δημιουργεί και επιστρέφει ένα νέο δέντρο που προκύπτει από το tree μετά την διαγραφή του
// "τελευταίου" υποδέντρου του (ώστε να παραμείνει complete). Το υποδέντρο που αφαιρείται, και
// τυχόν υποδέντρα που "μεταβάλλονται" κατά τη διαγραφή αυτή καταστρέφονται αυτόματα.

CompTree comptree_remove_last(CompTree tree){
    RecTree rectree = (RecTree)tree;
    return (CompTree)rectree_replace_subtree(rectree, rectree_size(rectree)-1, REC_TREE_EMPTY);
}

CompTree comptree_get_subtree(CompTree tree, int pos){
    RecTree rectree = (RecTree)tree;
    return (CompTree)rectree_get_subtree(rectree, pos);
}

CompTree comptree_replace_subtree(CompTree tree, int pos, CompTree subtree) {
    RecTree rectree = (RecTree)tree;
    RecTree recsubtree = (RecTree)subtree;
    return (CompTree)rectree_replace_subtree(rectree, pos, recsubtree); 
}