/* EE201A Winter 2018 Course Project
 */

#include "OAHelper.h"
#include <iostream>
using namespace std;

OAHelper::OAHelper()
{
}

OAHelper::~OAHelper()
{
}

/* Function returns position of an oaTerm as an oaPoint
 * oaTerm -> oaPin -> oaPinFig -> oaBox -> getCenter()
 * Position relative to origin of Instance
 */
oaPoint OAHelper::GetTermPosition(oaTerm* term)
{
	oaIter<oaPin> pinIterator(term->getPins());
	oaIter<oaPinFig> pinFigIterator(pinIterator.getNext()->getFigs());
	oaPinFig* pinFig = pinFigIterator.getNext();
	oaBox box; oaPoint point;
	pinFig->getBBox(box);
	box.getCenter(point);
	return point;
}

/* Function returns ABSOLUTE position of an oaInstTerm as an oaPoint
 * The associated oaTerm is fetched, which is basically the corresponding pin inside the macro block
 * The oaTerm position is then added to the oaInst origin to get the absolute instTerm position
 * Note: We cannot simply use the oaInst origin as the oaInstTerm position(as in Labs 1 & 3).
 */
oaPoint OAHelper::GetAbsoluteInstTermPosition(oaInstTerm* instTerm)
{
	//Get relative position of associated terminal inside the instance master
	//Not the term of the whole chip
	oaTerm* assocTerm = instTerm->getTerm();
	oaPoint relativePos = GetTermPosition(assocTerm);
    
	//Get the origin and orientation of the instance
	oaInst* inst = instTerm->getInst();
	oaPoint instOrigin;
	inst->getOrigin(instOrigin);
	oaOrient orient = inst->getOrient();
	
	//Rotate relative position by orient
	oaTransform trans = oaTransform(orient);
	relativePos.transform(trans);
	
	//Compute absolute position in global design
	oaPoint absolutePos = relativePos + instOrigin;
	return absolutePos;
}

/* Function moves the pin (oaTerm) associated with instTerm to ABSOLUTE position newPos
 */
void OAHelper::MovePinToPosition(oaInstTerm* instTerm, oaPoint newAbsolutePos)
{
	//Get current relative position of associated term
	oaTerm* assocTerm = instTerm->getTerm();
	oaPoint currentRelativePos = GetTermPosition(assocTerm);
	
	//Get origin, orientation and inverse orientation of inst
	oaInst *inst = instTerm->getInst();
	oaPoint instOrigin;
	inst->getOrigin(instOrigin);
	oaOrient orient = inst->getOrient();
	oaOrient inverseOrient = orient.getRelativeOrient(oacR0);

	//Find new relative position and rotate it according to inverse orient
	oaPoint newRelativePos = newAbsolutePos - instOrigin;
	oaTransform trans = oaTransform(inverseOrient);
	newRelativePos.transform(trans);
	
	//Calculate offset
	oaPoint offset = newRelativePos - currentRelativePos;
	oaTransform trans2 = oaTransform(offset);
	
	//Find associated term's pinfig and apply move
	oaIter<oaPin> pinIterator(assocTerm->getPins());
	oaIter<oaPinFig> pinFigIterator(pinIterator.getNext()->getFigs());
	oaPinFig* pinFig = pinFigIterator.getNext();

	pinFig->move(trans2);
}

/* Function moves the pin (oaTerm) associated with instTerm by the offset oaPoint
 */
void OAHelper::MovePinByOffset(oaInstTerm* instTerm, oaPoint offset)
{
	//Get orientation of inst, and find its inverse
	oaInst *inst = instTerm->getInst();
	oaOrient orient = inst->getOrient();
	oaOrient inverseOrient = orient.getRelativeOrient(oacR0);
	
	//Rotate offset according to inverse orient
	oaTransform trans = oaTransform(inverseOrient);
	offset.transform(trans);
		
	oaTransform trans2 = oaTransform(offset);

	//Find associated term's pinfig and apply move
	oaTerm* assocTerm = instTerm->getTerm();
	oaIter<oaPin> pinIterator(assocTerm->getPins());
	oaIter<oaPinFig> pinFigIterator(pinIterator.getNext()->getFigs());
	oaPinFig* pinFig = pinFigIterator.getNext();

	pinFig->move(trans2);
}

// Get the HPWL of a net
int OAHelper::getNetHPWL(oaNet* net){
	vector<oaTerm*> term_v;
	vector<oaInstTerm*> instTerm_v;
	vector<oaPoint> coord_v;
	getEndPoints(net, term_v, instTerm_v, coord_v);
	return getBoxHP(coord_v);
}

// Given a vector of oaPoint, find the smallest bonding 
// box that enclose all the points and return the HPWL
int OAHelper::getBoxHP(vector<oaPoint> &endPoint_v){ 
	int max_x = 0;
	int max_y = 0;
	int min_x = 0;
	int min_y = 0;
	for( int i = 0; i < endPoint_v.size() ; i++){
		max_x = (endPoint_v[i].x() > max_x) ? endPoint_v[i].x() : max_x;
		max_y = (endPoint_v[i].y() > max_y) ? endPoint_v[i].y() : max_y;
		if ( i != 0 ){
			min_x = (endPoint_v[i].x() < min_x) ? endPoint_v[i].x() : min_x;
			min_y = (endPoint_v[i].y() < min_y) ? endPoint_v[i].y() : min_y;
		}
		else{
			min_x = endPoint_v[i].x();
			min_y = endPoint_v[i].y();
		}
		
	}
	return max_x - min_x + max_y - min_y;
}

// Get all the end points of a net, including term and instTerm
// Store oaTerm and oaInstTerm in two vectors
// Return total number of endpoints
int OAHelper::getEndPoints(oaNet* p_net, vector<oaTerm*> &term_v, vector<oaInstTerm*> &instTerm_v, vector<oaPoint> &coord_v){
	int count = 0;
	oaIter<oaTerm> termIterator(p_net->getTerms());
	while (oaTerm *term = termIterator.getNext()){
		term_v.push_back(term);
		//Get the origin of term
		oaPoint tmpPoint;
		oaIter<oaPin> pinIterator(term->getPins());
        oaPin *termPin = pinIterator.getNext();    
        oaIter<oaPinFig> pinFigIterator(termPin->getFigs());
        oaPinFig *termPinFig = pinFigIterator.getNext();
        oaBox termPinBox;
        termPinFig->getBBox(termPinBox);
        termPinBox.getCenter(tmpPoint);
        coord_v.push_back(tmpPoint);
		count++;
	}
	oaIter<oaInstTerm> instTermIterator(p_net->getInstTerms());
	while (oaInstTerm *instTerm = instTermIterator.getNext()){
		instTerm_v.push_back(instTerm);
		//Get the origin of inst
		oaPoint tmpPoint;
		oaInst *inst = instTerm->getInst();
        inst->getOrigin(tmpPoint);
        coord_v.push_back(tmpPoint);
		count++;
	}
	return count;
}