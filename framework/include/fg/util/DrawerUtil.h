
#pragma once

#include <Ogre.h>
#include <OgreColourValue.h>

namespace fog
{
    using namespace Ogre;
    class DrawerUtil
    {
    public:
        static void drawHexagonTo(Ogre::ManualObject *obj,
                                  const std::vector<Ogre::Vector3> &vertices,
                                  const Ogre::ColourValue &color1)
        {
            drawHexagonTo(obj, vertices, color1, color1);
        }

        static void drawHexagonTo(Ogre::ManualObject *obj,
                                  const std::vector<Ogre::Vector3> &vertices,
                                  const Ogre::ColourValue &color1, ColourValue color2)
        {
            const float nomX = 0;
            const float nomY = 1;
            const float nomZ = 0;
            // Compute center
            Ogre::Vector3 center(0, 0, 0);
            for (auto &v : vertices)
                center += v;
            center *= (1.0f / 6.0f);

            size_t baseIndex = obj->getCurrentVertexCount();

            // Center
            obj->position(center.x, center.y, center.z);
            obj->normal(nomX, nomY, nomZ);
            obj->colour(color1);

            // Corners
            for (int i = 0; i < 6; ++i)
            {
                obj->position(vertices[i].x, vertices[i].y, vertices[i].z);
                obj->normal(nomX, nomY, nomZ);
                obj->colour(color2);
            }

            // Triangles
            for (int i = 0; i < 6; ++i)
            {
                int p1 = baseIndex + i + 1;
                int p2 = baseIndex + (i + 1) % 6 + 1;
                obj->triangle(baseIndex, p1, p2);
            }
        } //
    }; // end of class

    class TerrainedDrawerUtil
    {
    public:
        static void drawHexagonTo(Ogre::ManualObject *obj,
                                  const TerrainedVertices3 &vertices,
                                  const Ogre::ColourValue &color1)
        {
            const float nomX = 0;
            const float nomY = 1;
            const float nomZ = 0;

            int rows = vertices.size();

            for (int i = 0; i < rows; i++)
            {
                const std::vector<Vector3> &row = vertices[i];
                int cols = row.size();
                for (int j = 0; j < cols; j++)
                {
                    Vector3 pos = row[j];
                    obj->position(pos.x, pos.y, pos.z);
                    obj->normal(nomX, nomY, nomZ);
                    obj->colour(color1);
                }
            }

            int idx = 0;

            for (int i = 0; i < rows - 1; i++)
            {
                const std::vector<Vector3> &row1 = vertices[i];
                int size = row1.size();

                const std::vector<Vector3> &row2 = vertices[i + 1];
                int size2 = row2.size();

                for (int j = 0; j < size; j++)
                {

                    //         *   .   .   .   .   .   .
                    //         | \ | \ | \ | \ | \ | \ |
                    //         .   .   .   .   .   .   .
                    if (j < size2 - 1)
                    {
                        obj->triangle(idx, idx + size, idx + size + 1);
                    }

                    //         . __* __ .__. __. __ .__ .
                    //           \ | \ | \ | \ | \ | \ |
                    //         .   .   .   .   .   .   .
                    if (j > 0 && j < size2)
                    {
                        obj->triangle(idx, idx - 1, idx + size);
                    }

                    //         . __. __ .__. __. __ .__ . __ *__ . __ .
                    //           \ | \ | \ | \ | \ | \ |    /   /    /
                    //         .   .   .   .   .   .   .
                    if (j > 0 && j > size2 - 1)
                    {
                        obj->triangle(idx, idx - 1, idx - j + size + size2 - 1);
                    }

                    idx++; // total index.
                }
            }

        } //

    }; // end of class
}; // end of namespace