#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing

#include "ADTRecTree.h"
#include "ADTRecTree_utils.h"

void test_replace(void){
    int num = 6;
    RecTree tree_6 = rectree_create(&num, REC_TREE_EMPTY, REC_TREE_EMPTY);
    RecTree tree_5 = rectree_create(&num, REC_TREE_EMPTY, REC_TREE_EMPTY);
    RecTree tree_2 = rectree_create(&num, tree_5, tree_6);
    RecTree tree_4 = rectree_create(&num, REC_TREE_EMPTY, REC_TREE_EMPTY);
    RecTree tree_3 = rectree_create(&num, REC_TREE_EMPTY, REC_TREE_EMPTY);
    RecTree tree_1 = rectree_create(&num, tree_3, tree_4);
    RecTree tree_0 = rectree_create(&num, tree_1, tree_2);
    TEST_ASSERT(rectree_get_subtree(tree_0, 6) == tree_6);
    TEST_ASSERT(rectree_get_subtree(tree_2, 2) == tree_6);
    TEST_ASSERT(rectree_get_subtree(tree_0, 0) == tree_0);
    TEST_ASSERT(rectree_get_subtree(tree_0, 10) == REC_TREE_EMPTY);

    RecTree new_subtree = rectree_create(&num, REC_TREE_EMPTY, REC_TREE_EMPTY);
    RecTree new_tree = rectree_replace_subtree(tree_0, 2, new_subtree);
    TEST_ASSERT(rectree_right(new_tree) == new_subtree);
    TEST_ASSERT(tree_5 != REC_TREE_EMPTY);
    TEST_ASSERT(rectree_left(new_tree) == tree_1);
    rectree_destroy(tree_5);
    rectree_destroy(tree_6);
    rectree_destroy(tree_1);
    rectree_destroy(tree_3);
    rectree_destroy(tree_4);
    rectree_destroy(new_subtree);
    rectree_destroy(new_tree);
}










// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
	{ "rectree_replace_subtre", test_replace },

	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};