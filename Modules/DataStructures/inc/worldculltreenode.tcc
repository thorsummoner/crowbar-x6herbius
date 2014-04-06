#include "worldculltreenode.h"
#include "iconstbboxvolume.h"
#include "geomutil.h"
#include <QList>
#include <Qt3D/QBox3D>

DATASTRUCTURES_BEGIN_NAMESPACE

// Helper functions!
template <typename T>
static T& deref_if_pointer(T& t) { return t; }
template <typename T>
static const T& deref_if_pointer(const T& t) { return t; }

template <typename T>
static T& deref_if_pointer(T* t) { return *t; }
template <typename T>
static const T& deref_if_pointer(const T* t) { return *t; }

template<typename T, int MD, int MO>
WorldCullTreeNode<T,MD,MO>::WorldCullTreeNode(QBox3D bounds, WorldCullTreeNode* parent) : TreeNode(parent),
    m_ObjectHash(), m_Bounds(bounds)
{
}

template<typename T, int MD, int MO>
WorldCullTreeNode<T,MD,MO>::~WorldCullTreeNode()
{
}

template<typename T, int MD, int MO>
QBox3D WorldCullTreeNode<T,MD,MO>::bounds() const
{
    return m_Bounds;
}

template<typename T, int MD, int MO>
void WorldCullTreeNode<T,MD,MO>::setBounds(QBox3D bounds)
{
    m_Bounds = bounds;
}

template<typename T, int MD, int MO>
void WorldCullTreeNode<T,MD,MO>::addObject(const T &obj)
{
    // Ensure we implement the required interfaces.
    Q_ASSERT( checkImplementsInterfaces(obj) );
    
    // Record the object.
    m_ObjectHash.insert(obj, 0);    // Char used for memory conservation.
}

template<typename T, int MD, int MO>
void WorldCullTreeNode<T,MD,MO>::addObjectRecurse(const T &obj)
{
    // Ensure we implement the required interfaces.
    Q_ASSERT( checkImplementsInterfaces(obj) );
    
    // If the bounding box intersects ours, add to this node (or its children).
    if ( GEOMETRY_NAMESPACE::boundingBoxesIntersect(bounds(), deref_if_pointer(obj).boundingBox()) )
    {
        // If we're not a leaf, add to our children.
        if ( !isLeaf() )
        {
            for ( int i = 0; i < childCount(); i++ )
            {
                WorldCullTreeNode* child = dynamic_cast<WorldCullTreeNode*>(childAt(i));
                Q_ASSERT( child );
                child->addObjectRecurse(obj);
            }
        }
        
        // Otherwise, add to this node.
        else
        {
            addObject(obj);
        }
    }
}

template<typename T, int MD, int MO>
void WorldCullTreeNode<T,MD,MO>::removeObject(const T &obj)
{
    // Ensure we implement the required interfaces.
    Q_ASSERT( checkImplementsInterfaces(obj) );
    
    // Remove the object.
    m_ObjectHash.remove(obj);
}

template<typename T, int MD, int MO>
void WorldCullTreeNode<T,MD,MO>::removeObjectRecurse(const T &obj)
{
    // Ensure we implement the required interfaces.
    Q_ASSERT( checkImplementsInterfaces(obj) );
    
    // Attempt removal from ourself.
    removeObject(obj);
    
    // Remove from all children too.
    for ( int i = 0; i < childCount(); i++ )
    {
        WorldCullTreeNode* child = dynamic_cast<WorldCullTreeNode*>(childAt(i));
        Q_ASSERT( child );
        child->removeObject(obj);
    }
}

template<typename T, int MD, int MO>
void WorldCullTreeNode<T,MD,MO>::removeAllObjects()
{
    m_ObjectHash.clear();
}

template<typename T, int MD, int MO>
void WorldCullTreeNode<T,MD,MO>::removeAllObjectsRecurse()
{
    // Remove our objects.
    removeAllObjects();
    
    // Remove all objects from our children.
    for ( int i = 0; i < childCount(); i++ )
    {
        WorldCullTreeNode* child = dynamic_cast<WorldCullTreeNode*>(childAt(i));
        Q_ASSERT( child );
        child->removeAllObjectsRecurse();
    }
}

template<typename T, int MD, int MO>
bool WorldCullTreeNode<T,MD,MO>::containsObject(const T &obj) const
{
    // Ensure we implement the required interfaces.
    Q_ASSERT( checkImplementsInterfaces(obj) );
    
    return m_ObjectHash.contains(obj);
}

template<typename T, int MD, int MO>
WorldCullTreeNode<T,MD,MO>* WorldCullTreeNode<T,MD,MO>::findObjectRecurse(const T &obj)
{
    // Ensure we implement the required interfaces.
    Q_ASSERT( checkImplementsInterfaces(obj) );
    
    // If the object is present in this node, return it.
    if ( containsObject(obj) ) return this;
    
    // Check recursively through children to see if the object is there.
    for ( int i = 0; i < childCount(); i++ )
    {
        WorldCullTreeNode* child = dynamic_cast<WorldCullTreeNode*>(childAt(i));
        Q_ASSERT( child );
        
        WorldCullTreeNode* found = child->findObjectRecurse(obj);
        if ( found ) return found; // Return the node if it holds the object.
    }
    
    return NULL;    // Return NULL otherwise.
}

template<typename T, int MD, int MO>
void WorldCullTreeNode<T,MD,MO>::updateObject(const T &obj)
{
    // Ensure we implement the required interfaces.
    Q_ASSERT( checkImplementsInterfaces(obj) );
    
    // If the bounding box intersects ours, add.
    if ( NGeometry::boundingBoxesIntersect(bounds(), deref_if_pointer(obj).boundingBox()) )
    {
        addObject(obj);
    }
    
    // If not, remove.
    else
    {
        removeObject(obj);
    }
}

template<typename T, int MD, int MO>
void WorldCullTreeNode<T,MD,MO>::updateObjectRecurse(const T &obj)
{
    // Ensure we implement the required interfaces.
    Q_ASSERT( checkImplementsInterfaces(obj) );
    
    // If we have children, update in them first.
    if ( !isLeaf() )
    {
        for ( int i = 0; i < childCount(); i++ )
        {
            WorldCullTreeNode* child = dynamic_cast<WorldCullTreeNode*>(childAt(i));
            Q_ASSERT( child );
            child->updateObjectRecurse(obj);
        }
    }
    
    // Otherwise, update in this node.
    else
    {
        updateObject(obj);
    }
}

template<typename T, int MD, int MO>
void WorldCullTreeNode<T,MD,MO>::updateAllObjectsRecurse()
{
    // If we have children, update in them first.
    if ( !isLeaf() )
    {
        for ( int i = 0; i < childCount(); i++ )
        {
            WorldCullTreeNode* child = dynamic_cast<WorldCullTreeNode*>(childAt(i));
            Q_ASSERT( child );
            child->updateAllObjectsRecurse();
        }
    }
    
    // Otherwise, update in this node.
    else
    {
        // I don't think we can remove objects live when iterating, so make a list.
        QList<T> toRemove;
        
        // Discover objects to remove.
        // Whoops, foreach returns the values from the hash, not the keys...
//        foreach( T obj, m_ObjectHash )
//        {
//            if ( NGeometry::boundingBoxesIntersect(bounds(), deref_if_pointer(obj).boundingBox()) )
//            {
//                toRemove.append(obj);
//            }
//        }
        
        QHash<T,char>::const_iterator it = m_ObjectHash.constBegin();
        
        while ( it != m_ObjectHash.constEnd() )
        {
            T obj = it.key();
            
            if ( NGeometry::boundingBoxesIntersect(bounds(), deref_if_pointer(obj).boundingBox()) )
            {
                toRemove.append(obj);
            }
        }
        
        // Remove them.
        foreach( T obj, toRemove )
        {
            removeObject(obj);
        }
    }
}

template<typename T, int MD, int MO>
int WorldCullTreeNode<T,MD,MO>::objectCount() const
{
    return m_ObjectHash.size();
}

template<typename T, int MD, int MO>
bool WorldCullTreeNode<T,MD,MO>::checkImplementsInterfaces(const T &obj) const
{
    // This -should- cause a compilation error if the method is not implemented.
    return deref_if_pointer(obj)._implementsIConstBBoxVolume();
}

template<typename T, int MD, int MO>
typename QHash<T,char>::const_iterator WorldCullTreeNode<T,MD,MO>::objectsConstBegin() const
{
    return m_ObjectHash.constBegin();
}

template<typename T, int MD, int MO>
typename QHash<T,char>::const_iterator WorldCullTreeNode<T,MD,MO>::objectsConstEnd() const
{
    return m_ObjectHash.constEnd();
}

template<typename T, int MD, int MO>
typename QHash<T,char>::iterator WorldCullTreeNode<T,MD,MO>::objectsBegin()
{
    return m_ObjectHash.begin();
}

template<typename T, int MD, int MO>
typename QHash<T,char>::iterator WorldCullTreeNode<T,MD,MO>::objectsEnd()
{
    return m_ObjectHash.end();
}

template<typename T, int MD, int MO>
void WorldCullTreeNode<T,MD,MO>::splitRecurse()
{
    Q_ASSERT( isLeaf() );
    if ( !isLeaf() ) return;
    
    // Splitting process:
    // If all of the node's dimensions are large enough:
    // - Create 8 children with appropriate bounding boxes.
    // - Add the relevant children of this node to each child node.
    // - Remove all objects from this node since it is no longer a leaf.
    // - Call splitRecurse() on all children containing enough objects.
    
    QVector3D max = bounds().maximum();
    QVector3D min = bounds().minimum();
    QVector3D size = max - min;         // Length of box in X/Y/Z axes.
    
    // If the box is not large enough for us to split, return.
    if ( size.x() < splitMinDimensions() || size.y() < splitMinDimensions() || size.z() < splitMinDimensions() ) return;
    
    // Get the mid-point of the box.
    QVector3D midpoint = (min + max) / 2.0;
    
    // Create 8 children.
    for ( int i = 0; i < 8; i++ )
    {
        // Calculate the bounds of the new child.
        QVector3D childMin;
        QVector3D childMax;
        
        // Copying Valve's way of doing this, which seems to be quite efficient and save headaches.
        if (i & 1)
        {
            childMin.setX(min.x());
            childMax.setX(midpoint.x());
        }
        else
        {
            childMin.setX(midpoint.x());
            childMax.setX(max.x());
        }
        
        if (i & 2)
        {
            childMin.setY(min.y());
            childMax.setY(midpoint.y());
        }
        else
        {
            childMin.setY(midpoint.y());
            childMax.setY(max.y());
        }
        
        if (i & 4)
        {
            childMin.setZ(min.z());
            childMax.setZ(midpoint.z());
        }
        else
        {
            childMin.setZ(midpoint.z());
            childMax.setZ(max.z());
        }
        
        QBox3D childBounds(childMin, childMax);
        
        // Add the new node.
        WorldCullTreeNode<T,MD,MO>* childNode = new WorldCullTreeNode<T,MD,MO>(childBounds, this);
        addChild(childNode);
        
        // For each object in this node, check to see if it should be inserted into this new child.
        for ( QHash<T,char>::iterator it = m_ObjectHash.begin(); it != m_ObjectHash.end(); it++ )
        {
            Q_ASSERT( it.key() != NULL && checkImplementsInterfaces(it.key()) );
            
            // If the bounding boxes intersect:
            if ( GEOMETRY_NAMESPACE::boundingBoxesIntersect(childNode->bounds(), deref_if_pointer(it.key()).boundingBox()) )
            {
                childNode->addObject(it.key());
            }
        }
    }
    
    // Remove all our objects, since we're no longer a leaf.
    removeAllObjects();
    
    // Call split on all children who meet the minimum object number requirement.
    for ( int i = 0; i < childCount(); i++ )
    {
        WorldCullTreeNode<T,MD,MO>* node = cullTreeChildAt(i);
        Q_ASSERT( node );
        
        if ( node->objectCount() >= splitMinObjects() )
        {
            node->splitRecurse();
        }
    }
}

template<typename T, int MD, int MO>
WorldCullTreeNode<T,MD,MO>* WorldCullTreeNode<T,MD,MO>::cullTreeChildAt(int index) const
{
    return dynamic_cast<WorldCullTreeNode<T,MD,MO>*>(childAt(index));
}

DATASTRUCTURES_END_NAMESPACE
