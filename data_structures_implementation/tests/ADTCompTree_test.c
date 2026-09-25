#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing

#include "ADTRecTree.h"
#include "ADTCompTree.h"

void test_create(void){
    int value = 0;
    CompTree tree = comptree_create(&value, COMP_TREE_EMPTY, COMP_TREE_EMPTY);
    TEST_ASSERT(tree != COMP_TREE_EMPTY);
    TEST_ASSERT(comptree_size(tree) == 1);
    TEST_ASSERT(*(int*)comptree_value(tree) == 0);
    TEST_ASSERT(comptree_left(tree) == COMP_TREE_EMPTY);
    TEST_ASSERT(comptree_right(tree) == COMP_TREE_EMPTY);
    comptree_destroy(tree);

    tree = comptree_create(NULL , COMP_TREE_EMPTY, COMP_TREE_EMPTY);
    TEST_ASSERT(comptree_size(tree) == 0);
    TEST_ASSERT(tree == COMP_TREE_EMPTY);
    TEST_ASSERT(comptree_value(tree) == NULL);
    TEST_ASSERT(comptree_right(tree) == COMP_TREE_EMPTY);
    TEST_ASSERT(comptree_left(tree) == COMP_TREE_EMPTY);
    comptree_destroy(tree);

    int second = 5;
    int third = 10;
    CompTree left_subtree = comptree_create(&second, COMP_TREE_EMPTY, COMP_TREE_EMPTY);
    CompTree right_subtree = comptree_create(&third, COMP_TREE_EMPTY, COMP_TREE_EMPTY);
    tree = comptree_create(&value, left_subtree, right_subtree);
    TEST_ASSERT(comptree_size(tree) == 3);
    TEST_ASSERT(comptree_left(tree) == left_subtree);
    TEST_ASSERT(comptree_right(tree) == right_subtree);
    comptree_destroy(tree);
    comptree_destroy(left_subtree);
    comptree_destroy(right_subtree);

}

void test_insert(void){
    int value = 4;
    int numb = 10;
    CompTree tree_3 = comptree_create(&numb, COMP_TREE_EMPTY, COMP_TREE_EMPTY);
    CompTree tree_1 = comptree_create(&numb, tree_3, COMP_TREE_EMPTY);
    CompTree tree_2 = comptree_create(&numb, COMP_TREE_EMPTY, COMP_TREE_EMPTY);
    CompTree tree_0 = comptree_create(&numb, tree_1, tree_2);
    CompTree tree_4 = comptree_insert_last(tree_0, &value);
    TEST_ASSERT(*(int*)comptree_value(comptree_right(comptree_left(tree_4))) == value);
    TEST_ASSERT(comptree_right(tree_4) == tree_2);
    TEST_ASSERT(comptree_left(comptree_left(tree_4)) == tree_3);

    tree_4 = comptree_remove_last(tree_4);
    TEST_ASSERT(comptree_right(comptree_left(tree_4)) == COMP_TREE_EMPTY);
    comptree_destroy(comptree_left(tree_4));
    comptree_destroy(tree_2);
    comptree_destroy(tree_3);
    comptree_destroy(tree_4);
}

// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
	{ "comptree_create", test_create },
    { "comptree_insert_last", test_insert },

	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};