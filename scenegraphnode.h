#include <vector>
#include "transform.h"
#include "geometryengine.h"
#include <QOpenGLFunctions_3_1>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <string>

using namespace std;

#ifndef SCENEGRAPHNODE_H
#define SCENEGRAPHNODE_H


class SceneGraphNode{

private:
    SceneGraphNode* parent;
    Transform transform;
    objectType type;

    bool Drawable;
    bool Collidable;


public:
    SceneGraphNode();
    SceneGraphNode(SceneGraphNode* parent, objectType type);

    objectType getType() const;
    bool isDrawable() const;
    bool isCollidable() const;

    Transform getTransform();
    SceneGraphNode *getParent();

    void setTransform(Transform t);
    void addTransform(Transform t);

    /**Force related**/
    QVector3D velocity = QVector3D(0.0,0.0,1.0);
    QVector3D gravity = QVector3D(0.0,-1.0,0.0);
    //QVector3D position;

    bool mobile;
};

#endif // SCENEGRAPHNODE_H
