#include <stdlib.h>
#include "ADTRecTree.h"
#include "ADTMap.h"

Pointer trees = (Pointer)1;

Map values, left_subtrees, right_subtrees, sizes;

int compaire_pointers(Pointer a, Pointer b){
    return a - b;
}

struct rec_tree {
};

// Δημιουργεί και επιστρέφει ένα νέο δέντρο, με τιμή (στη ρίζα) value και υποδέντρα left και right.

RecTree rectree_create(Pointer value, RecTree left, RecTree right) {
    RecTree tree;
    //Αν δεν υπάρχει κάποιο δέντρο την πρώτη φορά που θα κληθεί η
    //rectree_create δημιουργούνται τα maps
    if((long)trees == 1) {
        values = map_create(compaire_pointers, NULL, NULL);
        left_subtrees = map_create(compaire_pointers, NULL, NULL);
        right_subtrees = map_create(compaire_pointers, NULL, NULL);
        sizes = map_create(compaire_pointers, NULL, NULL);
    }
    long size = 0;
    if(value == NULL) {
        tree = REC_TREE_EMPTY;
        return tree;
    }
    tree = trees;
    trees ++ ;   

    map_insert(values, tree, value);
    
    size ++;
    if(left != REC_TREE_EMPTY){
        size += (long)map_find(sizes, left);
    }
    map_insert(left_subtrees, tree, left);
    if(right != REC_TREE_EMPTY){
        size += (long)map_find(sizes, right);
    }
    map_insert(right_subtrees, tree, right);
    map_insert(sizes, tree, (Pointer)size);
    return tree;
}

// Επιστρέφει τον αριθμό στοιχείων που περιέχει το δέντρο.

int rectree_size(RecTree tree) {
    return (long)map_find(sizes, tree);
}

// Ελευθερώνει όλη τη μνήμη που δεσμεύει το δέντρο tree.

void rectree_destroy(RecTree tree) {
    if(tree != REC_TREE_EMPTY) {
        if(map_find(values, tree) != NULL){
            map_remove(values, tree);
            map_remove(sizes, tree);
            map_remove(right_subtrees, tree);
            map_remove(left_subtrees, tree);
        }
    }       
    if(map_size(values) == 0){
        map_destroy(values);
        map_destroy(left_subtrees);
        map_destroy(right_subtrees);
        map_destroy(sizes);
        trees = (Pointer)1;
    }
}

// Επιστρέφουν την τιμή (στη ρίζα), το αριστερό και το δεξί υποδέντρο του δέντρου tree.

Pointer rectree_value(RecTree tree) {
    return map_find(values, tree);
}

RecTree rectree_left(RecTree tree) {
    return map_find(left_subtrees, tree);
}

RecTree rectree_right(RecTree tree) {
    return map_find(right_subtrees, tree);
}