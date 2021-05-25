#pragma once
#include "aiMeshOps.h"
#include "aiPolyMesh.h"
#include "aiMeshSchema.h"

class aiSubDSample : public aiMeshSample<aiSubD> 
{
public:
    aiSubDSample(aiSubD *schema, TopologyPtr topo);
    ~aiSubDSample();
};


struct aiSubDTraits 
{
    using SampleT = aiSubDSample;
    using AbcSchemaT = AbcGeom::ISubDSchema;
};

class aiSubD : public aiMeshSchema<aiSubDTraits>
{
public:
    aiSubD(aiObject* parent, const abcObject& abc);
    ~aiSubD() override;

    Sample* newSample() override;
};

// Explicit specialization, since SubD does not have normals
template <>
inline AbcGeom::IN3fGeomParam aiMeshSchema<aiSubDTraits>::readNormalsParam()
{
    return AbcGeom::IN3fGeomParam();
}

