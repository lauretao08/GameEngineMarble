#include "scenegraphnode.h"
#include <vector>
#include <QMatrix4x4>
#include <QQuaternion>

#ifndef SCENEGRAPH_H
#define SCENEGRAPH_H


class SceneGraph
{

private:

    SceneGraphNode root;
    SceneGraphNode *addrRoot;
    std::vector<SceneGraphNode> graph;
    std::vector<SceneGraphNode*> addrGraph;

public:
    SceneGraph();

    void AddRoot(SceneGraphNode root,SceneGraphNode *addrRoot);
    void AddNode(SceneGraphNode node, SceneGraphNode *addrNode);

    void addRotation(int objectID,QQuaternion rotation);


    void displaySceneElements(QOpenGLShaderProgram *program,GeometryEngine *geometries, QMatrix4x4 projection, QQuaternion rotation);
};

#endif // SCENEGRAPH_H
