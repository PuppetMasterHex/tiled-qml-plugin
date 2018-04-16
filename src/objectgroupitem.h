#ifndef OBJECTGROUP_H
#define OBJECTGROUP_H

#include <QQuickItem>
#include "mapitem.h"

namespace Tiled
{
  class ObjectGroup;
  class MapObject;
}

namespace TiledQuick
{
  class ObjectGroupItem : public QQuickItem
  {
    Q_OBJECT

    Q_PROPERTY(MapItem *tileMapItem READ mapItem WRITE setMapItem)
    Q_PROPERTY(int index READ globalIndex)
    Q_PROPERTY(QString name READ name WRITE setname NOTIFY nameChanged)
    Q_PROPERTY(QVariantMap customProperties READ customProps NOTIFY customPropsChanged)
    Q_PROPERTY(QVariantList mapObjects READ mapObjects NOTIFY mapObjectsChanged)

  public:
    ObjectGroupItem(QQuickItem *parentItem=nullptr);

    MapItem *mapItem() const { return m_map; }
    int globalIndex() const;
    QString name() const { return m_name; }
    QVariantMap customProps() const;
    QVariantList mapObjects() const { return m_objectList; }

  public slots:
    void setMapItem(MapItem *mapItem);
    void setname(QString groupName);

  signals:
    void nameChanged(QString name);
    void customPropsChanged(QVariantMap customProps);
    void mapObjectsChanged(QVariantList objectList);

    void objectAdded(QVariantMap mapObject);

  private:
    void initObjectGroup();

    MapItem *m_map = nullptr;
    QString m_name;

    Tiled::ObjectGroup *m_objectGroup = nullptr;
    QVariantList m_objectList;//used to hold the object properties after we add class instance data like id/name/x/y/width/height
  };
}

#endif // OBJECTGROUP_H
