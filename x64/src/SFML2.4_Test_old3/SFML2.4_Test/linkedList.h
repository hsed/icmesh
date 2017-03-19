#pragma once
#include <string>
#include <sstream>

using namespace std;

//LinkedList Template Class for ADT (Abstract Data Type) Access Routines
template<class dataType> class LinkedList {

public:
	LinkedList() {
		//default constructor
		mainList = NULL; //list points to null by default
		isSorted = false;
	}
	~LinkedList()
	{
		delList(mainList); //free up memory;
		//mainList == NULL; //optional, not really needed as all variables will be auto destroyed.
	}

	static void clearSS(stringstream& _ss) {
		_ss.clear(); //clear any previous writes
		_ss.str(std::string());
	}
	void readStream(stringstream& _ss) {
		//do stuff;
		dataType el;
		while (_ss >> el) { addItem(el); }
		clearSS(_ss);
	}
	stringstream writeStream() {
		stringstream ss;
		listPtr l = mainList; //make a copy
		while (l != NULL) { ss << l->val << endl; l = l->next; }
		return ss;
	}
	string printList(bool isLIFO = true)
	{
		clearSS(ss);
		//Still need to pass by val to not affect mainList
		if (isLIFO) { printLIFO(mainList); }
		else { printFIFO(mainList); }
		return ss.str();
	}
	void addItem(dataType el)
	{
		listPtr tmpNode = new node;
		tmpNode->val = el;
		tmpNode->next = mainList;
		mainList = tmpNode;
	}
	void insertItem(dataType el) {
		//first check whether we can just add elem to start;
		//if neither conditions are true then call overloaded self private function (do sorted insert);
		listPtr prevPtr = NULL;
		if (mainList == NULL || !isSorted || mainList->val >= el) { addItem(el); }
		else { insertItem(el, mainList, prevPtr); } 
	}
	int delItem(dataType el) {
		listPtr prevPtr = NULL;
		if (isListEmpty()) { return -1; }
		else if (el < mainList->val && isSorted) { return 0; } //The list is sorted and (elem < first elem in list) so not found.
		else if (el == mainList->val) {
			//el is the first element in list, doesn't matter if sorted
			delAt(mainList, prevPtr);
			return 1; //item found
		}
		else {
			//if neither conditions are true then do sequenctial search and delete (overloaded self private function);
			if (delItem(el, mainList, prevPtr)) {
				return 1; //item found
			}
			else {
				return 0; //item not found
			}
		}
	}
	int countEven() {
		return countEven(mainList); //call overloaded self-fn to protect mainList ptr
	}
	bool sortList()
	{
		//Use bubble-sort algorithm to sort elements, only pointers to be swapped. Pass-by-val to point to sorted list.
		//mainList is directly called in this function
		listPtr listPrev, listJ, tmpNode; //Pointers to the list
		bool swap = true;
		int size = listSize(mainList);
		//listI is not needed we just need to iterate the outer loop n times.
		for (int i = 0; i < size && swap; i++) {
			swap = false;
			for (listPrev = NULL, listJ = mainList; listJ->next != NULL; listPrev = listJ, listJ = listJ->next) {
				//will be same as j=0 ---> j<(size-1)
				if (listJ->next->val < listJ->val) {
					tmpNode = listJ;
					listJ = listJ->next;
					tmpNode->next = listJ->next;
					listJ->next = tmpNode;
					if (listPrev == NULL) { mainList = listJ; } //Special case change, main list if starting element was changed.
					else { listPrev->next = listJ; } //Ensure previous elements are rerouted to new listJ.
					swap = true;
				}
			}
		}
		isSorted = true;
		return true;
	}
	bool isListEmpty() {
		return (mainList == NULL);
	}

protected:
	struct node {
		dataType val;
		node* next;
	};
	typedef node* listPtr;
	listPtr mainList;
private:
	stringstream ss;
	bool isSorted;
	

	// prints list to ss in LIFO order
	void printLIFO(listPtr l) {
		if (l != NULL) {
			ss << l->val << endl;
			printLIFO(l->next);
		}
	}

	// prints list to ss in FIFO order
	void printFIFO(listPtr l) {
		if (l != NULL) {
			printFIFO(l->next);
			ss << l->val << endl;
		}
	}

	//Returns the number of elements present in the list, useful for sorting. LastElem(Null) is not counted. Recursive.
	int listSize(listPtr l) {
		if (l == NULL) { return 0; }
		else { return 1 + listSize(l->next); }
	}

	//To be used ONLY when list is sorted, Passes by copy as we are sure we do not need to add new elem in front
	void insertItem(dataType el, listPtr l, listPtr& prevPtr)
	{
		if (l != NULL && l->val < el) {
			insertItem(el, l->next, l);
		}
		else {
			listPtr tmpNode = new node;
			tmpNode->val = el;
			tmpNode->next = l;
			prevPtr->next = tmpNode;
		}
	}


	//point 'prevPtr' to 'l->next', delete element pointed by 'l', must be by-ref due to deletion, then set 'l' as prevPtr;
	void delAt(listPtr& l, listPtr& prevPtr)
	{
		if (prevPtr != NULL) {
			prevPtr->next = l->next;
			delete l;
			l = prevPtr;
		}
		else {
			//l is the first element, special case.
			prevPtr = l;
			l = l->next;
			delete prevPtr;
		}

	}

	//recursive function
	bool delItem(dataType el, listPtr l, listPtr& prevPtr) {
		if (l != NULL && prevPtr != NULL && el == l->val) { delAt(l, prevPtr); return true; } //found it, so delete it; prevPtr chked as head is pre-tested;	
		else if (l != NULL && el > l->val && isSorted) { return delItem(el, l->next, l); } //Keep going
		else if (l != NULL && !isSorted) { return delItem(el, l->next, l); } //If not sorted we must check all values
		else { return false; } //not found, default case
	}

	//Returns the number of even elements found in the list
	int countEven(listPtr l) {
		//ALT: !(l->val & 1) --> This will return false (=0) every time odd is found and true(=1) everytime its even
		if (l == NULL) { return 0; }
		else { return !(l->val % 2) + countEven(l->next); }
	}

	//Pass 'l' by copy so that original l still points to original location (now empty);
	void delList(listPtr l)
	{
		if (l != NULL) {
			delList(l->next); //Go all the way to the end first
			delete l; //Delete the listNode before every return to outer function
		}
	}
};

