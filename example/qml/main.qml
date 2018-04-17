import QtQuick.Window 2.2
import QtQuick 2.8
import Box2D 2.0
import org.mapeditor.Tiled 1.0 as Tiled

Window {
  id: appWindow
  visible: true
  width: 640
  height: 480
  title: qsTr("Hello World")

  Rectangle { anchors.fill: parent; color: "#000011" }

  function createMapObject(type, customProperties) {
    if(!customProperties)
    {
      customProperties = {};
    }

    var component = Qt.createComponent(type);
//    if (component.status === Component.Ready)
//      finishCreation();
//    else
//      component.statusChanged.connect(finishCreation);
    if (component.status === Component.Ready)
    {
      return component.createObject(mapItem, customProperties);
    }
    else
    {
      console.warn("Error creating object of type:", type, component.errorString());
      return undefined;
    }
  }

  function setMapObjectProps(mapObject, qmlObject) {

  }

  Item {
    id: playerController

    focus: true
    Keys.onReleased: {
      switch(event.key) {
      case Qt.Key_Right:
        player.xMovement -= 1.0;
        break;
      case Qt.Key_Left:
        player.xMovement += 1.0;
        break;
      case Qt.Key_Up:
        player.yMovement += 1.0;
        break;
      case Qt.Key_Down:
        player.yMovement -= 1.0;
      }
    }

    Keys.onRightPressed: player.xMovement += 1.0;
    Keys.onLeftPressed: player.xMovement -= 1.0;
    Keys.onDownPressed: player.yMovement += 1.0;
    Keys.onUpPressed: player.yMovement -= 1.0;
    Keys.onSpacePressed: {
      var tilePos10x10y = mapItem.tileToScreenCoords(10, 10);
      player.x = tilePos10x10y.x
      player.y = tilePos10x10y.y
    }

//    Timer {
//      interval: 10
//      repeat: true
//      running: true
//      onTriggered: {
//        player.x = player.x + (player.speed * player.hMovement)
//        player.y = player.y + (player.speed * player.vMovement)
//        mapContainer.x = -((player.x + player.width/2) * mapContainer.scale - appWindow.width/2)
//        mapContainer.y = -((player.y + player.height/2) * mapContainer.scale - appWindow.height/2)
//      }
//    }
  }

  Text {
    x: 20
    y: 20
    z: 100
    color: "white"
    style: Text.Outline;
    styleColor: "black"
    font.pointSize: 14
    property int tileX: mapItem.screenToTileCoords(player.x+player.width/2, player.y+player.height/2).x
    property int tileY: mapItem.screenToTileCoords(player.x+player.width/2, player.y+player.height/2).y
    text: String("x: %1 y: %2 \ntileX: %3 tileY: %4").arg(player.x).arg(player.y).arg(tileX).arg(tileY)
  }

  Timer {
    interval: 2000
    repeat: false
    //running: true
    onTriggered: {
      player.playerStartPos = mapItem.tileToScreenCoords(10, 23);
      mapLoader.source = "file:///" + exampleDir + "/desert.tmx"
    }
  }

  Item {
    id: mapView
    anchors.fill: parent

    StaticWall {
      anchors.left: mapContainer.left
      anchors.right: mapContainer.right
      anchors.bottom: mapContainer.top
      height: 32
    }

    Item {
      id: mapContainer
      x: -((player.x + player.width/2) * mapContainer.scale - appWindow.width/2)
      y: -((player.y + player.height/2) * mapContainer.scale - appWindow.height/2)

      World {
        id: physicsWorld
        gravity: Qt.point(0,0)
      }

      DebugDraw {
          id: debugDraw
          world: physicsWorld
          opacity: 0.75
          visible: false
          z: 100
      }


      ParallelAnimation {
        id: containerAnimation

        property alias scale: scaleAnimation.to
        property alias x: xAnimation.to
        property alias y: yAnimation.to
        alwaysRunToEnd: true

        NumberAnimation { id: scaleAnimation; target: mapContainer; property: "scale"; easing.type: Easing.OutCubic; duration: 100 }
        NumberAnimation { id: xAnimation; target: mapContainer; property: "x"; easing.type: Easing.OutCubic; duration: 100 }
        NumberAnimation { id: yAnimation; target: mapContainer; property: "y"; easing.type: Easing.OutCubic; duration: 100 }
      }

      Component.onCompleted: {
        containerAnimation.scale = mapContainer.scale
        containerAnimation.x = mapContainer.x
        containerAnimation.y = mapContainer.y
      }


      Tiled.ObjectGroup {
        id: collisionObjects
        tileMapItem: mapItem
        name: "Collision"

        Shortcut {
          //enabled: BUILD_TYPE === "debug"
          sequence: "F3"
          autoRepeat: false
          onActivated: {
            collisionObjects.visible = !collisionObjects.visible
          }
        }

        onObjectAdded: {
          var newObj = createMapObject(mapObject.qmlType, mapObject);
          if(newObj)
          {
            newObj.visible = Qt.binding(function(){ return collisionObjects.visible; })
            newObj.opacity = 0.2
            if(newObj.persistent)
            {
              //console.log("Added persistent object:", newObj.name)
              mapItem.persistentObjectList.push(newObj);
            }
            else
            {
              //console.log("Added temp object:", newObj.name)
              mapItem.temporaryObjectList.push(newObj);
            }
          }
        }
      }

      Tiled.ObjectGroup {
        id: teleportObects
        tileMapItem: mapItem
        name: "Teleports"

        Shortcut {
          //enabled: BUILD_TYPE === "debug"
          sequence: "F4"
          autoRepeat: false
          onActivated: {
            teleportObects.visible = !teleportObects.visible
          }
        }

        onObjectAdded: {
          console.log("Added teleport object:", mapObject.name, mapObject.roomFileName)
          var newObj = createMapObject(mapObject.qmlType, mapObject);
          if(newObj)
          {
            newObj.visible = Qt.binding(function(){ return teleportObects.visible; })
            newObj.opacity = 0.2
            if(newObj.persistent)
            {
              //console.log("Added persistent object:", newObj.name)
              mapItem.persistentObjectList.push(newObj);
            }
            else
            {
              //console.log("Added temp object:", newObj.name)
              mapItem.temporaryObjectList.push(newObj);
            }
          }
        }
      }

      Tiled.ObjectGroup {
        id: eventObjects
        tileMapItem: mapItem
        name: "Secrets"

        Shortcut {
          //enabled: BUILD_TYPE === "debug"
          sequence: "F5"
          autoRepeat: false
          onActivated: {
            eventObjects.visible = !eventObjects.visible
          }
        }

        onObjectAdded: {
          console.log("Added secret object:", mapObject.name, mapObject.roomFileName)
          var newObj = createMapObject(mapObject.qmlType, mapObject);
          if(newObj)
          {
            newObj.visible = Qt.binding(function(){ return eventObjects.visible; })
            newObj.opacity = 0.2
            if(newObj.persistent)
            {
              //console.log("Added persistent object:", newObj.name)
              mapItem.persistentObjectList.push(newObj);
            }
            else
            {
              //console.log("Added temp object:", newObj.name)
              mapItem.temporaryObjectList.push(newObj);
            }
          }
        }
      }

      Tiled.MapItem {
        id: mapItem

        property var temporaryObjectList: []
        property var persistentObjectList: []

        property QtObject secretTileLayerItem;

        mapSource: Tiled.MapLoader {
          id: mapLoader
          property string mapName: "sewers.tmx"
          source:  "file://" + exampleDir + "/" + mapName; //"/isometric_grass_and_water.tmx"
          Component.onCompleted: console.log("Loaded map", source);
        }
        objectTypesSource: "file://" + exampleDir + "/example_objecttypes.xml"
        //        layer.enabled: mapContainer.scale < 1.0
        //        layer.smooth: true
        //        layer.mipmap: true
        visibleArea: {
          var scale = mapContainer.scale
          Qt.rect(-mapContainer.x / scale,
                  -mapContainer.y / scale,
                  mapView.width / scale,
                  mapView.height / scale);
        }

        PlayerActor {
          id: player
          width: 24
          height: 24

          onTeleportActivated: {
            if(map)
            {
              console.log("next map:", map)
              mapLoader.mapName = map;
            }


            var newCoords = mapItem.tileToScreenCoords(xTile, yTile)
            x = newCoords.x
            y = newCoords.y
          }

          onSecretAreasVisibleChanged: mapItem.secretTileLayerItem.visible=!secretAreasVisible

          property var playerStartPos;
        }

        onTileLayerItemCountChanged: {
          for(var tileLayerItemIndex = 0; tileLayerItemIndex < mapItem.tileLayerItemCount; ++tileLayerItemIndex)
          {
            var tmpTLI = mapItem.getTileLayerItemAt(tileLayerItemIndex);
            //tmpTLI.z = tmpTLI.index;
            console.log(tmpTLI.name, "z:", tmpTLI.z, tmpTLI.visible, player.z, tmpTLI.parent)
          }
        }

        onAboutToChangeMap: {
          for(var indexToDelete in temporaryObjectList)
          {
            temporaryObjectList[indexToDelete].destroy();
          }
          temporaryObjectList = [];
        }

        onMapChanged: {
          for(var prop in mapItem.customProperties) {
            console.log(prop, mapItem.customProperties[prop]);
          }

          if(mapLoader.source.toString().indexOf("sewers")>-1)
          {
            player.playerStartPos = mapItem.tileToScreenCoords(24, 36)
          }
          else
          {
            player.playerStartPos = mapItem.tileToScreenCoords(5, 23)
          }

          player.x = player.playerStartPos.x
          player.y = player.playerStartPos.y
          player.z = mapItem.z+1

          secretTileLayerItem = getTileLayerItemByName("SecretHidden");
        }
      }
    }
  }
}
