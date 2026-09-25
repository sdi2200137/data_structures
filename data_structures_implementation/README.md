![run-tests](../../workflows/run-tests/badge.svg)

## 2023 Project 2

Εκφώνηση: https://k08.chatzi.org/projects/project2/


### Προσωπικά στοιχεία

__Όνομα__: Παπαϊωάννου Πέτρος

__Α.Μ.__: sdi2200137

### Ασκήσεις που παραδίδονται

Συμπληρώστε εδώ τις ασκήσεις (και τυχόν bonus) που παραδίδετε. Αν κάποια άσκηση
βρίσκεται σε μη αναμενόμενα αρχεία αναφέρετε και τα αρχεία αυτά.

- Ασκήσεις 1 - 5
- ...

### Documentation

ADTRecTree_utils, rectree_replace_subtree:
Έχω σε σε σχόλια 4 γραμμές κωδικα έτσι ωστε να περάσει τα τεστ της άσκησης 5 αλλά αυτο δημιουργεί leaks στα τεστ των ασκήσεων 3, 4, 5
Αν αυτές οι γραμμές κωδικα δεν είναι σχολιασμένες τα τεστ των ασκήσεων 3, 4 λειτουργουν σωστά και δεν έχουν leaks


Άσκηση 2 :
Πολυπλοκότητα συναρτήσεων

rectree_create: η map_create εξαρτάται από την set_create, η οποία ειναι Ο(1), η map_create καλείται 4 φορές και η πολυπλοκότητα παραμένει Ο(1),
map_insert η οποία καλεί την set_find που είναι Ο(logn) και την set_insert που είναι και αυτή Ο(logn) 
η map_find καλει πάλι την set_find . Τελικά η rectree_create είναι Ο(logn)

rectree_size: καλεί την map_find που έχει πολυπλοκότητα Ο(logn) για το set με υλοποίηση AVL

rectree_value, rectree_left, rectree_right είναι το ίδιο με την rectree_size

rectree_destroy: καλεί την map_remove που έχει πολυπλοκότητα Ο((logn)^2) και την map_destroy
που έχει πολυπλοκότητα Ο(logn). Τελικα έχει πολυπλοκότητα Ο((logn)^2)

Άσκηση 3:

rectree_get_subtree: έχει πολυπλοκότητα Ο(logn) σε ένα AVL δέντρο επειδή είναι balanced

rectree_replace_subtree: καλεί την rectree_get_subtree logn άρα 
έχει πολυπλοκότητα Ο((logn)^2)

Άσκηση 4:

Οι βασικές λειτουργίες του CompTree εξαρτώνται από την υλοποίηση του RecTree

comptree_insert_last: καλεί την rectree_create οπότε εξαρτάται από το RecTree
και την rectree_replace_subtree που πάλι εξαρτάται από το RecTree
(σε balanced έχει πολυπλοκότητα Ο((logn)^2) και τελικα η comptree_insert_last Ο((logn)^2))

comptree_remove_last: χρησιμοποιεί rectree_replace_subtree που εξαρτάται από το RecTree

Άσκηση 5:
Έχω τροποποιήσει την naive_heapify ώστε να υλοποιεί τον περισσότερο αποδωτικό αλγόριθμο αλλά δεν ονόμασα την συνάρτηση κάπως διαφορετικά

Συμπληρώστε εδώ __όσο documentation χρειάζεται__ ώστε οι βαθμολογητές να
κατανοήσουν πλήρως τις λύσεις σας και να τις βαθμολογήσουν ανάλογα. Αυτό θα
πρέπει να γίνει ανεξάρτητα με το αν ο κώδικάς σας είναι καλά σχολιασμένος,
πράγμα που συνιστάται.
