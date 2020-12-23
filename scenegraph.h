#include "scenegraphnode.h"
#include <vector>
#include <QMatrix4x4>
#include <QQuaternion>
#include <cmath>

#ifndef SCENEGRAPH_H
#define SCENEGRAPH_H

#define SPHERE_NODE_ID 1
#define CUBE_NODE_ID 2

#define MAIN_NODE_ID SPHERE_NODE_ID

class SceneGraph{
private:

    SceneGraphNode root;
    SceneGraphNode *addrRoot;
    std::vector<SceneGraphNode> graph;
    std::vector<SceneGraphNode*> addrGraph;

    bool rootdefined;

public:
    SceneGraph();

    void AddRoot(SceneGraphNode root,SceneGraphNode *addrRoot);
    void AddNode(SceneGraphNode node, SceneGraphNode *addrNode);

    SceneGraphNode getNode(int);

    void addTranslation(int objectID,Translation translation);
    void addRotation(int objectID,Rotation rotation);

    void displaySceneElements(QOpenGLShaderProgram *program,GeometryEngine *geometries, QMatrix4x4 projection, Rotation rotation);

    void manageCollision();
    bool isColliding(int id_a,int id_b);
};

#endif // SCENEGRAPH_H
