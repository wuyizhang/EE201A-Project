#include <iostream>
#include "oaDesignDB.h"
#include <math.h>
#include <fstream>

using namespace oa;
using namespace std;

static oaNativeNS ns;

#ifndef NULL
#define NULL 0
#endif

// Debug function
void error(char* s, char* s2=""){
	cerr<<s<<' '<<s2<<'\n';
	exit(1);
};
		
class cblock {
	public:
	interval *blkint; // array of block intervals
	oaPoint blkcenter; // center of block
	oaUInt4 boundary[4]; // left, bottom, right, top value of block boundary
	oaPoint *intersection; // array for intersection point
	int inter_count; // current number of intersection points within the block
	cblock() {inter_count=0;} // constructor
}

struct interval {
	oaPoint inode[2]; // interval node for the intervals of a block
}

// Construct floorplan:
class floorplan {
	public:
	LL *ll_ver; // link list of vertical intersection lines
	LL *ll_hor; // link list of horizontal intersection lines
	inter_lines *v_lines; // array of of vertical intersection lines
	inter_lines *h_lines; // array of of horizontal intersection lines
	cblock *cells; // array of cells in design
	void floor_init(oaBlock *fblock); // initialize block boundary decomposition
}

struct inter_lines { // intersection lines
	oaPoint lnode[2]; // two endings of intersection lines
	oaUInt4 l_pos; // x (vertical) or y (horizontal) value of the intersection lines
} 

struct node {
    int number;
	int m_flag; // if this is a node from merging
    node *next, *prev;
    node(int num) {number=num; merge_flag=0;}; // constructor
    node() {number=-1; merge_flag=0;}; // constructor
};
		
class LL {
	protected:
	node headdummy, taildummy;  // dummy nodes for the ends of the link list (LL)
	node* head;                // ptr to head of LL
	node* tail;                // ptr to tail of LL
         
	public:
	node* current;             // ptr to current point of LL
	LL(void);         // constructor
	void addtail(node* nnode); // add nnode to tail of LL
	void reset(void);          // reset current to actual head
	void operator++();      // increase current to next node
	void removenode(node* nnode); // removes nnode from the LL
	unsigned int length; // length of linked list
}

LL::LL(void) {
	head=&headdummy;
	tail=&taildummy;
	head->next=tail;
	tail->prev=head;
	head->prev=NULL;
    tail->next=NULL;
    current = head;
	length = 0;
}

void LL::reset(void) {
	if(length!=0){
		current=head->next;
    }
	else
		current=NULL;
}

void LL::operator++() {
	if(current->next->number!=-1)
		current=current->next;
	else
		current=NULL;
}

void LL::addtail(node* nnode) {
	length++;
	nnode->next=tail;
	nnode->prev=tail->prev;
	tail->prev=nnode;
	nnode->prev->next=nnode;
}

void LL::removenode(node* nnode) {
	if(!search(nnode)){  
	// check if the node is in list
		cout << "node" << nnode->number << " is not in list!!" << endl;
		exit(1);
	}
	if(nnode!=NULL){
		nnode->prev->next=nnode->next;
		nnode->next->prev=nnode->prev;
		length--;
	}
}

/*
1. Iterate the blocks, building array and linked list for inter_lines by reading edge of each block. 
For horizontal edges, compare block's top and bottom y location and edges' y location to determine if they are intersected;
every edge line has a left intersection point and a right intersection point
(edge either intersects with another block or boundary of design), determine them by x location. record these two positions.
For vertical edges, compare block's right and left x location and edges' x location to determine if they are intersected,
every edge line has a top and a bottom intersection point
(edge either intersects with another block or boundary of design), determine them by y location. record these two positions.

2. Merge two adjacent lines (two boundary lines of a channel) into one line.
Iterate the linked lists, compare every two lines, if endings of two line segment are same in y (horizontal)
or same in x (vertical), and there is no block between them (iterate the block again), then merge them.
Merge: Construct a new merge line (added into the array and linked list) and delete the two old lines in linked list.
In the meantime, assign endings to the block by iterating blocks.
Why use linked list: This is to avoid checking the intersection lines that have been merged. 
It does not have to be used apparently.

By now, we can start building intervals of each block:
Iterate the blocks, build interval array from the upperleft point of the block and go through intersection array for next node.
This is go through intersection array every time, it is acceptable considering there won't be many intersection points for a block.
*/

// Programm to be added to main.cpp

void floorplan::floor_init(oaBlock *fblock){
	oaBox cellbox;
	int linenum = 0;
	int cellnum, linenum;
	int cellcnt = 0;
	int linecnt = 0;
	node *nnode;
	
	oaIter<oaNet> instIterator(fblock->getInsts());
	cellnum = fblock->getInsts().getCount();
	linenum = cellnum*2;
	cells = (cblock*)new cblock[cellnum];
	v_lines = (inter_lines*)new inter_lines[linenum];
	h_lines = (inter_lines*)new inter_lines[linenum];
	
	// Extract cell data and build floorplan
	while (oaInst* inst = instIterator.getNext()) {
		inst->getBBox(cellbox);
		
		cellbox.getCenter(cells[cellcnt].blkcenter);
		cells[cellcnt].boundary[0] = cellbox.left();
		cells[cellcnt].boundary[1] = cellbox.bottom();
		cells[cellcnt].boundary[2] = cellbox.right();
		cells[cellcnt].boundary[3] = cellbox.top();
		
		// Build intersection lines array and linked list
		v_lines[linecnt].l_pos = cellbox.left();
		h_lines[linecnt].l_pos = cellbox.bottom();
		nnode = new node(linecnt);
		ll_ver.addtail(nnode);
		ll_hor.addtail(nnode);
		linecnt++;
		
		v_lines[linecnt].l_pos = cellbox.right();
		h_lines[linecnt].l_pos = cellbox.top();
		nnode = new node(linecnt);
		ll_ver.addtail(nnode);
		ll_hor.addtail(nnode);
		linecnt++;
		
		cellcnt++;
	}
	
	// Check vertical edges of floorplan
	
	
	// Merge vertical edges of same channel
	
	
	// Check horizontal edges of floorplan
	
	
	// Merge horizontal edges of same channel
	
	
	// Build intervals of each cell (block)

	
}














