#pragme once

#include "boundingbox.h"
#include <list>
//sparse octree with dynamic bounding box
//and point replacement

//the octree stores integer pointers to indices
//these indices are attached to specific points
//the point knows which octree-item points to it.
//with this two-way link, we can do constant-time edits
//


class TreePointerRef
{
    void * item = nullptr;
    int index = -1; //-1 means invalid
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
};

class TreeNode
{
   public:

    double z_min;
    double z_max;
    double x_min;
    double x_max;
    double y_min;
    double y_max;

    TreeNode * Parent = nullptr;

    std::list<TreePointerRef *> items;

    //if any of these is not null, it means this node is subdivided, and should have no items of its own
    TreeNode * ULF = nullptr;
    TreeNode * ULB = nullptr;
    TreeNode * URF = nullptr;
    TreeNode * URB = nullptr;
    TreeNode * BLF = nullptr;
    TreeNode * BLB = nullptr;
    TreeNode * BRF = nullptr;
    TreeNode * BRB = nullptr;


    inline bool Contains_Coord(double x, double y, double z)
    {

        if(x >= x_min && x < x_max)
        {
            if(y >= y_min && y < y_max)
            {
                if(z >= z_min && z < z_max)
                {
                    return true;
                }
            }
        }
        return false;
    }

    inline void AddItemDirect(TreePointerRef * ref)
    {
        ref->item = this;
        this->items.push_back(ref);
    }
    inline TreePointerRef * AddItem(int i, double x, double y, double z)
    {
        TreePointerRef * ref = new TreePointerRef();
        ref->index = i;
        ref->x = x;
        ref->y = y;
        ref->z = z;
        ref->item = this;
        this->items.push_back(ref);
    }

    inline int ItemCount()
    {
        return items.size();
    }

    inline bool HasChildren()
    {
        return m_ULF != nullptr;
    }

    inline TreeNode * GetChild(int i)
    {
        if(i == 0)
        {
            return ULF;
        }
        if(i == 1)
        {
            return ULB;
        }
        if(i == 2)
        {
            return URF;
        }
        if(i == 3)
        {
            return URB;
        }
        if(i == 4)
        {
            return BLF;
        }
        if(i == 5)
        {
            return BLB;
        }
        if(i == 6)
        {
            return BRF;
        }
        if(i == 7)
        {
            return BRB;
        }

        return nullptr;

    }
    inline void CreateChildren()
    {
        if(m_ULF == nullptr)
        {
            double sizex = x_max-x_min;
            double sizey = y_max-y_min;
            double sizez = z_max-z_min;

            ULF = new TreeNode();
            ULF->x_min = x_min;
            ULF->x_max = x_min + 0.5 * sizex;
            ULF->y_min = y_min + 0.5 * sizey;
            ULF->y_max = y_max;
            ULF->z_min = z_min;
            ULF->z_max = z_min + 0.5 * sizez;
            ULB = new TreeNode();
            ULB->x_min = x_min;
            ULB->x_max = x_min + 0.5 * sizex;
            ULB->y_min = y_min + 0.5 * sizey;
            ULB->y_max = y_max;
            ULB->z_min = z_min + 0.5 * sizez;
            ULB->z_max = z_max;
            URF = new TreeNode();
            URF->x_min = x_min + 0.5 * sizex;
            URF->x_max = x_max;
            URF->y_min = y_min + 0.5 * sizey;
            URF->y_max = y_max;
            URF->z_min = z_min;
            URF->z_max = z_min + 0.5 * sizez;
            URB = new TreeNode();
            URB->x_min = x_min + 0.5 * sizex;
            URB->x_max = x_max;
            URB->y_min = y_min + 0.5 * sizey;
            URB->y_max = y_max;
            URB->z_min = z_min + 0.5 * sizez;
            URB->z_max = z_max;
            BLF = new TreeNode();
            BLF->x_min = x_min;
            BLF->x_max = x_min + 0.5 * sizex;
            BLF->y_min = y_min ;
            BLF->y_max = y_min + 0.5 * sizey;
            BLF->z_min = z_min;
            BLF->z_max = z_min + 0.5 * sizez;
            BLB = new TreeNode();
            BLB->x_min = x_min;
            BLB->x_max = x_min + 0.5 * sizex;
            BLB->y_min = y_min;
            BLB->y_max = y_min + 0.5 * sizey;
            BLB->z_min = z_min + 0.5 * sizez;
            BLB->z_max = z_max;
            BRF = new TreeNode();
            BRF->x_min = x_min + 0.5 * sizex;
            BRF->x_max = x_max;
            BRF->y_min = y_min;
            BRF->y_max = y_min + 0.5 * sizey;
            BRF->z_min = z_min;
            BRF->z_max = z_min + 0.5 * sizez;
            BRB = new TreeNode();
            BRB->x_min = x_min + 0.5 * sizex;
            BRB->x_max = x_max;
            BRB->y_min = y_min;
            BRB->y_max = y_min + 0.5 * sizey;
            BRB->z_min = z_min + 0.5 * sizez;
            BRB->z_max = z_max;
        }
    }


    inline void Destroy()
    {
        DestroyChildren();
    }

    inline void GetTotalItemCount()
    {
        //recursive over entire sub-tree
        if(HasChildren())
        {
            return ULF->GetTotalItemCount() + ULB->GetTotalItemCount() + URF->GetTotalItemCount() + URB->GetTotalItemCount()
                    + BLF->GetTotalItemCount() + BLB->GetTotalItemCount() + BRF->GetTotalItemCount() + BRB->GetTotalItemCount();
        }

    }

    inline void GetNSubLevels()
    {
        //recursive over entire sub-tree
        if(!HasChildren())
        {
            return 0;
        }else
        {
            return 1 + std::max(std::max(ULF->GetTotalItemCount(),ULB->GetTotalItemCount()),std::max(URF->GetTotalItemCount(),URB->GetTotalItemCount()),std::max(std::max(BLF->GetTotalItemCount(),BLB->GetTotalItemCount()),std::max(BRF->GetTotalItemCount(), BRB->GetTotalItemCount())));
        }
    }

    inline void DestroyChildren()
    {
        if(m_ULF != nullptr)
        {

            ULF->Destroy();
            delete ULF;
            ULB->Destroy();
            delete ULB;
            URF->Destroy();
            delete URF;
            URB->Destroy();
            delete URB;
            BLF->Destroy();
            delete BLF;
            BLB->Destroy();
            delete BLB;
            BRF->Destroy();
            delete BRF;
            BRB->Destroy();
            delete BRB;

        }

        ULF = nullptr;
        ULB = nullptr;
        URF = nullptr;
        URB = nullptr;
        BLF = nullptr;
        BLB = nullptr;
        BRF = nullptr;
        BRB = nullptr;
    }

    inline TreeNode * GetChild(double x, double y, double z)
    {
        if(ULF != nullptr)
        {
            if(ULF->Contains_Coord(x,y,z))
            {
                return ULF;
            }
            if(ULB->Contains_Coord(x,y,z))
            {
                return ULB;
            }
            if(URF->Contains_Coord(x,y,z))
            {
                return URF;
            }
            if(URB->Contains_Coord(x,y,z))
            {
                return URB;
            }
            if(BLF->Contains_Coord(x,y,z))
            {
                return BLF;
            }
            if(BLB->Contains_Coord(x,y,z))
            {
                return BLB;
            }
            if(BRF->Contains_Coord(x,y,z))
            {
                return BRF;
            }
            if(BRB->Contains_Coord(x,y,z))
            {
                return BRB;
            }
        }

        return nullptr;
    }

    inline bool Overlaps( double inx_min, double inx_max, double iny_min, double iny_max, double inz_min, double inz_max)
    {
        if((inx_min < x_min && inx_max < x_min) || (inx_min > x_min && inx_max > x_min))
        {
            return false;
        }
        if((iny_min < y_min && iny_max < y_min) || (iny_min > y_min && iny_max > y_min))
        {
            return false;
        }
        if((inz_min < z_min && inz_max < z_min) || (inz_min > z_min && inz_max > z_min))
        {
            return false;
        }

        return true;
    }
};


class Octree
{

    //root node
    TreeNode * m_Root;

    int m_MaxItems;
    int m_MaxLevel;
public:

    inline Octree(BoundingBox3D bb_initial, int max_level, int max_items, int max_level_add_for_grow = 2)
    {
        m_MaxItems = max_items;
        m_MaxLevel = max_level;
        m_Root = new TreeNode();
        m_Root->z_min = bb_initial.GetMinZ();
        m_Root->z_max = bb_initial.GetMaxZ();
        m_Root->x_min = bb_initial.GetMinX();
        m_Root->x_min = bb_initial.GetMaxX();
        m_Root->y_min = bb_initial.GetMinY();
        m_Root->y_min = bb_initial.GetMaxY();

    }

    inline void DistributeFromChildren(TreeNode * node)
    {
        //get all items in child nodes and put them in the parent
        //assumes the nodes children might have children
        //and that, even in case of too many items, it must be done (due to max level or some other cause

        if(node->HasChildren())
        {
            for(int i = 0; i < 8; i++)
            {
                TreeNode * child = node->GetChild(i);
                if(child->HasChildren())
                {
                    DistributeFromChildren(child);
                }
            }

            for(int i = 0; i < 8; i++)
            {
                TreeNode * child = node->GetChild(i);
                for(int j = child->ItemCount()-1; j> -1; j--)
                {
                    node->AddItemDirect(child->items[j]);
                    child->items.remove(items[j]);
                }
            }
        }

    }

    inline void CheckMaxLevels(TreeNode * node, int level)
    {
        if(level >= m_MaxLevel)
        {
            if(node->HasChildren())
            {
                DistributeFromChildren(node);
            }
        }else
        {
            if(node->HasChildren())
            {
                for(int i = 0; i < 8; i++)
                {
                    TreeNode * child = node->GetChild(i);
                    CheckMaxLevels(child,level+1);
                }
            }
        }


    }

    inline void AdaptRootToCoord(float x, float y, float z)
    {


        while(!m_Root->Contains_Coord(x,y,z))
        {
            double xn_min = 0.0;
            double xn_max = 0.0;
            double yn_min = 0.0;
            double yn_max = 0.0;
            double zn_min = 0.0;
            double zn_max = 0.0;


            if(x <= m_Root->x_min)
            {

                xn_min = m_Root->x_min -  (m_Root->x_max - m_Root->x_min);
                xn_max =  m_Root->x_min;

            }else
            {
                xn_min = m_Root->x_max ;
                xn_max =  m_Root->x_max +  (m_Root->x_max - m_Root->x_min);
            }
            if(y <= m_Root->y_min)
            {

                yn_min = m_Root->y_min -  (m_Root->y_max - m_Root->y_min);
                yn_max =  m_Root->y_min;

            }else
            {
                yn_min = m_Root->y_max ;
                yn_max =  m_Root->y_max +  (m_Root->y_max - m_Root->y_min);
            }

            if(z <= m_Root->z_min)
            {

                zn_min = m_Root->z_min -  (m_Root->z_max - m_Root->z_min);
                zn_max =  m_Root->z_min;

            }else
            {
                zn_min = m_Root->z_max ;
                zn_max =  m_Root->z_max +  (m_Root->z_max - m_Root->z_min);
            }

            TreeNode * newroot = new TreeNode();
            newroot->x_min = xn_min;
            newroot->x_max = xn_max;
            newroot->y_min = yn_min;
            newroot->y_max = yn_max;
            newroot->z_min = zn_min;
            newroot->z_max = zn_max;
            m_Root->Parent = newroot;
            m_Root = newroot;

        }

        CheckMaxLevels(m_Root,0);

    }

    inline void DistributeToChildren(TreeNode * node, int level)
    {
        //get all items in node and put them in the children if possible

        //no children if already on max level
        if(level >= m_MaxLevel)
        {
            return;
        }

        if(!node->HasChildren())
        {
            node->CreateChildren();
        }

        //go over all items
        for(int i = node->ItemCount() -1; i > -1 ; i--)
        {
            TreeNode * child =node->GetChild(node->items[i].x,node->items[i].y,node->items[i].z);

            //add to child and remove from parent
            if(child != nullptr)
            {
                child->AddItemDirect(node->items[i]);
                node->items.remove(node->items[i]);
            }

            //if child now has too many, distribute it to its children
            if(child->ItemCount() >= m_MaxItems)
            {
                DistributeToChildren(child, level + 1);
            }
        }
    }

    inline TreePointerRef * AddItem(int i, float x, float y, float z)
    {
        if(!(std::isnormal(x)&& std::isnormal(y) && std::isnormal(z)))
        {
            return nullptr;
        }
        TreeNode * node = m_Root;

        if(m_Root->Contains_Coord(x,y,z))
        {
            bool check_child = true;
            int level = -1;
            while(check_child)
            {
                level ++;
                check_child = false;
                if((!node->HasChildren()) && (node->ItemCount() < m_MaxItems || level >= m_MaxLevel))
                {
                    return node->AddItem(i,x,y,z);
                }else
                {
                    if(node->HasChildren())
                    {
                        TreeNode * node_prev = node;
                        node = node->GetChild(x,y,z);
                        if(node == nullptr)
                        {
                            return node_prev->AddItem(i,x,y,z);
                        }
                        check_child = true;
                    }else
                    {
                        node->CreateChildren();

                        DistributeToChildren(node,level);

                        TreeNode * node_prev = node;
                        node = node->GetChild(x,y,z);
                        if(node == nullptr)
                        {
                            return node_prev->AddItem(i,x,y,z);
                        }
                        check_child = true;
                    }

                }
            }
        }else
        {

            AdaptRootToCoord(x,y,z);
            TreeNode * newroot = m_Root;
            return AddItem(i,x,y,z);

        }
        return nullptr;
    }

    inline TreeNode * GetNode(double x, double y, double z)
    {
        TreeNode * node=  m_Root;

        while(node->HasChildren())
        {
            TreeNode * node_prev = node;
            node= node->GetChild(x,y,z);
            if(node == nullptr)
            {
                return node_prev;
            }
        }

        return node;
    }

    inline void PlaceItemInNode(TreePointerRef * ref,double x, double y, double z)
    {
        if(!(std::isnormal(x) && std::isnormal(y) && std::isnormal(z)))
        {
            return;
        }

        ref->x = x;
        ref->y = y;
        ref->z = z;

        if(m_Root->Contains_Coord(x,y,z))
        {
            TreeNode * node = GetNode(x,y,z);
            node->AddItemDirect(ref);

            if(node->ItemCount() >= m_MaxItems)
            {
                DistributeToChildren(node);
            }
        }else
        {
            AdaptRootToCoord(x,y,z);
            return PlaceItemInNode(ref,x,y,z);
        }

    }

    inline void RemoveItem(TreePointerRef * ref)
    {
        if(ref->item == nullptr)
        {
        }else
        {
             TreeNode * node = (TreeNode*)ref->item;
             node->items.remove(ref);
             if(node->Parent != nullptr)
             {
                if(node->Parent->GetTotalItemCount() < m_MaxItems)
                {
                    DistributeFromChildren(node->Parent);
                }
             }
        }


    }
    inline void MoveItem(TreePointerRef * ref, double x, double y, double z)
    {
        if(std::isnormal(x) && std::isnormal(y) && std::isnormal(z))
        {
            //moving a single item, so we need to check if the nodes parent still has too many sub-items to have children
            RemoveItem(ref);
            PlaceItemInNode(ref,x,y,z);
        }

    }

    inline void CallForItemsRegion(TreeNode * node, std::function<void(TreePointerRef *)> func, double x_min, double x_max, double y_min, double y_max, double z_min, double z_max)
    {
        //first get closest node that contains all of the region

        //then do a recusive iteration, but skip those subnodes that are completely outside region


        if(node->HasChildren())
        {
            for(int i = 0; i < 8; i++)
            {
                TreeNode * child = node->GetChild(i);
                if(child->Overlaps(x_min,x_max,y_min,y_max,z_min,z_max))
                {
                    CallForItemsRegion(child,func,x_min,x_max,y_min,y_max,z_min,z_max);
                }
            }
        }

        {
            for(int j = node->ItemCount()-1; j> -1; j--)
            {
                TreePointerRef * ref = node->items[j];
                if(ref->x >= x_min && ref->x < x_max && ref->y >= y_min && ref->y < y_max && ref->z >= z_min && ref->z < z_max)
                {
                    func(ref);
                }
            }
        }

    }

};
