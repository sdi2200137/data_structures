# Κάνοντας compile το <foo>_test.c με μια υλοποίηση <foo>.c του
# συγκεκριμένου τύπου, παράγουμε ένα tets για την υλοποίηση αυτή.

# Υλοποιήσεις μέσω Heap: ADTPriorityQueue
# Το Heap χρησιμοποιεί Vector, οπότε χρειαζόμαστε και μια υλοποίηση του ADTVector.
#

UsingLinkedTree_ADTRecTree_ADTCompTree_ADTPriorityQueue_test_OBJS  = ADTPriorityQueue_test.o $(MODULES)/UsingDynamicArray/ADTVector.o $(MODULES)/UsingADTCompTree/ADTPriorityQueue.o $(MODULES)/UsingADTRecTree/ADTCompTree.o $(MODULES)/UsingLinkedTree/ADTRecTree.o $(MODULES)/ADTRecTree_utils.o

UsingADTMap__ADTRecTree_ADTCompTree_ADTPriorityQueue_test_OBJS = ADTPriorityQueue_test.o $(MODULES)/UsingDynamicArray/ADTVector.o $(MODULES)/UsingADTCompTree/ADTPriorityQueue.o $(MODULES)/ADTRecTree_utils.o $(MODULES)/UsingADTMap/ADTRecTree.o $(MODULES)/UsingADTRecTree/ADTCompTree.o $(MODULES)/UsingADTMap/ADTMap.o $(MODULES)/UsingADTMap/ADTSet.o

UsingLinkedTree_ADTRecTree_test_OBJS = ADTRecTree_test.o $(MODULES)/UsingLinkedTree/ADTRecTree.o 

UsingADTMap_ADTRecTree_test_OBJS = ADTRecTree_test.o $(MODULES)/UsingADTMap/ADTRecTree.o $(MODULES)/UsingADTMap/ADTMap.o  $(MODULES)/UsingADTMap/ADTSet.o

UsingLinkedTree_ADTRecTree_utils_test_OBJS = ADTRecTree_utils_test.o $(MODULES)/UsingLinkedTree/ADTRecTree.o $(MODULES)/ADTRecTree_utils.o

UsingADTMap_ADTRecTree_utils_test_OBJS = ADTRecTree_utils_test.o $(MODULES)/UsingADTMap/ADTRecTree.o $(MODULES)/ADTRecTree_utils.o $(MODULES)/UsingADTMap/ADTMap.o  $(MODULES)/UsingADTMap/ADTSet.o

UsingLinkedTree_ADTRecTree_ADTCompTree_test_OBJS = ADTCompTree_test.o $(MODULES)/UsingLinkedTree/ADTRecTree.o $(MODULES)/UsingADTRecTree/ADTCompTree.o $(MODULES)/ADTRecTree_utils.o

UsingADTMap_ADTRecTree_ADTCompTree_test_OBJS = ADTCompTree_test.o $(MODULES)/UsingADTMap/ADTRecTree.o $(MODULES)/UsingADTMap/ADTMap.o  $(MODULES)/UsingADTMap/ADTSet.o $(MODULES)/UsingADTRecTree/ADTCompTree.o $(MODULES)/ADTRecTree_utils.o

# Ο βασικός κορμός του Makefile
include ../common.mk