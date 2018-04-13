import QtQuick 2.0
import Box2D 2.0

PhysicsItem {
  id: wall
  Rectangle {color: parent.color; anchors.fill: parent }

  fixtures: Box {
    width: wall.width
    height: wall.height
    density: 5
    friction: 0.0
    restitution: 0.0
  }

  entityType: type

  property int objectId;
  property string name;
  property string type;
  property bool persistent;
  property color color;
  //Component.onDestruction: console.log("DESTROYED!!")
}
