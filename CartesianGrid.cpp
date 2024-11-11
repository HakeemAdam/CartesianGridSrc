#include "CartesianGrid.h"
#include "CartesianGrid.h"

#include <UT/UT_DSOVersion.h>

#include <OP/OP_OperatorTable.h>
#include <OP/OP_Operator.h>
#include <OP/OP_AutoLockInputs.h>

#include <GU/GU_Detail.h>

#include <PRM/PRM_Include.h>
#include  <PRM/PRM_Default.h>
#include <PRM/PRM_Template.h>

#include <cmath>

static PRM_Name gridTypes[]=
    {
        PRM_Name("rectangular", "Rectangular"),
        PRM_Name("equilateral", "Equilateral"),
        PRM_Name("isometric", "Isometric"),
        PRM_Name("radial", "Radial"),
        PRM_Name("concentric", "Concentric"),
        PRM_Name(0)
    };

static PRM_ChoiceList gridMenu(PRM_CHOICELIST_SINGLE, gridTypes);

static PRM_Name gridParmNames[]=
    {
        PRM_Name("size", "Size"),
        PRM_Name("center", "Center"),
        PRM_Name("addCenter", "Add Center"),
        PRM_Name("spacing", "Spacing"),
        PRM_Name("gridType", "Grid Type"),
        PRM_Name(0)
    };

static PRM_Default gridParmDefaults[]=
    {
    PRM_Default(5.0f),
    PRM_Default(5.0f),
    PRM_Default(5.0f),
    PRM_Default(0.0f),
    PRM_Default(0.0f),
    PRM_Default(0.0f),
    };

static PRM_Default gridTypeDefault(0);

static PRM_Template gridParms[]=
    {
        PRM_Template(PRM_ORD, 1, &gridParmNames[4], &gridTypeDefault, &gridMenu),
        PRM_Template(PRM_XYZ, 3, &gridParmNames[0], gridParmDefaults),
        PRM_Template(PRM_XYZ, 3, &gridParmNames[1], gridParmDefaults+3),
        PRM_Template(PRM_FLT,1, &gridParmNames[3], &gridParmDefaults[0]),
        PRM_Template(PRM_TOGGLE, 1, &gridParmNames[2], &gridParmDefaults[0]),
        
    
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
= default;


void CartesianGrid::createRectangularGrid(GU_Detail* gdp, int rows, int cols, float spacing)
{
    GA_RWHandleV3 handle = gdp->addFloatTuple(GA_ATTRIB_POINT, "P",3);
    if(!handle.isValid())
    {
        return;
    }

    float startX = -(cols-1)*spacing*0.5f;
    float startY = -(rows-1)*spacing*0.5f;

    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < cols; j++)
        {
            float x = startX + j *spacing;
            float z = startY + i * spacing;
            GA_Offset ptoff = gdp->appendPoint();

            UT_Vector3 pos(x,0,z);
            handle.set(ptoff,pos);
        }
    }
           
}

void CartesianGrid::createTriangularGrid(GU_Detail* gdp, int rows, int cols, float spacing)
{
    GA_RWHandleV3 handle = gdp->addFloatTuple(GA_ATTRIB_POINT, "P",3);
    if(!handle.isValid())
    {
        return;
    }

    const float sqrt3 = sqrt(3.0f);
    float traingleHeight = spacing * sqrt3 * 0.5f;

    float startX = -(cols-1)*spacing*0.5f;
    float startY = -(rows-1)*traingleHeight*0.5f;

    for(int row = 0; row < rows; row++)
    {
        for(int col = 0; col < cols; col++)
        {
            float x = startX + col * spacing;
            float y = startY + row * traingleHeight;

            if(row % 2 ==1)
            {
                x+=spacing*0.5f;
            }
            GA_Offset ptoff = gdp->appendPoint();
            UT_Vector3 pos(x,0,y);
            handle.set(ptoff,pos);
        }
    }
}

void CartesianGrid::createEquilateralTriGrid(GU_Detail* gdp, int rows, int cols, float spacing, bool addCenter)
{
    GA_RWHandleV3 handle = gdp->addFloatTuple(GA_ATTRIB_POINT, "P",3);
    if(!handle.isValid())
    {
        return;
    }

    const float sqrt3 = sqrt(3.0f);
    const float xSpacing = spacing;
    const float ySpacing = spacing * sqrt3 * 0.5f;

    float startX = -(cols-1)*xSpacing*0.5f;
    float startY = -(rows-1)*ySpacing*0.5f;

    for(int row = 0; row < rows; row++)
    {
        for(int col = 0; col < cols; col++)
        {
            float x = startX + col * xSpacing;
            float y = startY + row * ySpacing;

            if(row % 2 ==1)
            {
                x+=xSpacing*0.5f;
            }

            GA_Offset ptoff = gdp->appendPoint();
            UT_Vector3 pos(x,0,y);
            handle.set(ptoff,pos);
        }
    }
    
    if( addCenter)
    {
        for(int row = 0; row < rows-1; row++)
        {
            for(int col = 0; col < cols-1; col++)
            {
                float x = startX + col * xSpacing + xSpacing*0.5f;
                float y = startY + row * ySpacing + ySpacing*0.5f;

                if(row % 2 == 1)
                {
                    x += xSpacing * 0.5f;
                }

                GA_Offset ptoff = gdp->appendPoint();
                UT_Vector3 pos(x,0,y);
                handle.set(ptoff,pos);
            }
        }
    }

    
}

void CartesianGrid::createRadialGrid(GU_Detail* gdp, int rings, int segments, float spacing)
{
    GA_RWHandleV3 handle = gdp->addFloatTuple(GA_ATTRIB_POINT, "P",3);
    if(!handle.isValid())
    {
        return;
    }

    GA_Offset centPT = gdp->appendPoint();
    handle.set(centPT, UT_Vector3(0,0,0));

    for(int r =0; r < rings; r++)
    {
        float radius = r * spacing;

        for(int s=0; s < segments; s++)
        {
            float angle = (2.0f *3.1421*s)/segments;

            float x = radius * cos(angle);
            float y = radius * sin(angle);

            if(abs(x)< 0.0001f && abs(y)< 0.0001f)
                continue;

            GA_Offset ptoff = gdp->appendPoint();
            UT_Vector3 pos(x,0,y);
            handle.set(ptoff,pos);
        }
    }
}

void CartesianGrid::createConcentricGrid(GU_Detail* gdp, int rings, int segments, float spacing)
{
    GA_RWHandleV3 handle = gdp->addFloatTuple(GA_ATTRIB_POINT,"P",3);
    if(!handle.isValid())
    {
        return;
    }
    GA_Offset centPT = gdp->appendPoint();
    handle.set(centPT, UT_Vector3(0,0,0));

    for(int r =0; r < rings; r++)
    {
        float radius = r * spacing;
        int ringSegments = std::max(6,int(2 * 3.1421*radius/spacing));

        for(int s=0; s < ringSegments; s++)
        {
            float angle = (2.0f *3.1421*s)/ringSegments;
            float x = radius * cos(angle);
            float y = radius * sin(angle);

            if(abs(x)< 0.0001f && abs(y)< 0.0001f)
                continue;
            GA_Offset ptoff = gdp->appendPoint();
            UT_Vector3 pos(x,0,y);
            handle.set(ptoff,pos);
        }
    }
}


OP_ERROR CartesianGrid::cookMySop(OP_Context& context)
{
    OP_AutoLockInputs inputs(this);
    if(inputs.lock(context) >= UT_ERROR_ABORT)
        return error();

    gdp->clear();

    fpreal now = context.getTime();

    float gridType = evalInt("gridType", 0, now);
    float rows = evalFloat("size",0,now);
    float cols = evalFloat("size",2,now);
    float spacing = evalFloat("spacing",0,now);
    bool addCenter = evalInt("addCenter",0,now);

    if(gridType==0)
    {
        createRectangularGrid(gdp,rows,cols,spacing);
    }else if (gridType==1)
    {
        createEquilateralTriGrid(gdp,rows,cols,spacing, addCenter);
    } else if(gridType==2)
    {
        createTriangularGrid(gdp,rows,cols,spacing);
    } else if (gridType==3)
    {
        createRadialGrid(gdp,rows,cols,spacing);
    } else if (gridType==4)
    {
        createConcentricGrid(gdp,rows,cols,spacing);
    }

   // createRectangularGrid(gdp,5,5,1);
   // createTriangularGrid(gdp,5,5,1);
    //createEquilateralTriGrid(gdp,5,5,1);
    //gdp->pointGrid(rows,cols, 1, 1, 0,0,0,GU_PLANE_XY );
    
   
    

    return error();
}


