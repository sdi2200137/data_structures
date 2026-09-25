///////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Priority Queue μέσω σωρού.
//
///////////////////////////////////////////////////////////

#include <stdlib.h>
#include <assert.h>

#include "ADTPriorityQueue.h"
#include "ADTVector.h"
#include "ADTCompTree.h"			// Η υλοποίηση του PriorityQueue χρησιμοποιεί CompTree

// Ενα PriorityQueue είναι pointer σε αυτό το struct
struct priority_queue {
	CompTree comptree;				// Τα δεδομένα, σε CompTree
	CompareFunc compare;		// Η διάταξη
	DestroyFunc destroy_value;	// Συνάρτηση που καταστρέφει ένα στοιχείο του vector.
};


// Βοηθητικές συναρτήσεις ////////////////////////////////////////////////////////////////////////////

// Προσοχή: στην αναπαράσταση ενός complete binary tree με πίνακα, είναι βολικό τα ids των κόμβων να
// ξεκινάνε από το 1 (ρίζα), το οποίο απλοποιεί τις φόρμουλες για εύρεση πατέρα/παιδιών. Οι θέσεις
// ενός vector όμως ξεκινάνε από το 0. Θα μπορούσαμε απλά να αφήσουμε μία θέση κενή, αλλά δεν είναι ανάγκη,
// μπορούμε απλά να αφαιρούμε 1 όταν διαβάζουμε/γράφουμε στο vector. Για απλοποίηση του κώδικα, η
// πρόσβαση στα στοιχεία του vector γίνεται από τις παρακάτω 2 βοηθητικές συναρτήσεις.

// Επιστρέφει την τιμή του κόμβου node_id

static Pointer node_value(PriorityQueue pqueue, int node_id) {
	// τα node_ids είναι 1-based, το node_id αποθηκεύεται στη θέση node_id - 1

	return comptree_value(comptree_get_subtree(pqueue->comptree, node_id-1));
}

// Ανταλλάσει τις τιμές των κόμβων node_id1 και node_id2

static void node_swap(PriorityQueue pqueue, int node_id1, int node_id2) {
	// τα node_ids είναι 1-based, το node_id αποθηκεύεται στη θέση node_id - 1

    CompTree temp1 = comptree_get_subtree(pqueue->comptree, node_id1-1);
    CompTree temp2 = comptree_get_subtree(pqueue->comptree, node_id2-1);
    CompTree subtree1 = comptree_create(comptree_value(temp2), comptree_left(temp1), comptree_right(temp1));
    CompTree subtree2 = comptree_create(comptree_value(temp1), comptree_left(temp2), comptree_right(temp2));
    pqueue->comptree = comptree_replace_subtree(pqueue->comptree, node_id1-1, subtree1);
    pqueue->comptree = comptree_replace_subtree(pqueue->comptree, node_id2-1, subtree2);
}

// Αποκαθιστά την ιδιότητα του σωρού.
// Πριν: όλοι οι κόμβοι ικανοποιούν την ιδιότητα του σωρού, εκτός από
//       τον node_id που μπορεί να είναι _μεγαλύτερος_ από τον πατέρα του.
// Μετά: όλοι οι κόμβοι ικανοποιούν την ιδιότητα του σωρού.

static void bubble_up(PriorityQueue pqueue, int node_id) {
	// Αν φτάσαμε στη ρίζα, σταματάμε
	if (node_id == 1)
		return;

	int parent = node_id / 2;		// Ο πατέρας του κόμβου. Τα node_ids είναι 1-based

	// Αν ο πατέρας έχει μικρότερη τιμή από τον κόμβο, swap και συνεχίζουμε αναδρομικά προς τα πάνω
	if (pqueue->compare(node_value(pqueue, parent), node_value(pqueue, node_id)) < 0) {
		node_swap(pqueue, parent, node_id);
		bubble_up(pqueue, parent);
	}
}

// Αποκαθιστά την ιδιότητα του σωρού.
// Πριν: όλοι οι κόμβοι ικανοποιούν την ιδιότητα του σωρού, εκτός από τον
//       node_id που μπορεί να είναι _μικρότερος_ από κάποιο από τα παιδιά του.
// Μετά: όλοι οι κόμβοι ικανοποιούν την ιδιότητα του σωρού.

static void bubble_down(PriorityQueue pqueue, int node_id) {
	// βρίσκουμε τα παιδιά του κόμβου (αν δεν υπάρχουν σταματάμε)
	int left_child = 2 * node_id;
	int right_child = left_child + 1;

	int size = pqueue_size(pqueue);
	if (left_child > size)
		return;

	// βρίσκουμε το μέγιστο από τα 2 παιδιά
	int max_child = left_child;
	if (right_child <= size && pqueue->compare(node_value(pqueue, left_child), node_value(pqueue, right_child)) < 0)
			max_child = right_child;

	// Αν ο κόμβος είναι μικρότερος από το μέγιστο παιδί, swap και συνεχίζουμε προς τα κάτω
	if (pqueue->compare(node_value(pqueue, node_id), node_value(pqueue, max_child)) < 0) {
		node_swap(pqueue, node_id, max_child);
		bubble_down(pqueue, max_child);
	}
}

static CompTree comp_bubble_down(CompTree tree, PriorityQueue pqueue) {
	// βρίσκουμε τα παιδιά του κόμβου (αν δεν υπάρχουν σταματάμε)
	CompTree left_child = comptree_left(tree);
	CompTree right_child = comptree_right(tree);

	// int size = pqueue_size(pqueue);
	if (left_child == COMP_TREE_EMPTY)
		return tree;

	// βρίσκουμε το μέγιστο από τα 2 παιδιά
	CompTree max_child = left_child;
	int pos = 1;
	if (right_child != COMP_TREE_EMPTY && pqueue->compare(comptree_value(left_child), comptree_value(right_child)) < 0){
			max_child = right_child;
			pos = 2;
	}

	// Αν ο κόμβος είναι μικρότερος από το μέγιστο παιδί, swap και συνεχίζουμε προς τα κάτω
	if (pqueue->compare(comptree_value(tree), comptree_value(max_child)) < 0) {
		CompTree sub1 = comptree_create(comptree_value(tree), comptree_left(max_child), comptree_right(max_child));
    	CompTree sub2 = comptree_create(comptree_value(max_child), comptree_left(tree), comptree_right(tree));
		CompTree new_tree = comptree_replace_subtree(tree, 0, sub2);
		new_tree = comptree_replace_subtree(new_tree, pos, sub1);
		CompTree next = comptree_left(new_tree);
		if(pos == 2)
			next = comptree_right(new_tree);
		comp_bubble_down(next, pqueue);
		return new_tree;
	}
	return tree;
}


static CompTree build_tree(Vector values, int pos, PriorityQueue pqueue) {
	if(pos > vector_size(values)){
		return COMP_TREE_EMPTY;
	}
	if((pos * 2) > vector_size(values)){
		CompTree tree = comptree_create(vector_get_at(values, pos-1), COMP_TREE_EMPTY, COMP_TREE_EMPTY);
		return tree;
	}
	else {
		CompTree left = build_tree(values, (2*pos), pqueue);
		CompTree right = build_tree(values, (2*pos)+1, pqueue);
		CompTree tree = comptree_create(vector_get_at(values, pos-1), left, right);
		tree = comp_bubble_down(tree, pqueue);
		return tree;
	} 
}

// Αρχικοποιεί το σωρό από τα στοιχεία του vector values.

static void naive_heapify(PriorityQueue pqueue, Vector values) {
	// Απλά κάνουμε insert τα στοιχεία ένα ένα.
	// TODO: υπάρχει πιο αποδοτικός τρόπος να γίνει αυτό!
	pqueue->comptree = build_tree(values, 1, pqueue);
}


// Συναρτήσεις του ADTPriorityQueue //////////////////////////////////////////////////

PriorityQueue pqueue_create(CompareFunc compare, DestroyFunc destroy_value, Vector values) {
	assert(compare != NULL);	// LCOV_EXCL_LINE

	PriorityQueue pqueue = malloc(sizeof(*pqueue));
	pqueue->compare = compare;
	pqueue->destroy_value = destroy_value;

	// Δημιουργία του vector που αποθηκεύει τα στοιχεία.
	// ΠΡΟΣΟΧΗ: ΔΕΝ περνάμε την destroy_value στο vector!
	// Αν την περάσουμε θα καλείται όταν κάνουμε swap 2 στοιχεία, το οποίο δεν το επιθυμούμε.
	pqueue->comptree = comptree_create(NULL, COMP_TREE_EMPTY, COMP_TREE_EMPTY);

	// Αν values != NULL, αρχικοποιούμε το σωρό.
	if (values != NULL)
		naive_heapify(pqueue, values);

	return pqueue;
}

int pqueue_size(PriorityQueue pqueue) {
	return comptree_size(pqueue->comptree);
}

Pointer pqueue_max(PriorityQueue pqueue) {
	return node_value(pqueue, 1);		// root
}

void pqueue_insert(PriorityQueue pqueue, Pointer value) {
	// Προσθέτουμε την τιμή στο τέλος το σωρού
    pqueue->comptree = comptree_insert_last(pqueue->comptree, value);

 	// Ολοι οι κόμβοι ικανοποιούν την ιδιότητα του σωρού εκτός από τον τελευταίο, που μπορεί να είναι
	// μεγαλύτερος από τον πατέρα του. Αρα μπορούμε να επαναφέρουμε την ιδιότητα του σωρού καλώντας
	// τη bubble_up γα τον τελευταίο κόμβο (του οποίου το 1-based id ισούται με το νέο μέγεθος του σωρού).
	bubble_up(pqueue, pqueue_size(pqueue));
}

void pqueue_remove_max(PriorityQueue pqueue) {
	int last_node = pqueue_size(pqueue);
	assert(last_node != 0);		// LCOV_EXCL_LINE

	// Destroy την τιμή που αφαιρείται
	if (pqueue->destroy_value != NULL)
		pqueue->destroy_value(pqueue_max(pqueue));

	// Αντικαθιστούμε τον πρώτο κόμβο με τον τελευταίο και αφαιρούμε τον τελευταίο
	node_swap(pqueue, 1, last_node);
    pqueue->comptree = comptree_remove_last(pqueue->comptree);

 	// Ολοι οι κόμβοι ικανοποιούν την ιδιότητα του σωρού εκτός από τη νέα ρίζα
 	// που μπορεί να είναι μικρότερη από κάποιο παιδί της. Αρα μπορούμε να
 	// επαναφέρουμε την ιδιότητα του σωρού καλώντας τη bubble_down για τη ρίζα.
	bubble_down(pqueue, 1);
}

DestroyFunc pqueue_set_destroy_value(PriorityQueue pqueue, DestroyFunc destroy_value) {
	DestroyFunc old = pqueue->destroy_value;
	pqueue->destroy_value = destroy_value;
	return old;
}

void pqueue_destroy(PriorityQueue pqueue) {
	// Αντί να κάνουμε εμείς destroy τα στοιχεία, είναι απλούστερο να
	// προσθέσουμε τη destroy_value στο vector ώστε να κληθεί κατά το vector_destroy.
    comptree_destroy(pqueue->comptree);

	free(pqueue);
}