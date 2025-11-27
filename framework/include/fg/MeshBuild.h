#pragma once
#include <Ogre.h>
#include <OgreVector.h>
#include "fg/util/CellMark.h"
#include "fg/Plane.h"
#include "fg/util/CostMap.h"
#include <OgreManualObject.h>
#include <OgreColourValue.h>
#include "fg/Terrains.h"
#include "Cell.h"
#define FG_REPORT_ERROR_INDEX_OUT_OF_RANGE 0
#define FG_SPIDER_TOTAL_LAYER 3
#define FG_SPIDER_TOP_LAYER 3
#define FG_SPIDER_BOTTOM_LAYER 0

using namespace Ogre;

namespace fog
{

    class MeshBuild
    {

    public:
        class PointOnCircle
        {
        public:
            ManualObject *obj;
            int baseIndex;
            PointOnCircle(ManualObject *obj) : obj(obj) {}
            void begin(std::string material)
            {
                obj->clear();
                obj->begin(material, Ogre::RenderOperation::OT_TRIANGLE_LIST);
                baseIndex = obj->getCurrentVertexCount();
            }

            void operator()(Cell::Instance &cell, ColourValue color)
            {
                Vector2 origin = cell.getOrigin2D();
                Vector3 nom3(0, 1, 0);

                struct Visit
                {
                    ManualObject *obj;
                    ColourValue color;
                    Cell::Instance &cell;
                    Vector2 origin;
                    Vector3 nom3;
                    int layer;
                    void operator()(int pIdx, Vector2 &pointOnCircle)
                    {
                        Vector3 pos = cell.node->to3D(origin, pointOnCircle, &nom3);

                        obj->position(pos);
                        obj->normal(nom3);
                        obj->colour(color);

                        obj->position(pos.x, pos.y, pos.z + 10.0f); // density
                        obj->normal(nom3);
                        obj->colour(color);

                        obj->position(pos.x + 10.0f, pos.y, pos.z);
                        obj->normal(nom3);
                        obj->colour(color);
                    }
                } visit{
                    obj,
                    color,
                    cell,
                    origin,
                    nom3,
                };

                int LAYERS = 1;
                for (int i = 0; i < LAYERS; i++)
                {
                    visit.layer = i + 1;
                    float sizeI = std::powf(2, i) * 6;
                    Cell::forEachPointOnCircle(sizeI, 0.0f, visit);
                }
            }

            void end()
            {
                this->obj->end();
            }
        };

        struct Vertex
        {
            Vector3 position;
            ColourValue colour;
            Vector3 normal;
            bool isNormalInit = false;
            void addNormal(Vector3 norm)
            {
                if (!isNormalInit)
                {
                    this->normal = norm;
                    this->isNormalInit = true;
                    return;
                }
                this->normal += norm;
                this->normal.normalise();
            }
        };

        struct TriangleIndexData
        {
            int v1;
            int v2;
            int v3;
        };

        class AutoNormManualObject
        {
        public:
            ManualObject *obj;
            int baseIndex;
            std::vector<Vertex> vertices;

            std::vector<TriangleIndexData> triangles;

            void begin(ManualObject *obj)
            {
                this->obj = obj;
                this->resetBaseIndex();
            }
            void resetBaseIndex()
            {
                this->baseIndex = obj->getCurrentVertexCount();
            }

            void position(Vector3 pos)
            {
                Vertex v;
                v.position = pos;
                vertices.push_back(v);
            }

            void colour(ColourValue color)
            {
                int size1 = vertices.size();
                vertices[size1 - 1].colour = color;
            }

            void triangle(int v1, int v2, int v3)
            {
                int idx1 = v1 - baseIndex;
                int idx2 = v2 - baseIndex;
                int idx3 = v3 - baseIndex;
                //
                int size1 = vertices.size();
#if FG_REPORT_ERROR_INDEX_OUT_OF_RANGE
                if (idx1 < 0 || idx1 > size1 - 1)
                {
                    //
                    throw std::runtime_error("vertex index is out of range, please make sure the vertex is added. and make sure the data is not flushed already.");
                }
                if (idx2 < 0 || idx2 > size1 - 1)
                {
                    //
                    throw std::runtime_error("not supported feature.");
                }
                if (idx3 < 0 || idx3 > size1 - 1)
                {
                    //
                    throw std::runtime_error("not supported feature.");
                }
#endif

                TriangleIndexData tid = {idx1, idx2, idx3};

                triangles.push_back(tid);
            }
            void commit()
            {

                // calculate normal
                int size2 = triangles.size();

                for (int i = 0; i < size2; i++)
                {
                    Vertex &v1 = vertices[triangles[i].v1];
                    Vertex &v2 = vertices[triangles[i].v2];
                    Vertex &v3 = vertices[triangles[i].v3];

                    Vector3 p1 = v1.position;
                    Vector3 p2 = v2.position;
                    Vector3 p3 = v3.position;

                    Vector4 plane = Math::calculateFaceNormalWithoutNormalize(p1, p2, p3);
                    Vector3 norm(plane.x, plane.y, plane.z);
                    norm.normalise();
                    v1.addNormal(norm);
                    v2.addNormal(norm);
                    v3.addNormal(norm);
                }

                int size1 = vertices.size();
                // flush data
                for (int i = 0; i < size1; i++)
                {
                    obj->position(vertices[i].position);

                    if (vertices[i].isNormalInit)
                    {
                        obj->normal(vertices[i].normal);
                    }

                    obj->colour(vertices[i].colour);
                }

                // flush index
                for (int i = 0; i < size2; i++)
                {
                    obj->triangle(triangles[i].v1 + baseIndex, triangles[i].v2 + baseIndex, triangles[i].v3 + baseIndex);
                }
                triangles.clear();
                vertices.clear();
                resetBaseIndex();
            }
        };

        class SpiderNet
        {
        public:
            struct PointVisit
            {
                AutoNormManualObject *obj;
                ColourValue color;
                // Cell::Instance *cell;
                // Vector2 origin;

                int layer;
                int layerSize;
                int preLayerSize;
                int totalLayer = FG_SPIDER_TOTAL_LAYER; // settings global
                int topLayer = FG_SPIDER_TOP_LAYER;
                int botLayer = FG_SPIDER_BOTTOM_LAYER;
                // to build the mesh, this context alive on the whole building operation.
                // so it visits each cell and each points of cells.
                int idx; // point index

                template <typename F>
                void operator()(int pIdx, Vector2 &pointOnCircle, F &&positionFunc)
                {
                    Vector2 pointOnLayer = pointOnCircle * ((float)layer / (float)totalLayer);
                    // Vector3 pos = cell->node->to3D(origin, pointOnLayer, nullptr);
                    Vector3 pos = positionFunc(pointOnLayer);
                    obj->position(pos);
                    obj->colour(color);

                    //
                    int size1 = preLayerSize;
                    int size2 = layerSize;
                    int i = layer;
                    int j = pIdx; //
                    // skip i==0

                    if (i > botLayer) //
                    {
                        // (j-1-size1)        .       .     .   .   .   .
                        //         |   \     | \      | \   | \ | \ | \ |
                        //      (j-1)__(j)   .___*    __    .   .   .
                        //        0     1    2   3    4  5

                        if (j % 2 == 1) // 1,3,5
                        {
                            obj->triangle(idx, idx - 1 - size1 - j / 2, idx - 1);
                        }
                        //
                        if (j % 2 == 0 && j > 1) // 2,4,6
                        {
                            obj->triangle(idx, idx - size1 - j / 2, idx - 1);
                            obj->triangle(idx - 1, idx - size1 - j / 2, idx - size1 - j / 2 - 1);
                        }
                        if (j == size2 - 1) // 0=idx-j
                        {                   // last one, then find the first one.
                            int j0 = idx - j;
                            obj->triangle(idx - j, (idx - j) - size1, idx);
                            obj->triangle(idx, (idx - j) - size1, idx - j - 1);
                        }
                    }
                    idx++;
                    //
                }
            }; // end of Point Visit

            ManualObject *obj;
            int baseIndex;
            PointVisit visitPoint;
            bool useDefaultNorm = true;
            Vector3 defaultNorm = Vector3::UNIT_Y;
            AutoNormManualObject objProxy;

            SpiderNet(ManualObject *obj) : obj(obj) {}
            void begin(std::string material)
            {
                obj->clear();
                obj->begin(material, Ogre::RenderOperation::OT_TRIANGLE_LIST);
                baseIndex = obj->getCurrentVertexCount();
                //
                visitPoint.obj = &objProxy;

                objProxy.begin(obj);

                visitPoint.idx = baseIndex;
            }

            void operator()(Cell::Instance &cell, ColourValue color)
            {
                operator()([&cell, this](Vector2 & pointOnLayer)
                           { return cell.node->to3D(cell.getOrigin2D(), pointOnLayer, useDefaultNorm ? &defaultNorm : nullptr); }, color);
            }
            // each cell visit op.
            template <typename F>
            void operator()(F &&positionFunc, ColourValue color)
            {
                // visitPoint.cell = &cell;
                // visitPoint.origin = cell.getOrigin2D();
                visitPoint.color = color;
                visitPoint.layerSize = 0;
                //
                for (int i = visitPoint.botLayer; i < visitPoint.topLayer + 1; i++)
                {
                    visitPoint.layer = i;
                    visitPoint.preLayerSize = visitPoint.layerSize;
                    visitPoint.layerSize = layerSize(i);

                    Cell::forEachPointOnCircle(visitPoint.layerSize, 0.0f, visitPoint, positionFunc);
                }
                objProxy.commit();
            }

            int layerSize(int layer)
            {
                return layer = std::powf(2, layer) * 6;
            }

            void end()
            {
                this->obj->end();
            }
        }; // end of spider net.

        class HexPrism
        {
            ManualObject *obj;
            int baseIndex;

        public:
            HexPrism(ManualObject *obj) : obj(obj) {}
            void begin(std::string material)
            {
                obj->clear();
                obj->begin(material, Ogre::RenderOperation::OT_TRIANGLE_LIST);
                baseIndex = obj->getCurrentVertexCount();
            }

            void operator()(ColourValue color)
            {

            }
            void end()
            {
                this->obj->end();
            }
        };
    };
};