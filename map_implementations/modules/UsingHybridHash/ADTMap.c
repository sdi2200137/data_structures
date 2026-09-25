/////////////////////////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Map μέσω υβριδικού Hash Table
//
/////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>

#include "ADTMap.h"
#include "ADTVector.h"

// Κάθε θέση i θεωρείται γεινοτική με όλες τις θέσεις μέχρι και την i + NEIGHBOURS
#define NEIGHBOURS 3

// Οι κόμβοι του map στην υλοποίηση με hash table, μπορούν να είναι σε 3 διαφορετικές καταστάσεις,
// ώστε αν διαγράψουμε κάποιον κόμβο, αυτός να μην είναι empty, ώστε να μην επηρεάζεται η αναζήτηση
// αλλά ούτε occupied, ώστε η εισαγωγή να μπορεί να το κάνει overwrite.
typedef enum {
	EMPTY, OCCUPIED
} State;

// Το μέγεθος του Hash Table ιδανικά θέλουμε να είναι πρώτος αριθμός σύμφωνα με την θεωρία.
// Η παρακάτω λίστα περιέχει πρώτους οι οποίοι έχουν αποδεδιγμένα καλή συμπεριφορά ως μεγέθη.
// Κάθε re-hash θα γίνεται βάσει αυτής της λίστας. Αν χρειάζονται παραπάνω απο 1610612741 στοχεία, τότε σε καθε rehash διπλασιάζουμε το μέγεθος.
int prime_sizes[] = {53, 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317, 196613, 393241,
	786433, 1572869, 3145739, 6291469, 12582917, 25165843, 50331653, 100663319, 201326611, 402653189, 805306457, 1610612741};

// Χρησιμοποιούμε open addressing, οπότε σύμφωνα με την θεωρία, πρέπει πάντα να διατηρούμε
// τον load factor του  hash table μικρότερο ή ίσο του 0.5, για να έχουμε αποδoτικές πράξεις
#define MAX_LOAD_FACTOR 0.5

// Δομή του κάθε κόμβου που έχει το hash table (με το οποίο υλοιποιούμε το map)
struct map_node{
	Pointer key;   // Το κλειδί που χρησιμοποιείται για να hash-αρουμε
	Pointer value; // Η τιμή που αντισοιχίζεται στο παραπάνω κλειδί
	State state;   // Μεταβλητή για να μαρκάρουμε την κατάσταση των κόμβων (βλέπε διαγραφή)
	bool is_on_array; // Μεταβλητή που παίρνει την τιμή true οταν το map_node είναι μέσα στο array και false αν είναι σε κάποιο vector 
}; 

// Δομή του Map (περιέχει όλες τις πληροφορίες που χρεαζόμαστε για το HashTable)
struct map {
	MapNode array;				// Ο πίνακας που θα χρησιμοποιήσουμε για το map (remember, φτιάχνουμε ένα hash table)
	Vector chains;              //Το vector για το seperate chaining
	int capacity;				// Πόσο χώρο έχουμε δεσμεύσει.
	int size;					// Πόσα στοιχεία έχουμε προσθέσει
	CompareFunc compare;		// Συνάρτηση για σύγκριση δεικτών, που πρέπει να δίνεται απο τον χρήστη
	HashFunc hash_function;		// Συνάρτηση για να παίρνουμε το hash code του κάθε αντικειμένου.
	DestroyFunc destroy_key;	// Συναρτήσεις που καλούνται όταν διαγράφουμε έναν κόμβο απο το map.
	DestroyFunc destroy_value;
};

Map map_create(CompareFunc compare, DestroyFunc destroy_key, DestroyFunc destroy_value) {
	Map map = malloc(sizeof(*map));
	map->capacity = prime_sizes[0];
	map->array = malloc(map->capacity * sizeof(struct map_node));
	map->chains = vector_create(map->capacity, NULL); //αρχικοποιούμε το vector με map_capacity NULL ατοιχεία 

	// Αρχικοποιούμε τους κόμβους που έχουμε σαν διαθέσιμους.
	for (int i = 0; i < map->capacity; i++)
		map->array[i].state = EMPTY;
	
	map->size = 0;
	map->compare = compare;
	map->destroy_key = destroy_key;
	map->destroy_value = destroy_value;

	return map;
}

// Επιστρέφει τον αριθμό των entries του map σε μία χρονική στιγμή.
int map_size(Map map) {
	if(map != NULL)
		return map->size;
	return 0;
}

// Συνάρτηση για την επέκταση του Hash Table σε περίπτωση που ο load factor μεγαλώσει πολύ.
static void rehash(Map map) {
	// Αποθήκευση των παλιών δεδομένων
	int old_capacity = map->capacity;
	MapNode old_array = map->array;
	Vector old_vector = map->chains;

	// Βρίσκουμε τη νέα χωρητικότητα, διασχίζοντας τη λίστα των πρώτων ώστε να βρούμε τον επόμενο. 
	int prime_no = sizeof(prime_sizes) / sizeof(int);	// το μέγεθος του πίνακα
	for (int i = 0; i < prime_no; i++) {					// LCOV_EXCL_LINE
		if (prime_sizes[i] > old_capacity) {
			map->capacity = prime_sizes[i]; 
			break;
		}
	}
	// Αν έχουμε εξαντλήσει όλους τους πρώτους, διπλασιάζουμε
	if (map->capacity == old_capacity)					// LCOV_EXCL_LINE
		map->capacity *= 2;								// LCOV_EXCL_LINE

	// Δημιουργούμε ένα μεγαλύτερο hash table και vector
	map->array = malloc(map->capacity * sizeof(struct map_node));
	for (int i = 0; i < map->capacity; i++)
		map->array[i].state = EMPTY;
	
	map->chains = vector_create(map->capacity, NULL);

	// Τοποθετούμε ΜΟΝΟ τα entries που όντως περιέχουν ένα στοιχείο
	map->size = 0;
	for (int i = 0; i < old_capacity; i++) {
		if (old_array[i].state == OCCUPIED)
			map_insert(map, old_array[i].key, old_array[i].value);
	
		Vector vec = vector_get_at(old_vector, i);
		if(vec != NULL) {
			for(int j = 0; j < vector_size(vec); j++){
				MapNode node = vector_get_at(vec, j);
				if (node->state == OCCUPIED)
					map_insert(map, node->key, node->value);
			}
		}
	}

	//Αποδεσμεύουμε τον παλιό πίνακα και το παλιό vector ώστε να μήν έχουμε leaks
	for(int i = 0; i < old_capacity; i++) {
		Vector vec = vector_get_at(old_vector, i);
		if(vec != NULL) {
			vector_destroy(vec);
		}
	}

	
	free(old_array);
	vector_destroy(old_vector);
}

// Εισαγωγή στο hash table του ζευγαριού (key, item). Αν το key υπάρχει,
// ανανέωση του με ένα νέο value, και η συνάρτηση επιστρέφει true.

void map_insert(Map map, Pointer key, Pointer value) {
	// Σκανάρουμε το Hash Table μέχρι να βρούμε διαθέσιμη θέση για να τοποθετήσουμε το ζευγάρι,
	// ή μέχρι να βρούμε το κλειδί ώστε να το αντικαταστήσουμε.
	bool already_in_map = false;
	MapNode node = NULL;
	uint pos;
	int count = 0;
	for (pos = map->hash_function(key) % map->capacity;		// ξεκινώντας από τη θέση που κάνει hash το key
		 count <= NEIGHBOURS;							// σταματάμε οταν δούμε την pos (αρχικό) + NEIGHBOURS θέση
		 pos = (pos + 1) % map->capacity) {	

		//βρίσκουμε EMPTY, την κρατάμε και συνεχίζουμε για την περίπτωση να έχει μπει σε επόμενη θέση το στοιχείο
		if(map->array[pos].state == EMPTY) {
			if (node == NULL) {
				node = &map->array[pos];
				node->is_on_array = true;
			}
		}

		else if (map->compare(map->array[pos].key, key) == 0) {
			already_in_map = true;
			node = &map->array[pos];						// βρήκαμε το key, το ζευγάρι θα μπει αναγκαστικά εδώ (ακόμα και αν είχαμε προηγουμένως βρει EMPTY θέση)
			break;											// και δε χρειάζεται να συνεχίζουμε την αναζήτηση.
		}

		count++;
	}

	//αν δεν υπάρχει ήδη στο map μπορεί να είναι στο vector
	if(!already_in_map){
				Vector vec = vector_get_at(map->chains, map->hash_function(key) % map->capacity); //σε κάθε θέση του vector chains έχει ενα άλλο vector για τα στοιχεία της αντίστοιχης θέσης του πίνακα
				if(vec != NULL) {
					for(int i = 0; i < vector_size(vec); i++){
						MapNode mapnode = vector_get_at(vec, i);
						if (map->compare(mapnode->key, key) == 0) { 
							already_in_map = true;
							node = mapnode;						// βρήκαμε το key, το ζευγάρι θα μπει αναγκαστικά εδώ (ακόμα και αν είχαμε προηγουμένως βρει EMPTY θέση στον πινακα)
							node->is_on_array = false;
							break;								// και δε χρειάζεται να συνεχίζουμε την αναζήτηση.
						}
					}
				}
				if (node == NULL){  //αν δεν είναι ήδη στο map ή στο vector και δεν βρέθηκε ελεύθερη θεση στο map
									//τοτε τοποθετούμε το στοιχείο στο vector
					node = malloc(sizeof(struct map_node));
					node->is_on_array = false;
					if(vec == NULL) // αν δεν υπάρχει vector στη συγκεκτιμένη θέση το δημιουργούμε 
						vector_set_at(map->chains, (map->hash_function(key) % map->capacity), vector_create(0, NULL)) ;
					Vector vec2 = vector_get_at(map->chains, map->hash_function(key) % map->capacity);
					vector_insert_last(vec2, node);
				}
			}
		

		

	// Σε αυτό το σημείο, το node είναι ο κόμβος στον οποίο θα γίνει εισαγωγή.
	if (already_in_map) {
		// Αν αντικαθιστούμε παλιά key/value, τa κάνουμε destropy
		if (node->key != key && map->destroy_key != NULL)
			map->destroy_key(node->key);

		if (node->value != value && map->destroy_value != NULL)
			map->destroy_value(node->value);

	} else {
		// Νέο στοιχείο, αυξάνουμε τα συνολικά στοιχεία του map
		map->size++;
	}

	// Προσθήκη τιμών στον κόμβο
	node->state = OCCUPIED;
	node->key = key;
	node->value = value;

	// Αν με την νέα εισαγωγή ξεπερνάμε το μέγιστο load factor, πρέπει να κάνουμε rehash.
	// Στο load factor μετράμε και τα DELETED, γιατί και αυτά επηρρεάζουν τις αναζητήσεις.
	float load_factor = (float)(map->size) / map->capacity;
	if (load_factor > MAX_LOAD_FACTOR)
		rehash(map);

}

// Διαργραφή απο το Hash Table του κλειδιού με τιμή key
bool map_remove(Map map, Pointer key) {
	MapNode node = map_find_node(map, key);
	if (node == MAP_EOF)
		return false;

	//αν είναι στο array το κανουμε EMPTY ενώ αν είναι στο vector το αφαιρούμε από αυτό
	if(node->is_on_array)
		node->state = EMPTY;
	else{
		int i = 0;
		Vector vec = vector_get_at(map->chains, map->hash_function(node->key ) % map->capacity);
		//βρίσκουμε το στοιχείο στο vector
		for(VectorNode vecnode = vector_first(vec); vecnode != VECTOR_EOF; vecnode = vector_next(vec, vecnode)){
			MapNode node = vector_node_value(vec, vecnode);
			if(map->compare(node->key, key) == 0)
				break;
			i++ ;
		}
		//αλλαζουμε τη θέση του με το τελευταίο και το αφαιρούμε
		MapNode last = vector_get_at(vec, vector_size(vec)-1);
		vector_set_at(vec, i, last);
		vector_set_at(vec, vector_size(vec) - 1, node);
		vector_remove_last(vec);
	}
	// destroy
	if (map->destroy_key != NULL)
		map->destroy_key(node->key);
	if (map->destroy_value != NULL)
		map->destroy_value(node->value);

	map->size--;

	return true;
}

// Αναζήτηση στο map, με σκοπό να επιστραφεί το value του κλειδιού που περνάμε σαν όρισμα.

Pointer map_find(Map map, Pointer key) {
	MapNode node = map_find_node(map, key);
	if (node != MAP_EOF)
		return node->value;
	else
		return NULL;
}


DestroyFunc map_set_destroy_key(Map map, DestroyFunc destroy_key) {
	if(map != NULL){
		DestroyFunc previous = map->destroy_key;
		map->destroy_key = destroy_key;
		return previous;
	}
	else 
		return NULL;
}

DestroyFunc map_set_destroy_value(Map map, DestroyFunc destroy_value) {
	if(map != NULL){
		DestroyFunc previous = map->destroy_value;
		map->destroy_value = destroy_value;
		return previous;
	}
	else 
		return NULL;
}

// Απελευθέρωση μνήμης που δεσμεύει το map
void map_destroy(Map map) {
	for (int i = 0; i < map->capacity; i++) {
		if (map->array[i].state == OCCUPIED) {
			if (map->destroy_key != NULL)
				map->destroy_key(map->array[i].key);
			if (map->destroy_value != NULL)
				map->destroy_value(map->array[i].value);
		}
	} 

	free(map->array);
	for(int i = 0; i < map->capacity; i++) {
		Vector vec = vector_get_at(map->chains, i);
		if(vec != NULL) {
			for(VectorNode vecnode = vector_first(vec); vecnode != VECTOR_EOF; vecnode = vector_next(vec, vecnode)){
				MapNode node = vector_node_value(vec, vecnode);
				if (map->destroy_key != NULL)
					map->destroy_key(node->key);
				if (map->destroy_value != NULL)
					map->destroy_value(node->value);
			}
			vector_destroy(vec);
		}
	}
	vector_destroy(map->chains);
	free(map);
}

/////////////////////// Διάσχιση του map μέσω κόμβων ///////////////////////////


MapNode map_first(Map map) {
	//Ξεκινάμε την επανάληψή μας απο το 1ο στοιχείο του map και αν δεν υπάρχει παμε στο αντίστοιχο vector για κάθε θέση, μέχρι να βρούμε κάτι όντως τοποθετημένο
	for (int i = 0; i < map->capacity; i++){
		if (map->array[i].state == OCCUPIED)
			return &map->array[i];

		Vector vec = vector_get_at(map->chains, i);
		if(vec != NULL){
			for(int j = 0; j < vector_size(vec); j++){
				MapNode vecnode = vector_get_at(vec, j);
				if (vecnode->state == OCCUPIED)
					return vecnode;
			}
		}
	}
	return MAP_EOF;
}

MapNode map_next(Map map, MapNode node) {
	if(node->is_on_array){
		int i; //χρησιμοποιείται για να βρεθεί η θεση που έχει τοποθετηθεί το στοιχείο
		int count = 0;
		//το στοιχείο είναι στο array
		//αρχικά βρισκουμε σε ποια θέση εχει τοποθετηθεί
		for (i = node - map->array; i < map->capacity; i++){
			if(map->compare(map->array[i].key, node->key) == 0)
				break;
		
			if(count == (NEIGHBOURS - 1))
				break;
			count ++;

		}
		//αρχίζουμε από την επομενη(για τον πίνακα)
		int next_in_array = i + 1;


		for(int j = node - map->array; j < map->capacity; j++){
			//παμε πρώτα στο vector για να περάσουμε από ολα τα πιθανά επόμενα στοιχεία
			Vector vec = vector_get_at(map->chains, j);
			if(vec != NULL){
				for(VectorNode vecnode = vector_first(vec); vecnode != VECTOR_EOF; vecnode = vector_next(vec, vecnode)){
					MapNode node = vector_node_value(vec, vecnode);
					return node;
				}
			}
			//αν δεν υπάρχει στο vector το επόμενο στοιχείο είναι στην επόμενη θέση του array
			if(next_in_array < map->capacity){
				if (map->array[next_in_array].state == OCCUPIED)
					return &map->array[next_in_array];
				next_in_array ++;
			}
		}
	}
	else{
		//το στοιχείο βρισκεται στο vector
		uint pos = map->hash_function(node->key) % map->capacity;
		//αρχικά βρίσκουμε το στοιχείο
			Vector vec = vector_get_at(map->chains, pos);
			if(vec != NULL){
				VectorNode vecnode;
				for(vecnode = vector_first(vec);
					vecnode != VECTOR_EOF;
					vecnode = vector_next(vec, vecnode)){
						MapNode mapnode = vector_node_value(vec, vecnode);
						if(map->compare(mapnode->key, node->key) == 0)
							break;
				}
				//το επόμενο στοιχείο στο vector είναι αυτό που ψάχνουμε(αν υπάρχει)
				vecnode = vector_next(vec, vecnode);
				if(vecnode != VECTOR_EOF){
					return vector_node_value(vec, vecnode);
				}
			}
		//δεν υπήρχε επόμενο στο vector, άρα παμε στην επόμενη θέση του array και το αντίστοιχο vector
		for (int i = pos + 1; i < map->capacity; i++){
			if (map->array[i].state == OCCUPIED)
				return &map->array[i];
				
			Vector vec = vector_get_at(map->chains, i);
			if(vec != NULL){
				for(VectorNode vecnode = vector_first(vec);
					vecnode != VECTOR_EOF;
					vecnode = vector_next(vec, vecnode)){
						return vector_node_value(vec, vecnode);
				}
			}
		}
	}
	return MAP_EOF;
}

Pointer map_node_key(Map map, MapNode node) {
	if(node != NULL)
		return node->key;
	return NULL;
}

Pointer map_node_value(Map map, MapNode node) {
	if(node != NULL)
		return node->value;
	return NULL;
}

MapNode map_find_node(Map map, Pointer key) {
	// Διασχίζουμε τον πίνακα, ξεκινώντας από τη θέση που κάνει hash το key, και για όσο δε βρίσκουμε EMPTY
	int count = 0;
	for (uint pos = map->hash_function(key) % map->capacity;		// ξεκινώντας από τη θέση που κάνει hash το key
		count <= NEIGHBOURS;							//
		pos = (pos + 1) % map->capacity) {						// linear probing, γυρνώντας στην αρχή όταν φτάσουμε στη τέλος του πίνακα

		// Μόνο σε OCCUPIED θέσεις, ελέγχουμε αν το key είναι εδώ
		if ((map->array[pos].state == OCCUPIED) && (map->compare(map->array[pos].key, key) == 0))
			return &map->array[pos];

		// Αν διασχίσουμε NEIGHBOURS σταματάμε
		
		count++;
	}

	//το στοιχείο μπορεί να βρίσκεται στο vector
	Vector vec = vector_get_at(map->chains, map->hash_function(key) % map->capacity);
	if(vec != NULL){
		for(int i = 0; i < vector_size(vec); i++){
			MapNode vecnode = vector_get_at(vec, i);
			if(vecnode == NULL)
				continue;
				
			if (map->compare(vecnode->key, key) == 0) {
				return vecnode;
			}
		}
	}
	
	return MAP_EOF;
}

// Αρχικοποίηση της συνάρτησης κατακερματισμού του συγκεκριμένου map.
void map_set_hash_function(Map map, HashFunc func) {
	if(map != NULL)
		map->hash_function = func;
}

uint hash_string(Pointer value) {
	// djb2 hash function, απλή, γρήγορη, και σε γενικές γραμμές αποδοτική
    uint hash = 5381;
    for (char* s = value; *s != '\0'; s++)
		hash = (hash << 5) + hash + *s;			// hash = (hash * 33) + *s. Το foo << 5 είναι γρηγορότερη εκδοχή του foo * 32.
    return hash;
}

uint hash_int(Pointer value) {
	return *(int*)value;
}

uint hash_pointer(Pointer value) {
	return (size_t)value;				// cast σε sizt_t, που έχει το ίδιο μήκος με έναν pointer
}