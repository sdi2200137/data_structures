#include <stdlib.h>
#include "ADTRecTree.h"
#include "ADTRecTree_utils.h"

// Επιστρέφει το υποδέντρο του tree στη θέση pos. Η αρίθμηση των θέσεων ξεκινάει
// από το 0 (ρίζα) και κινείται κατά επίπεδα, τα παιδιά της ρίζας έχουν θέση 1
// και 2, τα δικά τους παιδιά έχουν θέσεις 3,4,5,6, κλπ. Αν το υποδέντρο δεν
// υπάρχει θα επιστρέφεται REC_TREE_EMPTY. 

RecTree rectree_get_subtree(RecTree tree, int pos){
    if(pos > 2){
        int parent_pos = pos / 2;
        int is_left = 1;
        if((pos % 2) == 0) {
            parent_pos -- ;
            is_left = 0;
        }
        RecTree father = rectree_get_subtree(tree , parent_pos);
        if(father == REC_TREE_EMPTY)
            return REC_TREE_EMPTY;
        if(is_left)
            return rectree_left(father);
        return rectree_right(father);
    }
    else {
        if(pos == 2)
            return rectree_right(tree);
        else if(pos == 1)
            return rectree_left(tree);
        else if(pos == 0)
            return tree;
    }
    return REC_TREE_EMPTY;
}

// Δημιουργεί και επιστρέφει ένα νέο δέντρο, το οποίο προκύπτει αντικαθιστώντας
// το υποδέντρο του tree στη θέση pos με το subtree που δίνεται.  Η θέση pos
// πρέπει να αντιστοιχεί είτε σε υπάρχον κόμβο (που αντικαθίσταται), είτε στο
// κενό παιδί ενός υπάρχοντος κόμβου (οπότε προστίθεται εκεί το subtree).  Αν το
// subtree τοποθετείται στη ρίζα (pos == 0) τότε επιστρέφεται το ίδιο το subtree.
//
// Η συνάρτηση καταστρέφει αυτόματα τόσο το παλιό υποδέντρο που αντικαθίσταται
// (αν υπάρχει), καθώς και όλους τους προγόνους του που μεταβάλλονται (οπότε
// ξαναδημιουργούνται).

RecTree rectree_replace_subtree(RecTree tree, int pos, RecTree subtree) {
    if(!pos){
        // if(tree != REC_TREE_EMPTY)
        //     rectree_destroy(tree);
        return subtree;
    }
    int parent_pos = pos / 2;
    int is_left = 1;
    if((pos % 2) == 0) {
        parent_pos -- ;
        is_left = 0;
    }
    RecTree father = rectree_get_subtree(tree , parent_pos);
    Pointer value = rectree_value(father);
    RecTree rem_sub ; //remaining subtree
    RecTree new_parent ;
    if(is_left) {
        rem_sub = rectree_right(father);
        // if(rectree_left(father) != REC_TREE_EMPTY)
        //     rectree_destroy(rectree_left(father));
        new_parent = rectree_create(value, subtree, rem_sub);
    }
    else {
        rem_sub = rectree_left(father);
        rectree_destroy(rectree_right(father));
        new_parent = rectree_create(value, rem_sub, subtree);
    }
    RecTree to_return = rectree_replace_subtree(tree, parent_pos, new_parent);
    return to_return;
}