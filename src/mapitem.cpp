/*
 * mapitem.cpp
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

#include "mapitem.h"

#include "tilelayeritem.h"

#include "isometricrenderer.h"
#include "map.h"
#include "orthogonalrenderer.h"
#include "tilelayer.h"
#include "objectgroup.h"
#include "maploader.h"
#include "objecttypes.h"

#include <cmath>
#include <QQmlEngine>

using namespace TiledQuick;

MapItem::MapItem(QQuickItem *parent)
  : QQuickItem(parent)
  , mMap(0)
  , mRenderer(0)
{
}

void MapItem::setModel(MapLoader *t_model)
{
  Q_ASSERT(t_model != nullptr);
  if(mModel == t_model)
    return;

  if(mModel)
    disconnect(mModel, &MapLoader::mapChanged, this, &MapItem::setMap); //why would someone change the MapLoader?? investigate if we have to disconnect here
  mModel = t_model;
  connect(mModel, &MapLoader::mapChanged, this, &MapItem::setMap);
  setMap(mModel->map());
}

void MapItem::setObjectTypesSource(QUrl objectTypesSource)
{
  if (mObjectTypesSource == objectTypesSource)
    return;

  mObjectTypesSource = objectTypesSource;
  Tiled::ObjectTypes objTypesList;
  if(!objectTypesSource.isEmpty())
  {
    Tiled::ObjectTypesSerializer serializer;
    serializer.readObjectTypes(objectTypesSource.toLocalFile(), objTypesList);
    if(!serializer.errorString().isEmpty())
    {
      qWarning() << "Error parsing objecttypes xml file:" << objectTypesSource.toLocalFile() << serializer.errorString();
    }
    else
    {
      Tiled::Object::setObjectTypes(objTypesList);
    }
  }
  emit objectTypesSourceChanged(mObjectTypesSource);
}

void TiledQuick::MapItem::MapItem::setMap(Tiled::Map *map)
{
  if (mMap == map)
    return;

  emit aboutToChangeMap();
  mMap = map;
  refresh();
}

void MapItem::setVisibleArea(const QRectF &visibleArea)
{
  mVisibleArea = visibleArea;
  emit visibleAreaChanged();
}

/**
 * Determines the rectangle of visible tiles of the given tile \a layer, based
 * on the visible area of this MapItem instance.
 *
 * Only works for orthogonal maps.
 */
QRect MapItem::visibleTileArea(const Tiled::TileLayer *layer) const
{
  const int tileWidth = mMap->tileWidth();
  const int tileHeight = mMap->tileHeight();

  QMargins drawMargins = layer->drawMargins();
  drawMargins.setTop(drawMargins.top() - tileHeight);
  drawMargins.setRight(drawMargins.right() - tileWidth);

  QRectF rect = visibleArea().adjusted(-drawMargins.right(),
                                       -drawMargins.bottom(),
                                       drawMargins.left(),
                                       drawMargins.top());

  int startX = qMax((int) rect.x() / tileWidth, 0);
  int startY = qMax((int) rect.y() / tileHeight, 0);
  int endX = qMin((int) std::ceil(rect.right()) / tileWidth, layer->width() - 1);
  int endY = qMin((int) std::ceil(rect.bottom()) / tileHeight, layer->height() - 1);

  return QRect(QPoint(startX, startY), QPoint(endX, endY));
}

int MapItem::tileLayerCount() const
{
  if(!mMap)
  {
    return -1;
  }
  else
  {
    return mMap->tileLayerCount();
  }
}

int MapItem::objectGroupCount() const
{
  if(!mMap)
  {
    return -1;
  }
  else
  {
    return mMap->objectGroupCount();
  }
}

QRectF MapItem::boundingRect() const
{
  if (!mRenderer)
    return QRectF();

  const auto mapSize = mMap->size();
  return QRectF(0,0, mapSize.width(), mapSize.height());
}

QVariantMap MapItem::customProperties() const
{
  if(!mMap)
    return QVariantMap();
  else
    return mMap->properties();
}

Tiled::ObjectGroup *MapItem::getObjectGroupByName(const QString &name) const
{
  return mObjectGroupTable.value(name, nullptr);
}

QPointF MapItem::screenToTileCoords(qreal x, qreal y) const
{
  if (!mRenderer)
    return QPointF(x, y);
  return mRenderer->screenToTileCoords(x, y);
}

QPointF MapItem::screenToTileCoords(const QPointF &position) const
{
  if (!mRenderer)
    return position;
  return mRenderer->screenToTileCoords(position);
}

QPointF MapItem::tileToScreenCoords(qreal x, qreal y) const
{
  if (!mRenderer)
    return QPointF(x, y);
  return mRenderer->tileToScreenCoords(x, y);
}

QPointF MapItem::tileToScreenCoords(const QPointF &position) const
{
  if (!mRenderer)
    return position;
  return mRenderer->tileToScreenCoords(position);
}

QPointF MapItem::screenToPixelCoords(qreal x, qreal y) const
{
  if (!mRenderer)
    return QPointF(x, y);
  return mRenderer->screenToPixelCoords(x, y);
}

QPointF MapItem::screenToPixelCoords(const QPointF &position) const
{
  if (!mRenderer)
    return position;
  return mRenderer->screenToPixelCoords(position);
}

QPointF MapItem::pixelToScreenCoords(qreal x, qreal y) const
{
  if (!mRenderer)
    return QPointF(x, y);
  return mRenderer->pixelToScreenCoords(x, y);
}

QPointF MapItem::pixelToScreenCoords(const QPointF &position) const
{
  if (!mRenderer)
    return position;
  return mRenderer->pixelToScreenCoords(position);
}

QPointF MapItem::pixelToTileCoords(qreal x, qreal y) const
{
  if (!mRenderer)
    return QPoint(x, y);
  return mRenderer->pixelToTileCoords(x, y);
}

QPointF MapItem::pixelToTileCoords(const QPointF &position) const
{
  if (!mRenderer)
    return position;
  return mRenderer->pixelToTileCoords(position);
}

TileLayerItem *MapItem::getTileLayerItemAt(int index) const
{
  TileLayerItem *retVal = nullptr;
  if(index >= 0 && mMap && index < mTileLayerItems.count())
  {
    retVal = mTileLayerItems.at(index);
    QQmlEngine::setObjectOwnership(retVal, QQmlEngine::CppOwnership); //otherwise QML would take ownership and delete the object
  }
  return retVal;
}

void MapItem::componentComplete()
{
  QQuickItem::componentComplete();
  if (mMap)
    refresh();
}

void MapItem::refresh()
{
  if (!isComponentComplete())
    return;

  qDeleteAll(mTileLayerItems);
  mTileLayerItems.clear();
  emit tileLayerItemCountChanged(mTileLayerItems.count());

  delete mRenderer;
  mRenderer = 0;

  if (!mMap)
    return;

  switch (mMap->orientation()) {
    case Tiled::Map::Isometric:
      mRenderer = new Tiled::IsometricRenderer(mMap);
      break;
    default:
      mRenderer = new Tiled::OrthogonalRenderer(mMap);
      break;
  }

  foreach (Tiled::Layer *layer, mMap->layers()) {
    if (Tiled::TileLayer *tl = layer->asTileLayer()) {
      TileLayerItem *layerItem = new TileLayerItem(tl, mRenderer, this);
      layerItem->setZ(tl->siblingIndex());
      mTileLayerItems.append(layerItem);
      qDebug() << "Added layer" << tl->name() << "z:" << tl->siblingIndex() << "bounds:" << tl->bounds();
    }
  }
  emit tileLayerItemCountChanged(mTileLayerItems.count());

  mObjectGroupTable.clear();
  const auto objGroups = mMap->objectGroups();
  for(Tiled::ObjectGroup *tmpGroup : objGroups)
  {
    qDebug() << "Added object group:" << tmpGroup->name();
    mObjectGroupTable.insert(tmpGroup->name(), tmpGroup);
  }


  const QSize size = mMap->size();
  setImplicitSize(size.width(), size.height());
  emit customPropertiesChanged(mMap->properties());
  emit mapChanged(); //only emit after the renderer is initialized
}
