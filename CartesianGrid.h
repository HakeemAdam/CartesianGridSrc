#ifndef __CARTESIANGRID_H__
#define __CARTESIANGRID_H__

#include <SOP/SOP_Node.h>

class CartesianGrid : public SOP_Node
{
public:
    static OP_Node *myConstructor(OP_Network *net, const char *name,OP_Operator* op);
    static const PRM_Template gridParms;

protected:
    CartesianGrid(OP_Network *net, const char *name, OP_Operator* op);
    virtual ~CartesianGrid();

    virtual OP_ERROR cookMySop(OP_Context &context);

private:
    
};
#endif
