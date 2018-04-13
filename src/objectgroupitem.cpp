#include "objectgroupitem.h"

#include "map.h"
#include "objectgroup.h"
#include "mapobject.h"
#include "objecttypes.h"



namespace TiledQuick
{
  ObjectGroupItem::ObjectGroupItem(QQuickItem *t_parent) : QQuickItem(t_parent)
  {

  }

  int ObjectGroupItem::globalIndex() const
  {
    return Tiled::globalIndex(m_objectGroup);
  }

  QVariantMap ObjectGroupItem::customProps() const
  {
    if(!m_objectGroup)
      return QVariantMap();
    else
      return m_objectGroup->properties();
  }

  void ObjectGroupItem::setMapItem(MapItem *t_map)
  {
    if(!t_map || m_map == t_map)
      return;

    if(m_map)
    {
      disconnect(m_map, &MapItem::mapChanged, this, &ObjectGroupItem::initObjectGroup);
      disconnect(m_map, &MapItem::objectTypesSourceChanged, this ,&ObjectGroupItem::initObjectGroup);
    }

    m_map = t_map;
    connect(m_map, &MapItem::mapChanged, this, &ObjectGroupItem::initObjectGroup);
    connect(m_map, &MapItem::objectTypesSourceChanged, this ,&ObjectGroupItem::initObjectGroup);

//    if(m_map->map())
//      initObjectGroup();
  }

  void ObjectGroupItem::setname(QString t_name)
  {
    Q_ASSERT(t_name.isEmpty() == false);
    if (m_name == t_name)
      return;

    m_name = t_name;
    emit nameChanged(m_name);
  }

  void ObjectGroupItem::initObjectGroup()
  {
    Q_ASSERT(m_name.isEmpty() == false);
    if(m_name.isEmpty())
    {
      qWarning() << "ObjectGroup has no name";
      return;
    }

    m_objectGroup = m_map->getObjectGroupByName(m_name);

    if(!m_objectGroup)
    {
      qWarning() << "ObjectGroup" << m_name << "not found in map";
      return;
    }

    emit customPropsChanged(m_objectGroup->properties());
    m_objectList.clear();

    QList<QVariantMap> notificationList; //used to emit objectAdded after the mapObjectsChanged was emitted
    for(Tiled::MapObject *tmpMapObj : qAsConst(m_objectGroup->objects()))
    {
      ///@note Tiled::FilePath is currently not supported and it should be replaced with QUrl in the future (it is just a QUrl in a struct)
      QVariantMap tmpObjProps;
      for(Tiled::ObjectType tmpType : tmpMapObj->objectTypes())
      {
        if(tmpType.name == tmpMapObj->type())
        {
          tmpObjProps = tmpType.defaultProperties;
          if(tmpType.color.isValid())
            tmpObjProps.insert("color", tmpType.color);
          break;
        }
      }
      const auto props = tmpMapObj->properties();
      for(QString tmpPropName : props.keys())
      {
        tmpObjProps.insert(tmpPropName, props.value(tmpPropName));
      }
      tmpObjProps.insert("objectId", tmpMapObj->id());
      tmpObjProps.insert("name", tmpMapObj->name());
      tmpObjProps.insert("type", tmpMapObj->type());
      tmpObjProps.insert("x", tmpMapObj->x());
      tmpObjProps.insert("y", tmpMapObj->y());
      tmpObjProps.insert("width", tmpMapObj->width());
      tmpObjProps.insert("height", tmpMapObj->height());
      tmpObjProps.insert("bounds", tmpMapObj->bounds());
      tmpObjProps.insert("rotation", tmpMapObj->rotation());
      tmpObjProps.insert("visible", tmpMapObj->isVisible());
       if(tmpObjProps.contains("z") == false)
         tmpObjProps.insert("z", globalIndex()); //set up the z value as the index of the layer
      if(tmpObjProps.contains("color") == false)
        tmpObjProps.insert("color", m_objectGroup->color()); ///@note this is a convenience for easier debugging when displaying objects
      m_objectList.append(tmpObjProps);
      notificationList.append(tmpObjProps);
    }

    emit mapObjectsChanged(m_objectList);

    for(const auto tmpObj : qAsConst(notificationList))
    {
      emit objectAdded(tmpObj); //emit after mapObjectsChanged to avoid unexpected behavior (object was added but is not in objects property)
    }
  }
}
