
#include <stdlib.h>

#include "ADTVector.h"
#include "ADTList.h"
#include "state.h"


// Οι ολοκληρωμένες πληροφορίες της κατάστασης του παιχνιδιού.
// Ο τύπος State είναι pointer σε αυτό το struct, αλλά το ίδιο το struct
// δεν είναι ορατό στον χρήστη.

struct state {
	Vector objects;			// περιέχει στοιχεία Object (Πλατφόρμες, Αστέρια)
	struct state_info info;	// Γενικές πληροφορίες για την κατάσταση του παιχνιδιού
	float speed_factor;		// Πολλαπλασιαστής ταχύτητς (1 = κανονική ταχύτητα, 2 = διπλάσια, κλπ)
};


// Δημιουργεί και επιστρέφει ένα αντικείμενο

static Object create_object(ObjectType type, float x, float y, float width, float height, VerticalMovement vert_mov, float speed, bool unstable) {
	Object obj = malloc(sizeof(*obj));
	obj->type = type;
	obj->rect.x = x;
	obj->rect.y = y;
	obj->rect.width = width;
	obj->rect.height = height;
	obj->vert_mov = vert_mov;
	obj->vert_speed = speed;
	obj->unstable = unstable;
	return obj;
}

// Προσθέτει αντικείμενα στην πίστα (η οποία μπορεί να περιέχει ήδη αντικείμενα).
// Τα αντικείμενα ξεκινάνε από x = start_x, και επεκτείνονται προς τα δεξιά.
//
// ΠΡΟΣΟΧΗ: όλα τα αντικείμενα έχουν συντεταγμένες x,y σε ένα
// καρτεσιανό επίπεδο.
// - Στο άξονα x το 0 είναι η αρχή της πίστας και οι συντεταγμένες
//   μεγαλώνουν προς τα δεξιά.
//
// - Στον άξονα y το 0 είναι το πάνω μέρος της πίστας, και οι συντεταγμένες μεγαλώνουν
//   προς τα _κάτω_. Η πίστα έχει σταθερό ύψος, οπότε όλες οι
//   συντεταγμένες y είναι ανάμεσα στο 0 και το SCREEN_HEIGHT.
//
// Πέρα από τις συντεταγμένες, αποθηκεύουμε και τις διαστάσεις width,height
// κάθε αντικειμένου. Τα x,y,width,height ορίζουν ένα παραλληλόγραμμο, οπότε
// μπορούν να αποθηκευτούν όλα μαζί στο obj->rect τύπου Rectangle (ορίζεται
// στο include/raylib.h). Τα x,y αναφέρονται στην πάνω-αριστερά γωνία του Rectangle.

static void add_objects(State state, float start_x) {
	// Προσθέτουμε PLATFORM_NUM πλατφόρμες, με τυχαία χαρακτηριστικά.

	for (int i = 0; i < PLATFORM_NUM; i++) {
		Object platform = create_object(
			PLATFORM,
			start_x + 150 + rand() % 80,						// x με τυχαία απόσταση από το προηγούμενο στο διάστημα [150, 230]
			SCREEN_HEIGHT/4 + rand() % SCREEN_HEIGHT/2,			// y τυχαία στο διάστημα [SCREEN_HEIGHT/4, 3*SCREEN_HEIGHT/4]
			i == 0 ? 250 : 50 + rand()%200,						// πλάτος τυχαία στο διάστημα [50, 250] (η πρώτη πάντα 250)
			20,													// ύψος
			i < 3 || rand() % 2 == 0 ? MOVING_UP : MOVING_DOWN,	// τυχαία αρχική κίνηση (οι πρώτες 3 πάντα πάνω)
			0.6 + 3*(rand()%100)/100,							// ταχύτητα τυχαία στο διάστημα [0.6, 3.6]
			i > 0 && (rand() % 10) == 0							// το 10% (τυχαία) των πλατφορμών είναι ασταθείς (εκτός από την πρώτη)
		);
		vector_insert_last(state->objects, platform);

		// Στο 50% των πλατφορμών (τυχαία), εκτός της πρώτης, προσθέτουμε αστέρι
		if(i != 0 && rand() % 2 == 0) {
			Object star = create_object(
				STAR,
				start_x + 200 + rand() % 60,				// x με τυχαία απόσταση από την προηγούμενη πλατφόρμα στο διάστημα [200,260]
				SCREEN_HEIGHT/8 + rand() % SCREEN_HEIGHT/2,	// y τυχαία στο διάστημα [SCREEN_HEIGHT/8, 5*SCREEN_HEIGHT/8]
				30, 30,										// πλάτος, ύψος
				IDLE,										// χωρίς κίνηση
				0,										 	// ταχύτητα 0
				false										// 'unstable' πάντα false για τα αστέρια
			);
			vector_insert_last(state->objects, star);
		}

		start_x = platform->rect.x + platform->rect.width;	// μετακίνηση των επόμενων αντικειμένων προς τα δεξιά
	}
}

// Δημιουργεί και επιστρέφει την αρχική κατάσταση του παιχνιδιού

State state_create() {
	// Δημιουργία του state
	State state = malloc(sizeof(*state));

	// Γενικές πληροφορίες
	state->info.playing = true;				// Το παιχνίδι ξεκινάει αμέσως
	state->info.paused = false;				// Χωρίς να είναι paused.
	state->speed_factor = 1;				// Κανονική ταχύτητα
	state->info.score = 0;				// Αρχικό σκορ 0

	// Δημιουργούμε το vector των αντικειμένων, και προσθέτουμε αντικείμενα
	// ξεκινώντας από start_x = 0.
	state->objects = vector_create(0, NULL);
	add_objects(state, 0);

	// Δημιουργούμε την μπάλα τοποθετώντας τη πάνω στην πρώτη πλατφόρμα
	Object first_platform = vector_get_at(state->objects, 0);
	state->info.ball = create_object(
		BALL,
		first_platform->rect.x,			// x στην αρχή της πλατφόρμας
		first_platform->rect.y - 40,	// y πάνω στην πλατφόρμα
		45, 45,							// πλάτος, ύψος
		IDLE,							// χωρίς αρχική κατακόρυφη κίνηση
		0,								// αρχική ταχύτητα 0
		false							// "unstable" πάντα false για την μπάλα
	);

	return state;
}

// Επιστρέφει τις βασικές πληροφορίες του παιχνιδιού στην κατάσταση state

StateInfo state_info(State state) {
	return &(state->info);
}

// Επιστρέφει μια λίστα με όλα τα αντικείμενα του παιχνιδιού στην κατάσταση state,
// των οποίων η συντεταγμένη x είναι ανάμεσα στο x_from και x_to.

List state_objects(State state, float x_from, float x_to) {
	List list = list_create(NULL);
	ListNode list_node = LIST_BOF ;
	for( VectorNode vec_node =  vector_first(state->objects);
		 vec_node != VECTOR_EOF ;
		 vec_node = vector_next(state->objects, vec_node)) {
			Object object = vector_node_value(state->objects, vec_node);
			if( (object->rect.x >= x_from) && (object->rect.x <= x_to)) {
				list_insert_next(list, list_node, object);
			}
	}
	return list;
}

// Ενημερώνει την κατάσταση state του παιχνιδιού μετά την πάροδο 1 frame.
// Το keys περιέχει τα πλήκτρα τα οποία ήταν πατημένα κατά το frame αυτό.

void state_update(State state, KeyState keys) {
	// Προς υλοποίηση
	//Αν το παιχνίδι έχει τελειώσει και πατηθεί enter, τότε ξαναρχίζει από την αρχή.
	if ( !state->info.playing ){
		if(keys->enter){
			state->info.playing = true ;
			state_destroy(state);
			state_create();
		}
	}
	else {
		//Αν πατηθεί P το παιχνίδι μπαίνει σε pause και δεν ενημερώνεται πλέον.
		if(state->info.paused == false)  {
			//Οριζόντια κίνηση μπάλας
			if(keys->right){
				//μετακινείται 6 pixels προς τα δεξιά σε κάθε frame αν ειναι πατημενο το δεξι βελος
				//επισης στις μετακινησεις συμπεριλαμβανεται και το speed_factor
				state->info.ball->rect.x = state->info.ball->rect.x + 6 * state->speed_factor ;
			}
			else if (keys->left){
				//μετακινείται 1 pixel αν ειναι πατημενο το αριστερο βελος
				state->info.ball->rect.x = state->info.ball->rect.x + 1 * state->speed_factor  ;
			}
			else {
				//διαφορετικα μετακινείται 4 pixels
				state->info.ball->rect.x = state->info.ball->rect.x + 4 * state->speed_factor ;
			}
			//Κατακόρυφη κίνηση μπάλας ανάλογα με τον τύπο κίνησης στον οποία βρίσκεται (vert_mov)
			if( state->info.ball->vert_mov == JUMPING ){
				// Μετακινείται προς τα πάνω τόσα pixels όσα η κατακόρυφη ταχύτητά της 
				state->info.ball->rect.y -= state->info.ball->vert_speed * state->speed_factor ;
				//Η ταχύτητα μειώνετα στο 85% της προηγούμενης τιμής.
				state->info.ball->vert_speed = state->info.ball->vert_speed * 85 / 100 ;
				//Αν η ταχύτητα (που μειώνεται κατά το άλμα) φτάσει το 0.5 τότε 
				//μπαίνει σε κατάσταση πτώσης (FALLING)
				if(state->info.ball->vert_speed <= 0.5)
					state->info.ball->vert_mov = FALLING ;
			}
			else if( state->info.ball->vert_mov ==  FALLING ){
				//Μετακινείτα προς τα κάτω τόσα pixels όσα η κατακόρυφη ταχύτητά της
				state->info.ball->rect.y += state->info.ball->vert_speed * state->speed_factor;
				//Η ταχήτητα αυξάνεται κατά 10%, με μέγιστο το 7.
				state->info.ball->vert_speed *= 1.1 ;
				if(state->info.ball->vert_speed > 7)
					state->info.ball->vert_speed = 7 ;
				for( VectorNode vec_node =  vector_first(state->objects);
					vec_node != VECTOR_EOF ;
					vec_node = vector_next(state->objects, vec_node)) {
							Object object = vector_node_value(state->objects, vec_node);
							//Αν η μπάλα βρίσκεται σε πτώση (FALLING) και συγκρουστεί με πλατφόρμα
							// τότε μετακινείται ακριβώς πάνω στην πλατφόρμα και αλλάζει σε IDLE
							if( object->type == PLATFORM ) {
								if(state->info.ball->rect.y + state->info.ball->rect.height  < object->rect.y + object->rect.height
								   && CheckCollisionRecs(object->rect, state->info.ball->rect)){
									if(object->unstable)
										object->vert_mov = FALLING;
									state->info.ball->rect.y = object->rect.y - state->info.ball->rect.height ;
									state->info.ball->vert_mov = IDLE ;
									break;
								}
							}
				}
			}
			else if( state->info.ball->vert_mov ==  IDLE ) {
				//Αν είναι πατημένο το πάνω βέλος 
				//μπαίνει σε κατάσταση άλματος με αρχική ταχύτητα 17.
				if(keys->up){
					state->info.ball->vert_mov = JUMPING ;
					state->info.ball->vert_speed = 17 * state->speed_factor;
				}
				else{
					//Συμπεριφορά μπάλας σε κατακόρυφη ηρεμία (IDLE)
					// ανάλογα με το αν βρίσκεται πάνω σε πλατφόρμα
					int is_on_platform = 0;
					for( VectorNode vec_node =  vector_first(state->objects);
						vec_node != VECTOR_EOF ;
						vec_node = vector_next(state->objects, vec_node)) {
							Object object = vector_node_value(state->objects, vec_node);
							if( object->type == PLATFORM ) {		
								//Αν η συντεταγμένη x βρίσκεται στα όρια κάποιας πλατφόρμας
								if((state->info.ball->rect.x >= object->rect.x) && (state->info.ball->rect.x <= object->rect.x + object->rect.width)) {
									//τότε η συντεταγμένη y τροποποιείται ώστε η μπάλα να ακολουθεί σε ύψος την πλατφόρμα.
									state->info.ball->rect.y = object->rect.y - state->info.ball->rect.height ;
									is_on_platform = 1 ;
									break;
								}	
							}
					}
					//Αν η συντεταγμένη x δεν βρίσκεται στα όρια καμίας πλατφόρμας τότε η μπάλα
					//μπαίνει σε κατάσταση πτώσης (FALLING) με αρχική ταχύτητα 1.5.
					if(!is_on_platform){
						state->info.ball->vert_mov =  FALLING ; 
						state->info.ball->vert_speed = 1.5 * state->speed_factor;
					}
				}
			}
			Object last_platform = NULL ;
			for( VectorNode vec_node =  vector_first(state->objects);
				vec_node != VECTOR_EOF ;
				vec_node = vector_next(state->objects, vec_node)) {
					Object object = vector_node_value(state->objects, vec_node);
					if( object->type == PLATFORM ){
						last_platform = object ;
						//Κατακόρυφη κίνηση πλατφόρμας ανάλογα με τον τύπο κίνησης στον οποία βρίσκεται (vert_mov)
						if( object->vert_mov == MOVING_UP){
							//Μετακινείται προς τα πάνω τόσα pixels όσα η κατακόρυφη ταχύτητά της
							object->rect.y -= object->vert_mov * state->speed_factor ;
							//Αν περάσει το SCREEN_HEIGHT/4 αλλάζει σε MOVING_DOWN
							if( object->rect.y < (SCREEN_HEIGHT/4) ){
								object->vert_mov = MOVING_DOWN ;
							}
						}
						else if( object->vert_mov == MOVING_DOWN) {
							//Μετακινείται προς τα κάτω τόσα pixels όσα η κατακόρυφη ταχύτητά της.
							object->rect.y += object->vert_mov * state->speed_factor;
							//Αν περάσει τo 3*SCREEN_HEIGHT/4 αλλάζει σε MOVING_UP
							if( object->rect.y > (3 * SCREEN_HEIGHT / 4) ){
								object->vert_mov = MOVING_UP ;
							}
						}
						else if( object->vert_mov == FALLING) {
							//Μετακινείται προς τα κάτω 4 pixels
							object->rect.y += 4 * state->speed_factor;
							//Αν μια πλατφόρμα σε πτώση (FALLING) φτάσει στο κάτω μέρος
							//της οθόνης αφαιρείται από το vector των αντικειμένων.
							if(object->rect.y == SCREEN_HEIGHT){
								vector_insert_last(state->objects, object) ;
								vector_remove_last(state->objects) ;
							}
						}
					}
					else if(object->type == STAR){
						//Αν η μπάλα συγκρουστεί με αστέρι τότε το αστέρι αφαιρείται 
						//από το vector των αντικειμένων και προστίθενται 10 πόντοι στο σκορ.
						if(CheckCollisionRecs(object->rect, state->info.ball->rect )){
							state->info.score += 10;
							// = vector_node_value(state->objects,vector_last(state->objects)) ;
							vector_insert_last(state->objects, object) ;
							vector_remove_last(state->objects) ;
						}
					}

			}
			//Αν η μπάλα φτάσει στο κάτω μέρος της οθόνης τερματίζει το παιχνίδι.
			if(state->info.ball->rect.y >= SCREEN_HEIGHT){
				state->info.playing = false ;
			}
			//Όταν το η μπάλα φτάσει κοντά (απόσταση μίας οθόνης) από την τελευταία πλατφόρμα,
			if(last_platform->rect.x - (state->info.ball->rect.x + state->info.ball->rect.width) <= SCREEN_WIDTH ){
				//δημιουργούνται νέα αντικείμενα, ξεκινώντας από το x μετά από την πλατφόρμα αυτή
				add_objects(state, last_platform->rect.x + last_platform->rect.width) ;
				//Επίσης η ταχύτητα του παιχνιδιού γίνεται 10% μεγαλύτερη
				//Αυτό επιτυγχάνεται αυξάνοντας το speed_factor και φροντίζοντας όλες οι μετακινήσεις να λαμβάνουν υπόψη το speed_facto
				state->speed_factor = state->speed_factor * 1.1 ;
			}
		}
		if(keys->p){
			if(state->info.paused == false){
				state->info.paused = true;
			}
			else{
				state->info.paused = false ;
			}
		}
	}
}

// Καταστρέφει την κατάσταση state ελευθερώνοντας τη δεσμευμένη μνήμη.

void state_destroy(State state) {
	// Προς υλοποίηση
	free(state);
}