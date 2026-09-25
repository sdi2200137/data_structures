///////////////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT RecTree μέσω links
//
///////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include "ADTRecTree.h"


// Προς υλοποίηση...


//  Ένα δέντρο είναι pointer σε αυτό το struct
struct rec_tree {
	Pointer root ;
	int size ;
	RecTree left;
	RecTree right;
};


// Δημιουργεί και επιστρέφει ένα νέο δέντρο, με τιμή (στη ρίζα) value και υποδέντρα left και right.

RecTree rectree_create(Pointer value, RecTree left, RecTree right) {
	RecTree tree = malloc(sizeof(struct rec_tree));
	tree->root = value;
	if(value != NULL)
		tree->size = 1;
	else {
		//Από τον ορισμό του αναδρομικού δέντρου αν αυτό δεν περιέχει στοιχείο ως ρίζα
		//τότε το δέντρο ειναι κενό και δεν υπαρχει λογος να ελέγξουμε για αριστερό 
		//δεξί υποδέντρο ή το μεγεθός του
		rectree_destroy(tree);
		return REC_TREE_EMPTY;
	}
	//στο μέγεθος του δέντρου προσθέτουμε το μέγεθος των υποδέντρων του αν
	//δεν είναι κενά 
	if(left != REC_TREE_EMPTY)
		tree->size += left->size;
	tree->left = left;

	if(right != REC_TREE_EMPTY)
		tree->size += right->size;
	tree->right = right;
	
	return tree;
}

// Επιστρέφει τον αριθμό στοιχείων που περιέχει το δέντρο.

int rectree_size(RecTree tree) {
	if(tree == REC_TREE_EMPTY)
		return 0;
	return tree->size;
}

// Ελευθερώνει όλη τη μνήμη που δεσμεύει το δέντρο tree.

void rectree_destroy(RecTree tree) {
	if(tree != REC_TREE_EMPTY){
		free(tree);
	}
}

// Επιστρέφουν την τιμή (στη ρίζα), το αριστερό και το δεξί υποδέντρο του δέντρου tree.

Pointer rectree_value(RecTree tree) {
	if(tree == REC_TREE_EMPTY)
		return NULL;
	return tree->root;
}

RecTree rectree_left(RecTree tree) {
	if(tree == REC_TREE_EMPTY)
		return REC_TREE_EMPTY;
	return tree->left;
}

RecTree rectree_right(RecTree tree) {
	if(tree == REC_TREE_EMPTY)
		return REC_TREE_EMPTY;
	return tree->right;
}

