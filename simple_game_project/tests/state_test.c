//////////////////////////////////////////////////////////////////
//
// Test για το state.h module
//
//////////////////////////////////////////////////////////////////

#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing

#include "state.h"
#include "ADTSet.h"
#include "set_utils.h"

int compare_ints(Pointer a, Pointer b){
	int* first = a;
	int* second = b;
	return *first - *second;
}

void test_state_create() {

	State state = state_create();
	TEST_ASSERT(state != NULL);

	StateInfo info = state_info(state);
	TEST_ASSERT(info != NULL);

	TEST_ASSERT(info->playing);
	TEST_ASSERT(!info->paused);
	TEST_ASSERT(info->score == 0);

	// Προσθέστε επιπλέον ελέγχους

	List list = list_create(NULL);
	list = state_objects(state, 0, SCREEN_WIDTH);
	TEST_ASSERT(list_size(list) >= 0 && list_size(list) <= 20);
	Object object = list_node_value(list, list_first(list));
	TEST_ASSERT(object->rect.x >= 0 && object->rect.x <= SCREEN_WIDTH);

	List second_list = list_create(NULL);
	second_list = state_objects(state, 10 * SCREEN_WIDTH, 20 * SCREEN_WIDTH);
	TEST_ASSERT(list_size(second_list) == 0);
}

void test_state_update() {
	State state = state_create();
	TEST_ASSERT(state != NULL && state_info(state) != NULL);

	// Πληροφορίες για τα πλήκτρα (αρχικά κανένα δεν είναι πατημένο)
	struct key_state keys = { false, false, false, false, false, false, false };
	
	// Χωρίς κανένα πλήκτρο, η μπάλα μετακινείται 4 pixels δεξιά
	Rectangle old_rect = state_info(state)->ball->rect;
	state_update(state, &keys);
	Rectangle new_rect = state_info(state)->ball->rect;

	TEST_ASSERT( new_rect.x == old_rect.x + 4 );

	// Με πατημένο το δεξί βέλος, η μπάλα μετακινείται 6 pixels δεξιά
	keys.right = true;
	old_rect = state_info(state)->ball->rect;
	state_update(state, &keys);
	new_rect = state_info(state)->ball->rect;

	TEST_CHECK( new_rect.x == old_rect.x + 6 );

	// Προσθέστε επιπλέον ελέγχους

	//Με πατημένο το αριστερό βέλος, η μπάλα μετακινείται 1 pixels δεξιά
	keys.right = false;
	keys.left = true;
	old_rect = state_info(state)->ball->rect;
	state_update(state, &keys);
	new_rect = state_info(state)->ball->rect;

	TEST_CHECK( new_rect.x == old_rect.x + 1 );

	keys.up = true;
	float speed = state_info(state)->ball->vert_speed;
	old_rect = state_info(state)->ball->rect;
	state_update(state, &keys);
	new_rect = state_info(state)->ball->rect;
	TEST_ASSERT(state_info(state)->ball->vert_mov == JUMPING);
	//Μετακινείται προς τα πάνω τόσα pixels όσα η κατακόρυφη ταχύτητά της.
	TEST_ASSERT(new_rect.y == old_rect.y - speed);
	speed = state_info(state)->ball->vert_speed;
	//Αν είναι πατημένο το πάνω βέλος μπαίνει σε κατάσταση άλματος με αρχική ταχύτητα 17
	TEST_ASSERT(speed == 17);



	//με πατημενο το p, το παιχνιδι γινεται paused
	keys.p = true;
	state_update(state, &keys);
	TEST_ASSERT(state_info(state)->paused);
	TEST_ASSERT(state_info(state)->score == 0);

}

void test_set_utils(){
	Set set = set_create(compare_ints, NULL);
	int number = 10 ;
	int* ptr = &number ;
	set_insert(set, ptr) ;
	int* temp =  set_find_eq_or_greater(set, ptr);
	TEST_ASSERT(temp != NULL);
	TEST_ASSERT(*temp == 10);

	int number2 = 8;
	temp = set_find_eq_or_greater(set, &number2);
	TEST_ASSERT(temp != NULL);
	TEST_ASSERT(*temp == 10);

	number2 = 20;
	temp = set_find_eq_or_greater(set, &number2);
	TEST_ASSERT(temp == NULL);

	number2 = 10;
	temp = set_find_eq_or_smaller(set, &number2);
	TEST_ASSERT(temp != NULL);
	TEST_ASSERT(*temp == 10);

	number2 = 5;
	temp = set_find_eq_or_smaller(set, &number2);
	TEST_ASSERT(temp == NULL);

	number2 = 30;
	temp = set_find_eq_or_smaller(set, &number2);
	TEST_ASSERT(temp != NULL);
	TEST_ASSERT(*temp == 10);



}

// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
	{ "test_state_create", test_state_create },
	{ "test_state_update", test_state_update },
	{ "test_set_utils", test_set_utils },

	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};