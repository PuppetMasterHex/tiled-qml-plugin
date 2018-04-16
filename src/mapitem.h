/*
 * mapitem.h
 * Copyright 2014, Thorbjørn Lindeijer <bjorn@lindeijer.nl>
 *
 * This file is part of Tiled Quick.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TILEDQUICK_MAPITEM_H
#define TILEDQUICK_MAPITEM_H

#include <QQuickItem>

#include "maploader.h"

namespace Tiled {
  class Map;
  class MapRenderer;
  class Tileset;
  class TileLayer;
  class ObjectGroup;
} // namespace Tiled

namespace TiledQuick {
  //class MapLoader;
  class TileItem;
  class TileLayerItem;

  /**
 * A declarative item that displays a map.
 */
  class MapItem : public QQuickItem
  {
    Q_OBJECT
    Q_ENUMS(Status)

    Q_PROPERTY(MapLoader *mapSource READ model WRITE setModel)
    Q_PROPERTY(QUrl objectTypesSource READ objectTypesSource WRITE setObjectTypesSource NOTIFY objectTypesSourceChanged) //set this if you want the default properties of the objecttypes.xml
    Q_PROPERTY(QRectF visibleArea READ visibleArea WRITE setVisibleArea NOTIFY visibleAreaChanged)
    Q_PROPERTY(int tileLayerCount READ tileLayerCount)
    Q_PROPERTY(int tileLayerItemCount READ tileLayerItemCount NOTIFY tileLayerItemCountChanged)
    Q_PROPERTY(int objectGroupCount READ objectGroupCount)
    Q_PROPERTY(QVariantMap customProperties READ customProperties NOTIFY customPropertiesChanged)

  public:
    explicit MapItem(QQuickItem *parent = 0);


    enum LayerTypeFlag {
      TileLayerType   = 0x01,
      ObjectGroupType = 0x02,
      ImageLayerType  = 0x04,
      GroupLayerType  = 0x08
    };
    Q_ENUM(LayerTypeFlag)

    MapLoader *model() const { return mModel; }
    void setModel(MapLoader *mapModel);
    Tiled::Map *map() const;
    QUrl objectTypesSource() const { return mObjectTypesSource; }
    void setObjectTypesSource(QUrl objectTypesSource);
    const QRectF &visibleArea() const;
    void setVisibleArea(const QRectF &visibleArea);
    QRect visibleTileArea(const Tiled::TileLayer *layer) const;
    int tileLayerCount() const;
    int tileLayerItemCount() const { return mTileLayerItems.count(); }
    int objectGroupCount() const;
    QRectF boundingRect() const;
    QVariantMap customProperties() const;

    Tiled::ObjectGroup *getObjectGroupByName(const QString &name) const;

    Q_INVOKABLE QPointF screenToTileCoords(qreal x, qreal y) const;
    Q_INVOKABLE QPointF screenToTileCoords(const QPointF &position) const;
    Q_INVOKABLE QPointF tileToScreenCoords(qreal x, qreal y) const;
    Q_INVOKABLE QPointF tileToScreenCoords(const QPointF &position) const;
    Q_INVOKABLE QPointF screenToPixelCoords(qreal x, qreal y) const;
    Q_INVOKABLE QPointF screenToPixelCoords(const QPointF &position) const;
    Q_INVOKABLE QPointF pixelToScreenCoords(qreal x, qreal y) const;
    Q_INVOKABLE QPointF pixelToScreenCoords(const QPointF &position) const;
    Q_INVOKABLE QPointF pixelToTileCoords(qreal x, qreal y) const;
    Q_INVOKABLE QPointF pixelToTileCoords(const QPointF &position) const;

    Q_INVOKABLE TileLayerItem *getTileLayerItemAt(int index) const;
    Q_INVOKABLE TileLayerItem *getTileLayerItemByName(const QString tileLayerName) const;
    void componentComplete();

  signals:
    void aboutToChangeMap();
    void mapChanged();
    void visibleAreaChanged();
    void tileLayerItemCountChanged(int newCount);
    void customPropertiesChanged(QVariantMap customProps);
    void objectTypesSourceChanged(QUrl objectTypesSource);

  private slots:
    void setMap(Tiled::Map *map);

  private:
    void refresh();

    MapLoader *mModel = nullptr;
    Tiled::Map *mMap;
    QRectF mVisibleArea;

    Tiled::MapRenderer *mRenderer;
    QList<TileLayerItem*> mTileLayerItems;
    QHash<QString, Tiled::ObjectGroup*> mObjectGroupTable;
    QUrl mObjectTypesSource;
  };

  inline const QRectF &MapItem::visibleArea() const
  { return mVisibleArea; }

  inline Tiled::Map *MapItem::map() const
  { return mMap; }

} // namespace TiledQuick

#endif // TILEDQUICK_MAPITEM_H

