#include <stdlib.h>

#include "set_utils.h"
#include "ADTSet.h"
#include "ADTList.h"
#include "state.h"


// Οι ολοκληρωμένες πληροφορίες της κατάστασης του παιχνιδιού.
// Ο τύπος State είναι pointer σε αυτό το struct, αλλά το ίδιο το struct
// δεν είναι ορατό στον χρήστη.

struct state {
	Set objects;			// περιέχει στοιχεία Object (Πλατφόρμες, Αστέρια)
	struct state_info info;	// Γενικές πληροφορίες για την κατάσταση του παιχνιδιού
	float speed_factor;		// Πολλαπλασιαστής ταχύτητς (1 = κανονική ταχύτητα, 2 = διπλάσια, κλπ)
	Object ball_platform    //πλατφορμα στην οποια ειναι πανω η μπαλα - πλατφορμα στην οποια αναμενεται να προσγειωθει
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

// Συγκρίνει δύο objects με βαση πιο είναι πιο δεξιά  

int compare_objects(Pointer a, Pointer b){
    Object first = a ;
    Object second = b ;
    if(first->rect.x < second->rect.x) {
        return -1 ;
    }
    else if(first->rect.x == second->rect.x){
        return 0 ;
    }
    else {
        return 1 ;
    }
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
		set_insert(state->objects, platform);

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
			set_insert(state->objects, star);
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
	state->objects = set_create(compare_objects , NULL);
	add_objects(state, 0);

	// Δημιουργούμε την μπάλα τοποθετώντας τη πάνω στην πρώτη πλατφόρμα
	Object first_platform = set_node_value(state->objects, set_first(state->objects));
	state->info.ball = create_object(
		BALL,
		first_platform->rect.x,			// x στην αρχή της πλατφόρμας
		first_platform->rect.y - 40,	// y πάνω στην πλατφόρμα
		45, 45,							// πλάτος, ύψος
		IDLE,							// χωρίς αρχική κατακόρυφη κίνηση
		0,								// αρχική ταχύτητα 0
		false							// "unstable" πάντα false για την μπάλα
	);

	//η μπαλα αρχιζει πανω απο την πρωτη πλατφοτμα
	state->ball_platform = first_platform;

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
    SetNode node = NULL ;
	//Η συνάρτηση state_objects πρέπει γρήγορα να επιστρέφει τα αντικείμενα που βρίσκονται
	//ανάμεσα στα x_from και x_to, χωρίς να εξετάζει όλα τα αντικείμενα της πίστας
	//Συγκεκριμενα εξεταζει μονο τα αντικειμενα στο διαστημα αυτο
    Object object = set_find_eq_or_greater(state->objects, &x_from) ;
    if(object != NULL){
        while (object->rect.x < x_to){
            list_insert_next(list, list_node, object) ;
            node = set_find_node(state->objects, &object->rect.x) ;
            node = set_next(state->objects, node) ;
            if(node != SET_EOF)
                object = set_node_value(state->objects, node) ;
        }
    }
    return list ;
}

// Ενημερώνει την κατάσταση state του παιχνιδιού μετά την πάροδο 1 frame.
// Το keys περιέχει τα πλήκτρα τα οποία ήταν πατημένα κατά το frame αυτό.

void state_update(State state, KeyState keys) {
	// Προς υλοποίηση
	if(keys->right){
		state->info.ball->rect.x = state->info.ball->rect.x + 6 * state->speed_factor ;
	}
	else if (keys->left){
		state->info.ball->rect.x = state->info.ball->rect.x + 1 * state->speed_factor  ;
	}
	else {
		state->info.ball->rect.x = state->info.ball->rect.x + 4 * state->speed_factor ;
	}
	if( state->info.ball->vert_mov == JUMPING ){
		state->info.ball->rect.y -= state->info.ball->vert_speed * state->speed_factor ;
		state->info.ball->vert_speed = state->info.ball->vert_speed * 85 / 100 ;
		if(state->info.ball->vert_speed <= 0.5)
			state->info.ball->vert_mov = FALLING ;
	}
	else if( state->info.ball->vert_mov ==  FALLING ){
		state->info.ball->rect.y += state->info.ball->vert_speed * state->speed_factor;
		state->info.ball->vert_speed *= 1.1 ;
		if(state->info.ball->vert_speed > 7)
			 state->info.ball->vert_speed = 7 ;
			 //Στην state_update, ο έλεγχος των συγκρούσεων πρέπει να είναι αποδοτικός χωρίς να εξετάζει όλα τα αντικείμενα της πίστας
			 //πιθανο χ της πλατφορμας που θα μπορουσε να βρισκεται απο πανω η μπαλα, αρχιζοντας απο το χ της μπαλας μειων το μεγιστο μηκος μιας πλατφορμας
			 float platform_x = state->info.ball->rect.x - 250 ; 
			 Object object = set_find_eq_or_greater(state->objects, &platform_x) ;
			 if(object != NULL){
				//ελεγχος μεχρι το χ του object να μην ξεπερνα το μηκος μιας πλατφορμας μετα την μπαλα 
				while (object->rect.x <= state->info.ball->rect.x + state->info.ball->rect.width + 250){
					if(object->type == PLATFORM) {
						if(state->info.ball->rect.y < object->rect.y && CheckCollisionRecs(object->rect, state->info.ball->rect)) {
		 					state->info.ball->vert_mov = IDLE ;
							state->ball_platform = object ;
							break;
						}
		 			}
            		SetNode node = set_find_node(state->objects, &object->rect.x) ;
            		node = set_next(state->objects, node) ;
            		if(node != SET_EOF)
                		object = set_node_value(state->objects, node) ;
				}
			 }
	}
	else if( state->info.ball->vert_mov ==  IDLE ) {
		if(keys->up){
			state->info.ball->vert_mov = JUMPING ;
			state->info.ball->vert_speed = 17 * state->speed_factor;
		}
		else{
			int is_on_platform = 0;
			float platform_x = state->info.ball->rect.x - 250 ; 
			Object object = set_find_eq_or_greater(state->objects, &platform_x) ;
			if(object != NULL){
				//ελεγχος μεχρι το χ του object να μην ξεπερνα το μηκος μιας πλατφορμας μετα την μπαλα 
				while (object->rect.x <= state->info.ball->rect.x + state->info.ball->rect.width + 250){
					if(object->type == PLATFORM) {
						if(state->info.ball->rect.x >= object->rect.x && state->info.ball->rect.x <= object->rect.x + object->rect.width) {
							state->info.ball->rect.y = state->info.ball->rect.height + object->rect.y ;
							is_on_platform = 1 ;
							state->ball_platform = object ;
							break;
						}	
		 			}
            		SetNode node = set_find_node(state->objects, &object->rect.x) ;
            		node = set_next(state->objects, node) ;
            		if(node != SET_EOF)
                		object = set_node_value(state->objects, node) ;
				}
			 }
			if(!is_on_platform){
				state->info.ball->vert_mov =  FALLING ; 
				state->info.ball->vert_speed = 1.5 ;
			}
		}
	}

	Object last_platform = NULL ;
	Object temp = set_find_eq_or_greater(state->objects, &state->info.ball->rect.x) ;
	while(temp->type == PLATFORM){
		last_platform = temp;
    	SetNode node = set_find_node(state->objects, &temp->rect.x) ;
		node = set_next(state->objects, node) ;
    	if(node != SET_EOF)
       		temp = set_node_value(state->objects, node) ;
	}
			//ενημερωση των αντικειμενων μεχρι μια οθονη πισω και μια οθονη μπροστα απο την μπαλα
			float object_x = state->info.ball->rect.x - SCREEN_WIDTH ; 
			Object object = set_find_eq_or_greater(state->objects, &object_x) ;
			while (object->rect.x <= state->info.ball->rect.x + state->info.ball->rect.width + SCREEN_WIDTH){
				//τιμη που θα αφαιρεθει απο το set
				Pointer tbr = NULL ;
				if( object->type == PLATFORM ){
					if( object->vert_mov == MOVING_UP){
						object->rect.y -= object->vert_mov * state->speed_factor ;
						if( object->rect.y > (SCREEN_HEIGHT/4) )
							object->vert_mov = MOVING_DOWN ;
					}
					else if( object->vert_mov == MOVING_DOWN) {
						object->rect.y += object->vert_mov * state->speed_factor;
						if( object->rect.y > (3 * SCREEN_HEIGHT /4) )
							object->vert_mov = MOVING_UP ;
					}
					else if( object->vert_mov == FALLING) {
						object->rect.y -= 4 * state->speed_factor;
						if(object->rect.y == SCREEN_HEIGHT){
							tbr = &object->rect.x ;	
						}
					}
				}
				else if(object->type == STAR){
					if(CheckCollisionRecs(object->rect, state->info.ball->rect)){
						state->info.score += 10;
						tbr = &object->rect.x ;
					}
				}
           		SetNode node = set_find_node(state->objects, &object->rect.x) ;
            	node = set_next(state->objects, node) ;
            	if(node != SET_EOF)
            		object = set_node_value(state->objects, node) ;
				set_remove(state->objects, tbr) ;
			}
	if ( !state->info.playing ){
		if(keys->enter)
			state->info.playing = true ;
	}
	else {
		if(keys->p)
			state->info.paused = true;
		if ( state->info.paused ){
			if(keys->n){}
		}


	}
	if(state->info.ball->rect.y == SCREEN_HEIGHT)
		state->info.playing = false ;
	if(state->info.ball->rect.x + state->info.ball->rect.width >= last_platform->rect.x - SCREEN_WIDTH ){
		add_objects(state, last_platform->rect.x + last_platform->rect.width) ;
		state->speed_factor = state->speed_factor * 1.1 ;
	}
	
}
