/* EE201A Winter 2018 Course Project
 */

#ifndef OAHELPER_H
#define	OAHELPER_H

#include <vector>
#include <string>
#include "oaDesignDB.h"

using namespace std;
using namespace oa;

class OAHelper
{
public:
    OAHelper();
    virtual ~OAHelper();
	static oaPoint GetTermPosition(oaTerm* term);
	static oaPoint GetAbsoluteInstTermPosition(oaInstTerm* instTerm);
	static void MovePinToPosition(oaInstTerm* instTerm, oaPoint newAbsolutePos);
	static void MovePinByOffset(oaInstTerm* instTerm, oaPoint offset);


	static int getNetHPWL(oaNet* net);
	static int getBoxHP(vector<oaPoint> &endPoint_v);
	static int getEndPoints(oaNet* p_net, vector<oaTerm*> &term_v, vector<oaInstTerm*> &instTerm_v, vector<oaPoint> &coord_v);
    static void setInstOrient(oaInst* inst, int degree);
    static int getAllInsts(oaDesign *design, vector<oaInst*> &inst_v);
private:

};

#endif	/* OAHELPER_H */

