#ifndef RENDERBOX_H
#define RENDERBOX_H

#include "geometry_global.h"

#include <Qt3D/QBox3D>
#include <QVector3D>
#include <QColor>

#include "irenderable.h"
#include "iconstbboxvolume.h"

GEOMETRY_BEGIN_NAMESPACE

class GEOMETRYSHARED_EXPORT RenderBox : public QBox3D, public NIRenderSystem::IRenderable, public IConstBBoxVolume
{
public:
    explicit RenderBox();
    explicit RenderBox(const QVector3D &origin, const QVector3D &min, const QVector3D &max);
    
    // === All functions inherited from QBox3D refer to the local space of this object. ===

    QColor colorMin() const;
    void setColorMin(const QColor &col);
    
    QColor colorMax() const;
    void setColorMax(const QColor &col);
    
    void setColor(const QColor &col);
    
    // Physical origin of this box in the world.
    QVector3D wOrigin() const;
    void setWOrigin(const QVector3D &pos);
    
    // Centre of the rendered box (which may be different from the origin),
    // in world space. center() returns in local space.
    QVector3D wCenter() const;
    
    // IRenderable interface
    virtual QGeometryData toGeomData() const;
    
    // IConstBBoxVolume interface.
    virtual QBox3D boundingBox() const;
    virtual bool _implementsIConstBBoxVolume() const;
    
private:
    QVector3D   m_vecPosition;
    QColor      m_colColourMin;
    QColor      m_colColourMax;
};
    
GEOMETRY_END_NAMESPACE

#endif // RENDERBOX_H
