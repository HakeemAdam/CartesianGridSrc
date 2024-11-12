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
        PRM_Name("concentric", "Concentric"),
        PRM_Name("hexagonal", "Hexagonal"),
        PRM_Name(0)
    };

static PRM_ChoiceList gridMenu(PRM_CHOICELIST_SINGLE, gridTypes);

static PRM_Name gridParmNames[]=
    {
        PRM_Name("size", "Size"),
        PRM_Name("center", "Center"),
        PRM_Name("addCenter", "Add Center"),
        PRM_Name("spacing", "Spacing"),
        PRM_Name("useEvenSpacing", "Use Even Spacing"),
        PRM_Name("gridType", "Grid Type"),
        PRM_Name("minPoints", "Min Points"),
        PRM_Name("spiralFactor", "Spiral Factor"),
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
    PRM_Default(6.0f),
    };

static PRM_Default gridTypeDefault(0);

static PRM_Template gridParms[]=
    {
        PRM_Template(PRM_ORD, 1, &gridParmNames[5], &gridTypeDefault, &gridMenu),
        PRM_Template(PRM_XYZ, 3, &gridParmNames[0], gridParmDefaults),
        PRM_Template(PRM_XYZ, 3, &gridParmNames[1], gridParmDefaults+3),
        PRM_Template(PRM_FLT,1, &gridParmNames[3], &gridParmDefaults[0]),
        PRM_Template(PRM_TOGGLE, 1, &gridParmNames[2], &gridParmDefaults[4]),
        PRM_Template(PRM_TOGGLE, 1, &gridParmNames[4], &gridParmDefaults[4]),
        PRM_Template(PRM_INT,1, &gridParmNames[6], &gridParmDefaults[6]),
        PRM_Template(PRM_FLT,1, &gridParmNames[7], &gridParmDefaults[5]),
        
    
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


void CartesianGrid::createRectangularGrid(GU_Detail* gdp, int rows, int cols, float spacing, UT_Vector3& center)
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

            UT_Vector3 pos(x +center.x(),center.y(),z + center.z());
            handle.set(ptoff,pos);
        }
    }
           
}


void CartesianGrid::createEquilateralTriGrid(GU_Detail* gdp, int rows, int cols, float spacing, bool addCenter, UT_Vector3& center)
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
            UT_Vector3 pos(x +center.x(),center.y(),y + center.z());
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


void CartesianGrid::createConcentricGrid(GU_Detail* gdp, int rings, int segments, float spacing, bool useEvenSpacing,  int minPoints,  float spiralFactor, UT_Vector3& center)
{
    GA_RWHandleV3 handle = gdp->addFloatTuple(GA_ATTRIB_POINT,"P",3);
    if(!handle.isValid())
    {
        return;
    }
    GA_Offset centPT = gdp->appendPoint();
    handle.set(centPT, center);

    for(int r =0; r < rings; r++)
    {
        float radius = r * spacing;

        int pointOnRing;
        if(useEvenSpacing)
        {
            pointOnRing = std::max(minPoints,int(2 * 3.1421*radius/spacing));
        }
        else
        {
            pointOnRing=segments;
        }
        

        for(int s=0; s < pointOnRing; s++)
        {
            float angle = (2.0f *3.1421*s)/pointOnRing+(spiralFactor*r);
            float x = radius * cos(angle);
            float y = radius * sin(angle);

            if(abs(x)< 0.0001f && abs(y)< 0.0001f)
                continue;
            
            GA_Offset ptoff = gdp->appendPoint();
            UT_Vector3 pos(x +center.x(),center.y(),y + center.z());
            handle.set(ptoff,pos);
        }
    }
}

void CartesianGrid::createHexagonalgrid(GU_Detail* gdp, int rows, int cols, float spacing, UT_Vector3& center)
{
    GA_RWHandleV3 handle = gdp->addFloatTuple(GA_ATTRIB_POINT, "P",3);
    if(!handle.isValid())
        return;
    //if(rows <=0 || cols <=0 || spacing <= 0 < 0.0001f) return;

    const float sqrt3 = sqrt(3.0f);
    float xSpacing = spacing * sqrt3;
    float ySpacing = spacing * 1.5f;

    float startX = -(cols-1)*xSpacing*0.5f;
    float startY = -(rows-1)*ySpacing*0.5f;

    for (int row = 0; row < rows; row++)
    {
        for(int col = 0; col < cols; col++)
        {
            float x = startX + col * xSpacing;
            float y = startY + row * ySpacing;

            if(row % 2 == 1)
            {
                x+=xSpacing*0.5f;
            }

            if(abs(x)<0.0001f && abs(y)<0.0001f)
                continue;

            GA_Offset ptoff = gdp->appendPoint();
            UT_Vector3 pos(x +center.x(),center.y(),y + center.z());
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
    bool useSpacing = evalInt("useEvenSpacing", 0, now);
    int minPoints = evalInt("minPoints",1,now);
    float spiralFactor = evalFloat("spiralFactor",0,now);
    UT_Vector3 center;
    center.x()=evalFloat("center",0,now);
    center.y()=evalFloat("center",1,now);
    center.z()=evalFloat("center",2,now);

    switch(static_cast<int>(gridType))
    {
        case 0:
            createRectangularGrid(gdp,rows,cols,spacing, center);
            break;
        case 1:
            createEquilateralTriGrid(gdp,rows,cols,spacing, addCenter,center);
            break;
        case 2:
            createConcentricGrid(gdp,rows,cols,spacing, useSpacing, minPoints, spiralFactor,center);
            break;
        case 3:
            createHexagonalgrid(gdp,rows,cols,spacing,center);
            break;
        default:
            createRectangularGrid(gdp,rows,cols,spacing,center);
            break;
    }
 
    return error();
}


