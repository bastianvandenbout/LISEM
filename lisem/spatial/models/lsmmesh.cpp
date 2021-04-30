#include "LSMmesh.h"
#include "error.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "geometry/triangleintersect.h"
/*bool LSMMesh::Setup(OpenGLCLManager * m)
{

    m_IsSetup = true;
    m_IsInstanced = false;

    //setup of mesh


    glad_glGenVertexArrays(1, &VAO);
    glad_glGenBuffers(1, &VBO);
    glad_glGenBuffers(1, &EBO);

    glad_glBindVertexArray(VAO);
    glad_glBindBuffer(GL_ARRAY_BUFFER, VBO);


    glad_glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glad_glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 &indices[0], GL_STATIC_DRAW);

    // vertex positions
    glad_glEnableVertexAttribArray(0);
    glad_glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)Vertex::positionOffset());
    // vertex normals
    glad_glEnableVertexAttribArray(1);
    glad_glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)Vertex::uvOffset());
    // vertex texture coords
    glad_glEnableVertexAttribArray(2);
    glad_glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)Vertex::normalOffset());
    // vertex texture coords
    glad_glEnableVertexAttribArray(3);
    glad_glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)Vertex::tangentOffset());
    // vertex texture coords
    glad_glEnableVertexAttribArray(4);
    glad_glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)Vertex::bitangentOffset());
    // vertex texture coords
    glad_glEnableVertexAttribArray(5);
    glad_glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)Vertex::colorOffset());

    glad_glBindVertexArray(0);


    //setup of textures

    return true;
}

bool LSMMesh::SetupInstanced(OpenGLCLManager * m, int n)
{

    if(m_IsSetup)
    {
        //if we already have the correct number
        if(m_IsInstanced)
        {
            if(m_NumInstances == n)
            {
                return true;
            }
        }

        //otherwise delete first, then reupload
        if(m_IsInstanced)
        {
            glad_glDeleteBuffers(1,&InstanceMBO);
            delete[] m_InstanceMatrices;
        }
        m_NumInstances = n;

        m_InstanceMatrices = new LSMMatrix4x4[n];

        // vertex buffer object
        glad_glGenBuffers(1, &InstanceMBO);
        glad_glBindBuffer(GL_ARRAY_BUFFER, InstanceMBO);
        glad_glBufferData(GL_ARRAY_BUFFER, n * sizeof(LSMMatrix4x4), &m_InstanceMatrices[0], GL_STATIC_DRAW);

        m_IsInstanced = true;

        //setup of mesh

        glad_glBindVertexArray(VAO);


        std::size_t vec4Size = sizeof(LSMVector4);
        glad_glEnableVertexAttribArray(6);
        glad_glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
        glad_glEnableVertexAttribArray(7);
        glad_glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
        glad_glEnableVertexAttribArray(8);
        glad_glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
        glad_glEnableVertexAttribArray(9);
        glad_glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

        glad_glVertexAttribDivisor(6, 1);
        glad_glVertexAttribDivisor(7, 1);
        glad_glVertexAttribDivisor(8, 1);
        glad_glVertexAttribDivisor(9, 1);

        //setup of textures

        glad_glBindVertexArray(0);
        std::cout<<"!!!!!!!!!!!!!!!!!!!! create instanced" << std::endl;
        return true;
    }

    return false;
}

*/

BoundingBox3D LSMMesh::GetBoundingBox(bool calc)
{

    BoundingBox3D b;

    if(calc)
    {
        int vCount = vertices.size();

        for (long a = 0; a < vCount; a++)
        {
            const LSMVector3& v1 = vertices[a].position();

            if(a == 0)
            {
               b.Set(v1);
            }else
            {
                b.Merge(v1);
            }
        }
    }
    m_BoundingBox = b;


    return m_BoundingBox;


}
BoundingBox3D LSMMesh::GetAndCalcBoundingBox()
{
    return GetBoundingBox(true);
}




LSMMesh LSMMesh::GetCopy()
{
    LSMMesh cop;
    cop.vertices = vertices;
    cop.indices = indices;
    cop.m_Material = m_Material.Copy();
    return std::move(cop);
}

LSMMesh::LSMMesh(std::vector<Vertex> in_vertices, std::vector<unsigned int> in_indices, ModelMaterial m, int mindex )
{
    vertices = in_vertices;
    indices = in_indices;
    m_Material = m;
    m_MaterialIndex = mindex;
}

 int LSMMesh::GetVertexCount()
{
    return vertices.size();
}

 int LSMMesh::GetFaceCount()
{
    return indices.size()/3;
}

 Vertex LSMMesh::GetVertex(int i)
{
    return vertices.at(i);
}
 Face LSMMesh::GetFace(int i)
{
    Face f;
    f.i_1 = indices.at(i*3 + 0);
    f.i_2 = indices.at(i*3 + 1);
    f.i_3 = indices.at(i*3 + 2);
    return f;
}

 void LSMMesh::SetMaterial(ModelMaterial m)
{
    m_Material = m;
}

 float LSMMesh::RayCast(LSMVector3 O, LSMVector3 Dir)
{

    bool found = false;
    float dist_min = 0.0f;
    int triangleCount = indices.size()/3;

    for (long a = 0; a < triangleCount; a++)
    {
        long i1 = indices.at(a * 3 + 0);
        long i2 = indices.at(a * 3 + 1);
        long i3 = indices.at(a * 3 + 2);

        const LSMVector3& v1 = vertices[i1].position();
        const LSMVector3& v2 = vertices[i2].position();
        const LSMVector3& v3 = vertices[i3].position();


        float dist = rayTriangleIntersect(Ray(O,Dir),v1,v2,v3);

        if(std::isfinite(dist))
        {
            if(found == false)
            {
                found = true;
                dist_min = dist;
            }else if(dist < dist_min)
            {
                dist_min = dist;
            }
        }

    }

    if(found)
    {
        return dist_min;
    }else
    {
        return -INFINITYFLTLSM;
    }

}

 void LSMMesh::CalculateNormalsAndTangents()
{
    CalculateNormals();
    CalculateTangents();
}

 void LSMMesh::CalcIsConvex()
{
    m_IsConvex = int_Calc_IsConvex();
}

 void LSMMesh::SetIsConvex(bool conv)
{
    m_IsConvex = conv;
}

 void LSMMesh::CalculateNormals()
{
    int vertexCount = vertices.size();
    LSMVector3 *norms1 = new LSMVector3[vertexCount];

    int triangleCount = indices.size()/3;

    for (long a = 0; a < triangleCount; a++)
    {
        long i1 = indices.at(a * 3 + 0);
        long i2 = indices.at(a * 3 + 1);
        long i3 = indices.at(a * 3 + 2);

        const LSMVector3& v1 = vertices[i1].position();
        const LSMVector3& v2 = vertices[i2].position();
        const LSMVector3& v3 = vertices[i3].position();

        const LSMVector2& w1 = vertices[i1].uv();
        const LSMVector2& w2 = vertices[i2].uv();
        const LSMVector2& w3 = vertices[i3].uv();

        LSMVector3 norm = LSMVector3::CrossProduct(v2-v1,v3-v1).Normalize();

        norms1[i1] = norms1[i1] + norm;
        norms1[i2] = norms1[i2] + norm;
        norms1[i3] = norms1[i3] + norm;

    }

    for (long a = 0; a < vertexCount; a++)
    {
        vertices[a].setTangent(norms1[a].Normalize());
    }

    delete[] norms1;
}

 void LSMMesh::CalculateTangents()
{
    int vertexCount = vertices.size();
    LSMVector3 *tan1 = new LSMVector3[vertexCount * 2];
    LSMVector3 *tan2 = tan1 + vertexCount;

    int triangleCount = indices.size()/3;

    for (long a = 0; a < triangleCount; a++)
    {
        long i1 = indices.at(a * 3 + 0);
        long i2 = indices.at(a * 3 + 1);
        long i3 = indices.at(a * 3 + 2);

        const LSMVector3& v1 = vertices[i1].position();
        const LSMVector3& v2 = vertices[i2].position();
        const LSMVector3& v3 = vertices[i3].position();

        const LSMVector2& w1 = vertices[i1].uv();
        const LSMVector2& w2 = vertices[i2].uv();
        const LSMVector2& w3 = vertices[i3].uv();

        float x1 = v2.x - v1.x;
        float x2 = v3.x - v1.x;
        float y1 = v2.y - v1.y;
        float y2 = v3.y - v1.y;
        float z1 = v2.z - v1.z;
        float z2 = v3.z - v1.z;

        float s1 = w2.x - w1.x;
        float s2 = w3.x - w1.x;
        float t1 = w2.y - w1.y;
        float t2 = w3.y - w1.y;

        float r = 1.0F / (s1 * t2 - s2 * t1);
        LSMVector3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
                (t2 * z1 - t1 * z2) * r);
        LSMVector3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
                (s1 * z2 - s2 * z1) * r);

        tan1[i1] = tan1[i1] + sdir;
        tan1[i2] = tan1[i2] + sdir;
        tan1[i3] = tan1[i3] + sdir;

        tan2[i1] = tan2[i1] + tdir;
        tan2[i2] = tan2[i2] + tdir;
        tan2[i3] = tan2[i3] + tdir;
    }

    for (long a = 0; a < vertexCount; a++)
    {
        LSMVector3 n = vertices[a].normal();
        LSMVector3 t = tan1[a];

        // Gram-Schmidt orthogonalize
        vertices[a].setTangent((t - n * (n.dot(t))).Normalize());

        vertices[a].setBiTangent(LSMVector3::CrossProduct(vertices[a].normal(),vertices[a].tangent()));
        // Calculate handedness
        //tangent[a].w = n.CrossProduct(t).dot(tan2[a]) < 0.0f? -1.0f:1.0f;
    }

    delete[] tan1;

}

 void LSMMesh::SetAsEllipsoidTruncated(f32 radiusH, f32 radiusV, f32 Ylow, f32 Yhigh, f32 offset, unsigned int polyCountX, unsigned int polyCountY)
{
    //code from Project Chrono

    // The following code is based on a modified version of the
    // Irrlicht createLSMereMesh function, that is also based on the
    // work by Alfaz93
    bool disc_high = false;
    bool disc_low = false;
    if (Yhigh < radiusV)
        disc_high = true;
    if (Ylow > -radiusV)
        disc_low = true;

    if (polyCountX < 2)
        polyCountX = 2;
    if (polyCountY < 2)
        polyCountY = 2;
    if (polyCountX * polyCountY > 32767) {  // prevent u16 overflow
        if (polyCountX > polyCountY)      // prevent u16 overflow
            polyCountX = 32767 / polyCountY - 1;
        else
            polyCountY = 32767 / (polyCountX + 1);
    }

    unsigned int polyCountXPitch = polyCountX + 1;  // get to same vertex on next level
    unsigned int n_tot_verts = (polyCountXPitch * polyCountY) + 2;
    unsigned int n_tot_verts_withoutdiscs = n_tot_verts;
    if (disc_high)
        n_tot_verts += polyCountXPitch;
    if (disc_low)
        n_tot_verts += polyCountXPitch;
    unsigned int n_tot_indeces = (polyCountX * polyCountY) * 6;

    vertices.resize(n_tot_verts);
    indices.resize(n_tot_indeces);

    LSMVector4 clr(100, 255, 255, 255);

    unsigned int i = 0;
    unsigned int i_disc = 0;
    unsigned int level = 0;

    for (unsigned int p1 = 0; p1 < polyCountY - 1; ++p1) {
        // main quads, top to bottom
        for (unsigned int p2 = 0; p2 < polyCountX - 1; ++p2) {
            const unsigned int curr = level + p2;
            indices[i] = curr + polyCountXPitch;
            indices[++i] = curr;
            indices[++i] = curr + 1;
            indices[++i] = curr + polyCountXPitch;
            indices[++i] = curr + 1;
            indices[++i] = curr + 1 + polyCountXPitch;
            ++i;
        }

        // the connectors from front to end
        indices[i] = level + polyCountX - 1 + polyCountXPitch;
        indices[++i] = level + polyCountX - 1;
        indices[++i] = level + polyCountX;
        ++i;

        indices[i] = level + polyCountX - 1 + polyCountXPitch;
        indices[++i] = level + polyCountX;
        indices[++i] = level + polyCountX + polyCountXPitch;
        ++i;
        level += polyCountXPitch;
    }

    unsigned int polyCountSq = polyCountXPitch * polyCountY;          // top point
    unsigned int polyCountSq1 = polyCountSq + 1;                      // bottom point
    unsigned int polyCountSqM = 0;                                    // first row's first vertex
    unsigned int polyCountSqM1 = (polyCountY - 1) * polyCountXPitch;  // last row's first vertex

    if (disc_high && !disc_low)
        polyCountSqM = n_tot_verts_withoutdiscs;
    if (disc_low && !disc_high)
        polyCountSqM1 = n_tot_verts_withoutdiscs;
    if (disc_low && disc_high) {
        polyCountSqM = n_tot_verts_withoutdiscs;
        polyCountSqM1 = polyCountSqM + polyCountX + 1;
    }

    for (unsigned int p2 = 0; p2 < polyCountX - 1; ++p2) {
        // create triangles which are at the top of the LSMere
        indices[i] = polyCountSq;
        indices[++i] = polyCountSqM + p2 + 1;
        indices[++i] = polyCountSqM + p2;
        ++i;

        // create triangles which are at the bottom of the LSMere
        indices[i] = polyCountSqM1 + p2;
        indices[++i] = polyCountSqM1 + p2 + 1;
        indices[++i] = polyCountSq1;
        ++i;
    }

    // create final triangle which is at the top of the LSMere
    indices[i] = polyCountSq;
    indices[++i] = polyCountSqM + polyCountX;
    indices[++i] = polyCountSqM + polyCountX - 1;
    ++i;

    // create final triangle which is at the bottom of the LSMere
    indices[i] = polyCountSqM1 + polyCountX - 1;
    indices[++i] = polyCountSqM1;
    indices[++i] = polyCountSq1;

    // calculate the angle which separates all points in a circle
    f64 r_low, alpha_low, r_high, alpha_high;
    if (disc_high) {
        r_high = radiusH * sqrt(1 - pow(Yhigh / radiusV, 2));
        alpha_high = atan((radiusH / radiusV) * (Yhigh / r_high));
    } else {
        r_high = 0;
        alpha_high = LISEM_PI  / 2;
    }

    if (disc_low) {
        r_low = radiusH * sqrt(1 - pow(Ylow / radiusV, 2));
        alpha_low = atan((radiusH / radiusV) * (Ylow / r_low));
    } else {
        r_low = 0;
        alpha_low = -(LISEM_PI / 2);
    }

    const f64 AngleX = 2 * LISEM_PI / polyCountX;
    f64 borderslice = polyCountY;
    if (disc_high)
        borderslice--;
    if (disc_low)
        borderslice--;
    const f64 AngleY = (alpha_high - alpha_low) / borderslice;  //= irr::core::PI / polyCountY;

    i = 0;
    i_disc = n_tot_verts_withoutdiscs;
    f64 axz;

    f64 ay = (LISEM_PI / 2) - alpha_high;  //=0   //=AngleY / 2;  // meant to work in 0..PI, building from top
    if (!disc_high)
        ay += AngleY;

    for (unsigned int y = 0; y < polyCountY; ++y) {
        const f64 sinay = sin(ay);
        const f64 cosay = cos(ay);
        axz = 0;

        // calculate the necessary vertices without the doubled one
        for (unsigned int xz = 0; xz < polyCountX; ++xz) {
            // calculate points position

            LSMVector3 pos((f32)(radiusH * cos(axz) * sinay), (f32)(radiusV * cosay),
                                     (f32)(radiusH * sin(axz) * sinay));
            // for LSMeres the normal is the position
            LSMVector3 normal(pos);
            normal.Normalize();

            // add the offset
            LSMVector3 Roffset((f32)(offset * cos(axz)), 0, (f32)(offset * sin(axz)));
            pos =pos + Roffset;

            // calculate texture coordinates via LSMere mapping
            // tu is the same on each level, so only calculate once
            f32 tu = 0.5f;
            if (y == 0) {
                if (normal.Y() != -1.0f && normal.Y() != 1.0f)
                {
                    tu = (f32)(std::acos(clampf(normal.X() / sinay, -1.0f, 1.0f)) * 0.5f * LISEM_1DIVPI);
                }
                if (normal.Z() < 0.0f)
                {
                    tu = 1 - tu;
                }
            } else
            {
                tu = vertices[i - polyCountXPitch].uv().x;
            }

            Vertex ve;
            ve.setPosition(LSMVector3(pos.X(), pos.Y(), pos.Z()));
            ve.setNormal(LSMVector3(normal.X(), normal.Y(), normal.Z()));
            ve.setUV(LSMVector2(tu,(f32)(ay * LISEM_1DIVPI)));

            vertices[i] = ve;

            ++i;

            axz += AngleX;
        }
        // This is the doubled vertex on the initial position
        vertices[i] = vertices[i - polyCountX];
        vertices[i].setUV(LSMVector2(1.0f,vertices[i].uv().y));
        ++i;
        ay += AngleY;
    }

    // the vertex at the top of the LSMere
    if (disc_high)
        vertices[i] = Vertex(LSMVector3(0.0f, Yhigh, 0.0f),LSMVector3(1.0,1.0,1.0),LSMVector3(0.0f, 1.0f, 0.0f),LSMVector2(0.5f, 0.0f));
    else
        vertices[i] = Vertex(LSMVector3(0.0f, radiusV, 0.0f),LSMVector3(1.0,1.0,1.0),LSMVector3(0.0f, 1.0f, 0.0f),LSMVector2(0.5f, 0.0f));

    // the vertex at the bottom of the LSMere
    ++i;
    if (disc_low)
        vertices[i] = Vertex(LSMVector3(0.0f, Ylow, 0.0f),LSMVector3(1.0,1.0,1.0),LSMVector3(0.0f, 1.0f, 0.0f),LSMVector2(0.5f, 1.0f));
    else
        vertices[i] = Vertex(LSMVector3(0.0f, -radiusV, 0.0f),LSMVector3(1.0,1.0,1.0),LSMVector3(0.0f, 1.0f, 0.0f),LSMVector2(0.5f, 1.0f));

    i_disc = n_tot_verts_withoutdiscs;
    if (disc_high) {
        for (unsigned int xz = 0; xz < (polyCountX + 1); ++xz) {
            // duplicate points for top disc
            vertices[i_disc] = vertices[i_disc - n_tot_verts_withoutdiscs];
            vertices[i_disc].setNormal(LSMVector3(0, 1, 0));
            i_disc++;
        }
    }
    int ifrom = 0;
    int mshift = (polyCountY - 1) * polyCountXPitch;  // last row's first vertex
    if (disc_low) {
        for (unsigned int xz = 0; xz < (polyCountX + 1); ++xz) {
            // duplicate points for low disc
            vertices[i_disc] = vertices[ifrom + mshift];
            vertices[i_disc].setNormal(LSMVector3(0, -1, 0));
            ifrom++;
            i_disc++;
        }
    }


    this->SetIsConvex(true);
}


 void LSMMesh::SetAsCube(LSMVector3 size)
{

    //code from Project Chrono

    // Create indices
    const unsigned int u[36] = {0,  1,  2,  0,  2,  3,  4,  6,  5,  4,  7,  6,  8,  9,  10, 8,  10, 11,
                       12, 14, 13, 12, 15, 14, 16, 18, 17, 16, 19, 18, 20, 21, 22, 20, 22, 23};

    indices.resize(36);

    for (unsigned int i = 0; i < 36; ++i)
    {
        indices[i] = u[i];
    }

    // Create vertices
    LSMVector4 clr(255, 255, 255, 255);


    vertices.push_back(Vertex(LSMVector3(0, 0, 0),LSMVector3(1.0,1.0,1.0),LSMVector3(0, 0, -1),LSMVector2(0.75f, 0.666f)));
    vertices.push_back(Vertex(LSMVector3(0, 1, 0),LSMVector3(1.0,1.0,1.0),LSMVector3(0, 0, -1),LSMVector2(0.75f, 0.333f)));
    vertices.push_back(Vertex(LSMVector3(1, 1, 0),LSMVector3(1.0,1.0,1.0),LSMVector3(0, 0, -1),LSMVector2(1.00f, 0.333f)));
    vertices.push_back(Vertex(LSMVector3(1, 0, 0),LSMVector3(1.0,1.0,1.0),LSMVector3(0, 0, -1),LSMVector2(1.00f, 0.666f)));

    vertices.push_back(Vertex(LSMVector3(0, 0, 1),LSMVector3(1.0,1.0,1.0),LSMVector3(0, 0, 1),LSMVector2(0.25f, 0.333f)));
    vertices.push_back(Vertex(LSMVector3(0, 1, 1),LSMVector3(1.0,1.0,1.0),LSMVector3(0, 0, 1),LSMVector2(0.25f, 0.666f)));
    vertices.push_back(Vertex(LSMVector3(1, 1, 1),LSMVector3(1.0,1.0,1.0),LSMVector3(0, 0, 1),LSMVector2(0.50f, 0.666f)));
    vertices.push_back(Vertex(LSMVector3(1, 0, 1),LSMVector3(1.0,1.0,1.0),LSMVector3(0, 0, 1),LSMVector2(0.50f, 0.333f)));

    vertices.push_back(Vertex(LSMVector3(0, 0, 0),LSMVector3(1.0,1.0,1.0),LSMVector3(-1, 0, 0),LSMVector2(0.00f, 0.333f)));
    vertices.push_back(Vertex(LSMVector3(0, 0, 1),LSMVector3(1.0,1.0,1.0),LSMVector3(-1, 0, 0),LSMVector2(0.25f, 0.333f)));
    vertices.push_back(Vertex(LSMVector3(0, 1, 1),LSMVector3(1.0,1.0,1.0),LSMVector3(-1, 0, 0),LSMVector2(0.25f, 0.666f)));
    vertices.push_back(Vertex(LSMVector3(0, 1, 0),LSMVector3(1.0,1.0,1.0),LSMVector3(-1, 0, 0),LSMVector2(0.00f, 0.666f)));

    vertices.push_back(Vertex(LSMVector3(1, 0, 0),LSMVector3(1.0,1.0,1.0),LSMVector3(1, 0, 0),LSMVector2(0.50f, 0.666f)));
    vertices.push_back(Vertex(LSMVector3(1, 0, 1),LSMVector3(1.0,1.0,1.0),LSMVector3(1, 0, 0),LSMVector2(0.75f, 0.666f)));
    vertices.push_back(Vertex(LSMVector3(1, 1, 1),LSMVector3(1.0,1.0,1.0),LSMVector3(1, 0, 0),LSMVector2(0.75f, 0.333f)));
    vertices.push_back(Vertex(LSMVector3(1, 1, 0),LSMVector3(1.0,1.0,1.0),LSMVector3(1, 0, 0),LSMVector2(0.50f, 0.333f)));

    vertices.push_back(Vertex(LSMVector3(0, 0, 0),LSMVector3(1.0,1.0,1.0),LSMVector3(0, -1, 0),LSMVector2(0.25f, 0.000f)));
    vertices.push_back(Vertex(LSMVector3(0, 0, 1),LSMVector3(1.0,1.0,1.0),LSMVector3(0, -1, 0),LSMVector2(0.25f, 0.333f)));
    vertices.push_back(Vertex(LSMVector3(1, 0, 1),LSMVector3(1.0,1.0,1.0),LSMVector3(0, -1, 0),LSMVector2(0.50f, 0.333f)));
    vertices.push_back(Vertex(LSMVector3(1, 0, 0),LSMVector3(1.0,1.0,1.0),LSMVector3(0, -1, 0),LSMVector2(0.50f, 0.000f)));

    vertices.push_back(Vertex(LSMVector3(0, 1, 0),LSMVector3(1.0,1.0,1.0),LSMVector3(0, 1, 0),LSMVector2(0.25f, 1.000f)));
    vertices.push_back(Vertex(LSMVector3(0, 1, 1),LSMVector3(1.0,1.0,1.0),LSMVector3(0, 1, 0),LSMVector2(0.25f, 0.666f)));
    vertices.push_back(Vertex(LSMVector3(1, 1, 1),LSMVector3(1.0,1.0,1.0),LSMVector3(0, 1, 0),LSMVector2(0.50f, 0.666f)));
    vertices.push_back(Vertex(LSMVector3(1, 1, 0),LSMVector3(1.0,1.0,1.0),LSMVector3(0, 1, 0),LSMVector2(0.50f, 1.000f)));

    for (unsigned int i = 0; i < 24; ++i) {
        vertices[i].setPosition(LSMVector3(vertices[i].position()) - LSMVector3(0.5f, 0.5f, 0.5f));
        vertices[i].setPosition(LSMVector3(vertices[i].position().x * size.X(),vertices[i].position().y * size.Y(),vertices[i].position().z * size.Z()));
    }


    this->SetIsConvex(true);
}


 void LSMMesh::SetAsCone(float radius_top, float radius_low, float length, unsigned int tesselation)
{
    //code from Project Chrono

    f32 oblique = 0;
    LSMVector4 color(255, 255, 255, 255);

    const f32 recTesselation = 1.0f/((f32)tesselation);
    const f32 recTesselationHalf = recTesselation * 0.5f;
    const f32 angleStep = (LISEM_PI* 2.f) * recTesselation;
    const f32 angleStepHalf = angleStep * 0.5f;

    // HULL

    unsigned int i;
    Vertex v;
    v.setColor(color.xyz());
    f32 tcx = 0.f;

    auto beta = atan2f(radius_low-radius_top, length*2);

    for (i = 0; i <= tesselation; ++i) {
        const f32 angle = angleStep * (float)(i);
        v.setPosition(LSMVector3(radius_low * cosf(angle),-length,radius_low * sinf(angle)));
        v.setNormal(LSMVector3(cosf(angle)*cosf(beta),  sinf(beta), sinf(angle)*cosf(beta)));
        v.setUV(LSMVector2(tcx,0.f));
        vertices.push_back(v);

        v.setPosition(LSMVector3(radius_top * cosf(angle),length,radius_top * sinf(angle)));
        v.setUV(LSMVector2(v.uv().x,1.f));

        vertices.push_back(v);

        tcx += recTesselation;
    }

    // indices for the main hull part
    for (i = 0; i < tesselation * 2; i += 2) {
        indices.push_back(i + 2);
        indices.push_back(i + 0);
        indices.push_back(i + 1);

        indices.push_back(i + 2);
        indices.push_back(i + 1);
        indices.push_back(i + 3);
    }

    // BOTTOM

    unsigned int index_bottom = vertices.size();

    for (i = 0; i <= tesselation; ++i) {
        const f32 angle = angleStep * (float)(i);
        v.setPosition(LSMVector3(radius_low * cosf(angle),-length,radius_low * sinf(angle)));
        v.setNormal(LSMVector3(0, -1, 0));
        v.setUV(LSMVector2(0.5f + 0.5f * cosf(angle),0.5f + 0.5f * sinf(angle)));

        vertices.push_back(v);
    }

    v.setPosition(LSMVector3(0.f,-length,0.f));
    v.setNormal(LSMVector3(0, -1.f, 0));
    v.setUV(LSMVector2(0.5,0.5));

    vertices.push_back(v);

    unsigned int index_center = vertices.size() - 1;

    for (i = 0; i < tesselation; ++i) {
        indices.push_back(index_center);
        indices.push_back(index_bottom + i + 0);
        indices.push_back(index_bottom + i + 1);
    }

    // TOP
    if (radius_top) {

        unsigned int index_top = vertices.size();

        for (i = 0; i <= tesselation; ++i) {
            const f32 angle = angleStep * (float)(i);

            v.setPosition(LSMVector3(radius_top * cosf(angle),length,radius_top * sinf(angle)));
            v.setNormal(LSMVector3(0, 1.f, 0));
            v.setUV(LSMVector2(0.5f + 0.5f * cosf(angle),0.5f + 0.5f * sinf(angle)));

            vertices.push_back(v);
        }

        v.setPosition(LSMVector3(0.f,length,0.f));
        v.setNormal(LSMVector3(0, 1.f, 0));
        v.setUV(LSMVector2(0.5,0.5));

        vertices.push_back(v);

        index_center = vertices.size() - 1;

        for (i = 0; i < tesselation; ++i) {
            indices.push_back(index_center);
            indices.push_back(index_top + i + 1);
            indices.push_back(index_top + i + 0);
        }
    }

    this->SetIsConvex(true);
}

 void LSMMesh:: Rotate(LSMVector3 axis, double angle)
{
    LSMMatrix4x4  objrotm;
    objrotm.SetRotation(axis,angle);

    for(int i = 0; i < vertices.size(); i++)
    {
        vertices.at(i).setPosition((objrotm * vertices.at(i).position()));
    }

}


 bool LSMMesh::IsSetup()
{
    return m_IsSetup;
}

 int LSMMesh::GetNumInstances()
{
    return m_NumInstances;
}


//check for convexity by determining all face planes, and checking if all other points are behind this plane
bool LSMMesh::int_Calc_IsConvex(float threshold)
{
    int vertexCount = vertices.size();
    LSMVector3 *norms1 = new LSMVector3[vertexCount];

    int triangleCount = indices.size()/3;


    for (unsigned long i = 0; i <  triangleCount; i++)
    {

        LSMVector3 Atmp = vertices[indices[i * 3 + 0]].position();
        LSMVector3 Btmp = vertices[indices[i * 3 + 1]].position();
        LSMVector3 Ctmp = vertices[indices[i * 3 + 2]].position();

        LSMVector3 A(Atmp.x, Atmp.y, Atmp.z);
        LSMVector3 B(Btmp.x, Btmp.y, Btmp.z);
        LSMVector3 C(Ctmp.x, Ctmp.y, Ctmp.z);
        B = B -A;
        C = C -A;

        LSMVector3 BCNorm = LSMVector3::CrossProduct(B,C).Normalize();

        float checkPoint = LSMVector3(vertices[0].position().x - A.x, vertices[0].position().y - A.y, vertices[0].position().z - A.z).dot(BCNorm);
        for (unsigned long j = 0; j < vertexCount; j++)
        {

            float dist = LSMVector3(vertices[j].position().x - A.x, vertices[j].position().y - A.y, vertices[j].position().z - A.z).dot(BCNorm);

            if(std::abs(checkPoint) > threshold & std::abs(dist) > threshold & checkPoint * dist < 0)
            {
                return false;
            }
        }
    }

    return true;
}

double LSMMesh::CalculateSurface()
{

    double surface = 0.0;
    int triangleCount = indices.size()/3;

    for (int i = 0; i < triangleCount; i++) {
        // Get vertices of triangle i.
        LSMVector3 v0 = this->vertices[indices[i*3 + 0]].position();
        LSMVector3 v1 = this->vertices[indices[i*3 + 1]].position();
        LSMVector3 v2 = this->vertices[indices[i*3 + 2]].position();

        double a = (v0-v1).length();
        double b = (v2-v1).length();
        double c = (v2-v0).length();

        double s = 0.5 * (a+b+c);

        surface += std::sqrt(std::max(0.01,s*(s-a)*(s-b)*(s-c)));
    }

    return surface;

}


double LSMMesh::CalculateProjectedSurface(LSMVector3 shape_loc,LSMVector2 Velocity,BoundingBox extent, float h, float dhdx, float dhdy, float z, float dzdx, float dzdy, bool do_concave_detailed)
{
    //two axis for projection
    //first is vertical
    LSMVector3 axis1 = LSMVector3(0.0,1.0,0.0).Normalize();
    //second is parallel to vertical and flow direction
    LSMVector3 axis2 = LSMVector3::CrossProduct(axis1,LSMVector3(Velocity.x,0.0,Velocity.y)).Normalize();
    //velocity axis is perpendicular to projection plane
    LSMVector3 axisv = LSMVector3(Velocity.x,0.0,Velocity.y).Normalize();

    double surface = 0.0;
    int triangleCount = indices.size()/3;

    for (int i = 0; i < triangleCount; i++) {
        // Get vertices of triangle i.
        LSMVector3 v00 = this->vertices[indices[i*3 + 0]].position() + shape_loc;
        LSMVector3 v10 = this->vertices[indices[i*3 + 1]].position() + shape_loc;
        LSMVector3 v20 = this->vertices[indices[i*3 + 2]].position() + shape_loc;


        //x and z coordinates
        v00 = extent.LimitWithin(v00);
        v10 = extent.LimitWithin(v10);
        v20 = extent.LimitWithin(v20);

        //get flow surface elevations
        float h0 = std::max(0.0f,h + v00.x * dhdx + v00.y *dhdy);
        float h1 = std::max(0.0f,h + v10.x * dhdx + v10.y *dhdy);
        float h2 = std::max(0.0f,h + v20.x * dhdx + v20.y *dhdy);

        //get dem surface elevations
        float z0 = std::max(0.0f,z + v00.x * dzdx + v00.y *dzdy);
        float z1 = std::max(0.0f,z + v10.x * dzdx + v10.y *dzdy);
        float z2 = std::max(0.0f,z + v20.x * dzdx + v20.y *dzdy);


        //fix y coordinates
        v00.y = std::max(z0,std::min(z0 + h0,v00.y));
        v10.y = std::max(z1,std::min(z1 + h1,v10.y));
        v20.y = std::max(z2,std::min(z2 + h2,v20.y));

        LSMVector3 v0 = v00.ProjectedOnTwoAxes(axis1,axis2);
        LSMVector3 v1 = v10.ProjectedOnTwoAxes(axis1,axis2);
        LSMVector3 v2 = v10.ProjectedOnTwoAxes(axis1,axis2);

        //project along axis
        double a = (v0-v1).length();
        double b = (v2-v1).length();
        double c = (v2-v0).length();

        double s = 0.5 * (a+b+c);

        surface += std::sqrt(std::max(0.01,s*(s-a)*(s-b)*(s-c)));
    }

    return surface;

}

//get volume and inertia properties
// Following function is a modified version of:
//
// Geometric Tools, LLC
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)
//
// void ComputeMassProperties (const Vector3<Real>* vertices, int numTriangles,
//                             const int* indices, bool bodyCoords, Real& mass,
//                             Vector3<Real>& center, Matrix3<Real>& inertia)
//
double LSMMesh::CalculateVolume(double density, LSMVector3 *center) {
    const double oneDiv6 = (double)(1.0 / 6.0);
    const double oneDiv24 = (double)(1.0 / 24.0);
    const double oneDiv60 = (double)(1.0 / 60.0);
    const double oneDiv120 = (double)(1.0 / 120.0);

    double mass = 1.0f;

    // order:  1, x, y, z, x^2, y^2, z^2, xy, yz, zx
    double integral[10] = {(double)0.0, (double)0.0, (double)0.0, (double)0.0, (double)0.0,
                           (double)0.0, (double)0.0, (double)0.0, (double)0.0, (double)0.0};

    int triangleCount = indices.size()/3;

    for (int i = 0; i < triangleCount; i++) {
        // Get vertices of triangle i.
        LSMVector3 v0 = this->vertices[indices[i*3 + 0]].position();
        LSMVector3 v1 = this->vertices[indices[i*3 + 1]].position();
        LSMVector3 v2 = this->vertices[indices[i*3 + 2]].position();

        // Get cross product of edges and normal vector.
        LSMVector3 V1mV0 = v1 - v0;
        LSMVector3 V2mV0 = v2 - v0;
        LSMVector3 N = LSMVector3::CrossProduct(V1mV0, V2mV0);

        // Compute integral terms.
        double tmp0, tmp1, tmp2;
        double f1x, f2x, f3x, g0x, g1x, g2x;
        tmp0 = v0.x + v1.x;
        f1x = tmp0 + v2.x;
        tmp1 = v0.x * v0.x;
        tmp2 = tmp1 + v1.x * tmp0;
        f2x = tmp2 + v2.x * f1x;
        f3x = v0.x * tmp1 + v1.x * tmp2 + v2.x * f2x;
        g0x = f2x + v0.x * (f1x + v0.x);
        g1x = f2x + v1.x * (f1x + v1.x);
        g2x = f2x + v2.x * (f1x + v2.x);

        double f1y, f2y, f3y, g0y, g1y, g2y;
        tmp0 = v0.y + v1.y;
        f1y = tmp0 + v2.y;
        tmp1 = v0.y * v0.y;
        tmp2 = tmp1 + v1.y * tmp0;
        f2y = tmp2 + v2.y * f1y;
        f3y = v0.y * tmp1 + v1.y * tmp2 + v2.y * f2y;
        g0y = f2y + v0.y * (f1y + v0.y);
        g1y = f2y + v1.y * (f1y + v1.y);
        g2y = f2y + v2.y * (f1y + v2.y);

        double f1z, f2z, f3z, g0z, g1z, g2z;
        tmp0 = v0.z + v1.z;
        f1z = tmp0 + v2.z;
        tmp1 = v0.z * v0.z;
        tmp2 = tmp1 + v1.z * tmp0;
        f2z = tmp2 + v2.z * f1z;
        f3z = v0.z * tmp1 + v1.z * tmp2 + v2.z * f2z;
        g0z = f2z + v0.z * (f1z + v0.z);
        g1z = f2z + v1.z * (f1z + v1.z);
        g2z = f2z + v2.z * (f1z + v2.z);

        // Update integrals.
        integral[0] += N.x * f1x;
        integral[1] += N.x * f2x;
        integral[2] += N.y * f2y;
        integral[3] += N.z * f2z;
        integral[4] += N.x * f3x;
        integral[5] += N.y * f3y;
        integral[6] += N.z * f3z;
        integral[7] += N.x * (v0.y * g0x + v1.y * g1x + v2.y * g2x);
        integral[8] += N.y * (v0.z * g0y + v1.z * g1y + v2.z * g2y);
        integral[9] += N.z * (v0.x * g0z + v1.x * g1z + v2.x * g2z);
    }

    integral[0] *= oneDiv6;
    integral[1] *= oneDiv24;
    integral[2] *= oneDiv24;
    integral[3] *= oneDiv24;
    integral[4] *= oneDiv60;
    integral[5] *= oneDiv60;
    integral[6] *= oneDiv60;
    integral[7] *= oneDiv120;
    integral[8] *= oneDiv120;
    integral[9] *= oneDiv120;

    // mass
    mass = integral[0];

    // center of mass
    if(center != nullptr)
    {
        *center = LSMVector3(integral[1], integral[2], integral[3]) / mass;
    }

    // inertia relative to world origin
    /*inertia(0, 0) = integral[5] + integral[6];
    inertia(0, 1) = -integral[7];
    inertia(0, 2) = -integral[9];
    inertia(1, 0) = inertia(0, 1);
    inertia(1, 1) = integral[4] + integral[6];
    inertia(1, 2) = -integral[8];
    inertia(2, 0) = inertia(0, 2);
    inertia(2, 1) = inertia(1, 2);
    inertia(2, 2) = integral[4] + integral[5];*/

    // inertia relative to center of mass
    /*if (bodyCoords) {
        inertia(0, 0) -= mass * (center.y * center.y + center.z * center.z);
        inertia(0, 1) += mass * center.x * center.y;
        inertia(0, 2) += mass * center.z * center.x;
        inertia(1, 0) = inertia(0, 1);
        inertia(1, 1) -= mass * (center.z * center.z + center.x * center.x);
        inertia(1, 2) += mass * center.y * center.z;
        inertia(2, 0) = inertia(0, 2);
        inertia(2, 1) = inertia(1, 2);
        inertia(2, 2) -= mass * (center.x * center.x + center.y * center.y);
    }*/

    return mass * density;
}

double LSMMesh::CalculateVolumeWithinSurfaceFlowBox(LSMVector3 shape_loc, BoundingBox extent, float h, float dhdx, float dhdy, float z, float dzdx, float dzdy, LSMVector3 * center)
{
    const double oneDiv6 = (double)(1.0 / 6.0);
    const double oneDiv24 = (double)(1.0 / 24.0);
    const double oneDiv60 = (double)(1.0 / 60.0);
    const double oneDiv120 = (double)(1.0 / 120.0);

    double mass = 1.0f;

    // order:  1, x, y, z, x^2, y^2, z^2, xy, yz, zx
    double integral[10] = {(double)0.0, (double)0.0, (double)0.0, (double)0.0, (double)0.0,
                           (double)0.0, (double)0.0, (double)0.0, (double)0.0, (double)0.0};

    int triangleCount = indices.size()/3;

    for (int i = 0; i < triangleCount; i++) {
        // Get vertices of triangle i.
        LSMVector3 v0 = this->vertices[indices[i*3 + 0]].position() + shape_loc;
        LSMVector3 v1 = this->vertices[indices[i*3 + 1]].position() + shape_loc;
        LSMVector3 v2 = this->vertices[indices[i*3 + 2]].position() + shape_loc;

        //manipulate positions to be within the surface flow box at least

        //x and z coordinates
        v0 = extent.LimitWithin(v0);
        v1 = extent.LimitWithin(v1);
        v2 = extent.LimitWithin(v2);

        //get flow surface elevations
        float h0 = std::max(0.0f,h + v0.x * dhdx + v0.y *dhdy);
        float h1 = std::max(0.0f,h + v1.x * dhdx + v1.y *dhdy);
        float h2 = std::max(0.0f,h + v2.x * dhdx + v2.y *dhdy);

        //get dem surface elevations
        float z0 = std::max(0.0f,z + v0.x * dzdx + v0.y *dzdy);
        float z1 = std::max(0.0f,z + v1.x * dzdx + v1.y *dzdy);
        float z2 = std::max(0.0f,z + v2.x * dzdx + v2.y *dzdy);


        //fix y coordinates
        v0.y = std::max(z0,std::min(z0 + h0,v0.y));
        v1.y = std::max(z1,std::min(z1 + h1,v1.y));
        v2.y = std::max(z2,std::min(z2 + h2,v2.y));

        // Get cross product of edges and normal vector.
        LSMVector3 V1mV0 = v1 - v0;
        LSMVector3 V2mV0 = v2 - v0;
        LSMVector3 N = LSMVector3::CrossProduct(V1mV0, V2mV0);

        // Compute integral terms.
        double tmp0, tmp1, tmp2;
        double f1x, f2x, f3x, g0x, g1x, g2x;
        tmp0 = v0.x + v1.x;
        f1x = tmp0 + v2.x;
        tmp1 = v0.x * v0.x;
        tmp2 = tmp1 + v1.x * tmp0;
        f2x = tmp2 + v2.x * f1x;
        f3x = v0.x * tmp1 + v1.x * tmp2 + v2.x * f2x;
        g0x = f2x + v0.x * (f1x + v0.x);
        g1x = f2x + v1.x * (f1x + v1.x);
        g2x = f2x + v2.x * (f1x + v2.x);

        double f1y, f2y, f3y, g0y, g1y, g2y;
        tmp0 = v0.y + v1.y;
        f1y = tmp0 + v2.y;
        tmp1 = v0.y * v0.y;
        tmp2 = tmp1 + v1.y * tmp0;
        f2y = tmp2 + v2.y * f1y;
        f3y = v0.y * tmp1 + v1.y * tmp2 + v2.y * f2y;
        g0y = f2y + v0.y * (f1y + v0.y);
        g1y = f2y + v1.y * (f1y + v1.y);
        g2y = f2y + v2.y * (f1y + v2.y);

        double f1z, f2z, f3z, g0z, g1z, g2z;
        tmp0 = v0.z + v1.z;
        f1z = tmp0 + v2.z;
        tmp1 = v0.z * v0.z;
        tmp2 = tmp1 + v1.z * tmp0;
        f2z = tmp2 + v2.z * f1z;
        f3z = v0.z * tmp1 + v1.z * tmp2 + v2.z * f2z;
        g0z = f2z + v0.z * (f1z + v0.z);
        g1z = f2z + v1.z * (f1z + v1.z);
        g2z = f2z + v2.z * (f1z + v2.z);

        // Update integrals.
        integral[0] += N.x * f1x;
        integral[1] += N.x * f2x;
        integral[2] += N.y * f2y;
        integral[3] += N.z * f2z;
        integral[4] += N.x * f3x;
        integral[5] += N.y * f3y;
        integral[6] += N.z * f3z;
        integral[7] += N.x * (v0.y * g0x + v1.y * g1x + v2.y * g2x);
        integral[8] += N.y * (v0.z * g0y + v1.z * g1y + v2.z * g2y);
        integral[9] += N.z * (v0.x * g0z + v1.x * g1z + v2.x * g2z);
    }

    integral[0] *= oneDiv6;
    integral[1] *= oneDiv24;
    integral[2] *= oneDiv24;
    integral[3] *= oneDiv24;
    integral[4] *= oneDiv60;
    integral[5] *= oneDiv60;
    integral[6] *= oneDiv60;
    integral[7] *= oneDiv120;
    integral[8] *= oneDiv120;
    integral[9] *= oneDiv120;

    // mass
    mass = integral[0];

    // center of mass
    if(center != nullptr)
    {
        *center = LSMVector3(integral[1], integral[2], integral[3]) / mass;
    }

    return mass ;
}

