/*
 * maploader.h
 * Copyright 2015, Thorbjørn Lindeijer <bjorn@lindeijer.nl>
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

#ifndef TILEDQUICK_MAPLOADER_H
#define TILEDQUICK_MAPLOADER_H

#include <QObject>
#include <QVariant>
#include <QUrl>

namespace Tiled {
class Map;
}

namespace TiledQuick {

class MapLoader : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)

    Q_ENUMS(Status)

public:
    enum Status {
        Null,
        Ready,
        Error
    };

    explicit MapLoader(QObject *parent = 0);
    ~MapLoader();

    QUrl source() const;
    Tiled::Map *map() const;
    Status status() const;
    QString error() const;

signals:
    void sourceChanged(const QUrl &source);
    void mapChanged(Tiled::Map *map);
    void statusChanged(Status status);
    void errorChanged(const QString &error);

public slots:
    void setSource(const QUrl &source);

private:
    QUrl m_source;
    Tiled::Map *m_map;
    Status m_status;
    QString m_error;
};

inline Tiled::Map *MapLoader::map() const
{
    return m_map;
}

inline MapLoader::Status MapLoader::status() const
{
    return m_status;
}

inline QString MapLoader::error() const
{
    return m_error;
}

inline QUrl MapLoader::source() const
{
    return m_source;
}

} // namespace TiledQuick

#endif // TILEDQUICK_MAPLOADER_H
