#pragma once
#include "fg/defines.h"
#include "fg/State.h"
#include "fg/CoreMod.h"
#include "fg/MaterialNames.h"
#include "fg/MeshBuild.h"

namespace fog
{

    class CellInstanceState : public State
    {
        std::stack<ColourValue> colours;
        ManualObject *obj;
        Cell::Instance cis;

    public:
        CellInstanceState(Cell::Instance cis) : cis(cis)
        {
            this->sceNode = Context<CoreMod>::get()->getSceneManager()->getRootSceneNode()->createChildSceneNode();
            this->obj = Context<CoreMod>::get()->getSceneManager()->createManualObject();
            this->sceNode->attachObject(this->obj);
        }

        virtual ~CellInstanceState()
        {
        }

        void init() override
        {
            ColourValue bottomColour;
            bool hasBottomColor = getCostColor(cis, bottomColour);
            if (hasBottomColor)
            {
                colours.push(bottomColour);
            }

            buildMesh();
        }

        Vector3 getOrigin3D()
        {
            return this->cis.getOrigin3D();
        }

        void buildMesh()
        {
            if (colours.empty())
            {
                obj->clear();
            }
            else
            {
                MeshBuild::SpiderNet buildMesh(obj);
                buildMesh.begin(MaterialNames::materialNameInUse);
                ColourValue currentColour = colours.top();
                buildMesh(cis, currentColour); //
                buildMesh.end();
            }
            // this->sceNode->setPosition(cis.getOrigin3D());
        }

        // Get color based on cost
        bool getCostColor(Cell::Instance &cell, Ogre::ColourValue &color) const
        {
            CostMap *costMap = Context<CostMap>::get();
            const int cost = costMap->getCost(cell.cKey.first, cell.cKey.second);
            switch (cost)
            {
            case CostMap::OBSTACLE:
                color = Ogre::ColourValue::Red;
                return true;
            case CostMap::DEFAULT_COST:
                color = Ogre::ColourValue(0.8f, 0.6f, 0.2f); // light Sand color
                return false;
            case 2:
                color = Ogre::ColourValue(0.6f, 0.4f, 0.1f); // Dark Sand color
                return true;
            case 3:
                color = Ogre::ColourValue(0.2f, 0.4f, 0.8f); // Water color
                return true;
            default:
                color = Ogre::ColourValue(0.7f, 0.7f, 0.7f); // light gray

                return true;
            }
        }

        CellKey getCellKey()
        {
            return this->cis.cKey;
        }

        void popColour()
        {
            // if (!this->colours.empty())
            //{
            this->colours.pop();
            //}
            buildMesh();
        }

        void pushColour(ColourValue color)
        {
            this->colours.push(color);
            buildMesh();
        }

    }; // end of class

    class CellInstanceManager : public State
    {
        std::unordered_map<CellKey, CellInstanceState *, PairHash> cellInstanceStates;

    public:
        CellInstanceManager()
        {
        }
        virtual ~CellInstanceManager()
        {
        }
        void init() override
        {
            this->sceNode = Context<CoreMod>::get()->getSceneManager()->getRootSceneNode()->createChildSceneNode();
            Context<Cell::Center>::get()-> //
                forEachCell([this](Cell::Instance &cell)
                            {
                                CellInstanceState *state = new CellInstanceState(cell);
                                state->init();
                                this->addChild(state); //
                                this->cellInstanceStates[cell.cKey] = state; });
        }

        CellInstanceState *getCellInstanceStateByPosition(Vector2 posIn2D)
        {
            Vector3 pos3D = Context<Node2D>::get()->to3D(posIn2D);
            return getCellInstanceStateByPosition(pos3D);
        }

        CellKey getCellKeyByPosition(Vector3 pos)
        {
            Cell::Instance cell;
            if (Context<Cell::Center>::get()->findCellByWorldPosition(pos, cell))
            {
                return cell.cKey;
            }
            return CellKey(-1, -1);
        }

        CellInstanceState *getCellInstanceStateByPosition(Vector3 pos)
        {
            Cell::Instance cell;
            if (Context<Cell::Center>::get()->findCellByWorldPosition(pos, cell))
            {
                return this->cellInstanceStates[cell.cKey];
            }
            return nullptr;
        }
        CellInstanceState *getCellInstanceStateByCellKey(CellKey cKey)
        {
            auto it = this->cellInstanceStates.find(cKey);
            if (it != this->cellInstanceStates.end())
            {
                return it->second;
            }
            return nullptr;
        }

    }; // end of class
}; // end of namespace