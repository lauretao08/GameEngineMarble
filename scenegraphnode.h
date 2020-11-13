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

public:
    SceneGraphNode();

    SceneGraphNode(SceneGraphNode* parent);

    void setTransform(Transform t);

    Transform getTransform();

    SceneGraphNode *getParent();


};

#endif // SCENEGRAPHNODE_H
