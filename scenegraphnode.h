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


class SceneGraphNode
{

private:
    SceneGraphNode* parent;
    Transform transform;
    objectType type;

public:
    SceneGraphNode();

    SceneGraphNode(SceneGraphNode* parent, objectType type);

    void setTransform(Transform t);
    void addTransform(Transform t);

    Transform getTransform();

    SceneGraphNode *getParent();


    objectType getType() const;
};

#endif // SCENEGRAPHNODE_H
