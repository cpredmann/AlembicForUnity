#pragma once
#include "aiMeshSchema.h"

class aiPolyMeshSample : public aiMeshSample<aiPolyMesh>
{
public:
    aiPolyMeshSample(aiPolyMesh* schema, TopologyPtr topo);
    ~aiPolyMeshSample();
};

struct aiPolyMeshTraits
{
    using SampleT = aiPolyMeshSample;
    using AbcSchemaT = AbcGeom::IPolyMeshSchema;
};

class aiPolyMesh : public aiMeshSchema<aiPolyMeshTraits>
{
public:
    aiPolyMesh(aiObject *parent, const abcObject &abc);
    ~aiPolyMesh() override;

    Sample* newSample() override;
};

template<class Container>
static inline int CalculateTriangulatedIndexCount(const Container& counts)
{
    int r = 0;
    size_t n = counts.size();
    for (size_t fi = 0; fi < n; ++fi)
    {
        int ngon = counts[fi];
        r += (ngon - 2) * 3;
    }
    return r;
}

template<class T, class IndexArray>
inline void CopyWithIndices(T *dst, const T *src, const IndexArray& indices)
{
    if (!dst || !src) { return; }
    size_t size = indices.size();
    for (size_t i = 0; i < (int)size; ++i)
    {
        dst[i] = src[indices[i]];
    }
}

template<class T, class AbcArraySample>
inline void Remap(RawVector<T>& dst, const AbcArraySample& src, const RawVector<int>& indices)
{
    if (indices.empty())
    {
        dst.assign(src.get(), src.get() + src.size());
    }
    else
    {
        dst.resize_discard(indices.size());
        CopyWithIndices(dst.data(), src.get(), indices);
    }
}

template<class T>
inline void Lerp(RawVector<T>& dst, const RawVector<T>& src1, const RawVector<T>& src2, float w)
{
    if (src1.size() != src2.size())
    {
        DebugError("something is wrong!!");
        return;
    }
    dst.resize_discard(src1.size());
    Lerp(dst.data(), src1.data(), src2.data(), (int)src1.size(), w);
}

template<class T>
static inline void copy_or_clear(T* dst, const IArray<T>& src, const MeshRefiner::Split& split)
{
    if (dst)
    {
        if (!src.empty())
            src.copy_to(dst, split.vertex_count, split.vertex_offset);
        else
            memset(dst, 0, split.vertex_count * sizeof(T));
    }
}

template<class T1, class T2>
static inline void copy_or_clear_3_to_4(T1* dst, const IArray<T2>& src, const MeshRefiner::Split& split)
{
    if (dst)
    {
        if (!src.empty()) {
            std::vector<T1> abc4(split.vertex_count);
            std::transform(src.begin(), src.end(), abc4.begin(), [](const T2& c){ return T1{c.x, c.y, c.z, 1.f}; });
            memcpy(dst, abc4.data() + split.vertex_offset, sizeof(T1) * split.vertex_count);
        } else {
            memset(dst, 0, split.vertex_count * sizeof(T1));
        }
    }
}
