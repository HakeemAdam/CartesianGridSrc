#include "CartesianGrid.h"

#include <UT/UT_DSOVersion.h>

#include <OP/OP_OperatorTable.h>
#include <OP/OP_Operator.h>
#include <OP/OP_AutoLockInputs.h>

#include <GU/GU_Detail.h>

#include <PRM/PRM_Include.h>
#include  <PRM/PRM_Default.h>
#include <PRM/PRM_Template.h>

static PRM_Template gridParms[]=
    {
        PRM_Template()
    };

void newSopOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator(
        "PG::CartesianGrid",
        "PG::CartesianGrid",
        CartesianGrid::myConstructor,
        gridParms,
        0,
        0,
        nullptr));
}

OP_Node* CartesianGrid::myConstructor(OP_Network* net, const char* name, OP_Operator* op)
{
    return new CartesianGrid(net, name, op);
}

CartesianGrid::CartesianGrid(OP_Network* net, const char* name, OP_Operator* op)
    :SOP_Node(net,name,op)
{
    mySopFlags.setManagesDataIDs(true);
}

CartesianGrid::~CartesianGrid()
{
}

OP_ERROR CartesianGrid::cookMySop(OP_Context& context)
{
    OP_AutoLockInputs inputs(this);
    if(inputs.lock(context) >= UT_ERROR_ABORT)
        return error();

    return error();
}
