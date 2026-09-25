#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing

#include "ADTRecTree.h"


// Επιστρέφει έναν ακέραιο σε νέα μνήμη με τιμή value
int* create_int(int value) {
	int* p = malloc(sizeof(int));
	*p = value;
	return p;
}



void test_create(void){
    int value = 0;
    RecTree tree = rectree_create(&value, REC_TREE_EMPTY, REC_TREE_EMPTY);
    TEST_ASSERT(tree != REC_TREE_EMPTY);
    TEST_ASSERT(rectree_size(tree) == 1);
    TEST_ASSERT(*(int*)rectree_value(tree) == 0);
    TEST_ASSERT(rectree_left(tree) == REC_TREE_EMPTY);
    TEST_ASSERT(rectree_right(tree) == REC_TREE_EMPTY);
    rectree_destroy(tree);

    tree = rectree_create(NULL , REC_TREE_EMPTY, REC_TREE_EMPTY);
    TEST_ASSERT(rectree_size(tree) == 0);
    TEST_ASSERT(tree == REC_TREE_EMPTY);
    TEST_ASSERT(rectree_value(tree) == NULL);
    TEST_ASSERT(rectree_right(tree) == REC_TREE_EMPTY);
    TEST_ASSERT(rectree_left(tree) == REC_TREE_EMPTY);
    rectree_destroy(tree);

    int second = 5;
    int third = 10;
    RecTree left_subtree = rectree_create(&second, REC_TREE_EMPTY, REC_TREE_EMPTY);
    RecTree right_subtree = rectree_create(&third, REC_TREE_EMPTY, REC_TREE_EMPTY);
    tree = rectree_create(&value, left_subtree, right_subtree);
    TEST_ASSERT(rectree_size(tree) == 3);
    TEST_ASSERT(rectree_left(tree) == left_subtree);
    TEST_ASSERT(rectree_right(tree) == right_subtree);
    rectree_destroy(tree);
    rectree_destroy(left_subtree);
    rectree_destroy(right_subtree);

}


// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
	{ "rectree_create", test_create },

	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};