#include "scenegraphnode.h"
#include <vector>
#include <QMatrix4x4>
#include <QQuaternion>
#include <cmath>

#ifndef SCENEGRAPH_H
#define SCENEGRAPH_H

#define SPHERE_NODE_ID 1
#define CUBE_NODE_ID 2
#define VELOCITY_THRESHOLD 0.01

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

    void addForce(int objectID, QVector3D force);
    void addTranslation(int objectID,Translation translation);
    void addRotation(int objectID,Rotation rotation);
    void setVelocity(int objectID,QVector3D velocity);
    void addVelocity(int objectID,QVector3D velocity);

    void displaySceneElements(QOpenGLShaderProgram *program,GeometryEngine *geometries, QMatrix4x4 projection, Rotation rotation);

    void manageCollision();
    bool isColliding(int id_a,int id_b);

    /**Force related**/
    void updateForce(float delta_t);
    void updateCurrentTime(){previousTime=currentTime;currentTime = GetCurrentTime();}
    float previousTime = 0.0;
    float currentTime;
};

#endif // SCENEGRAPH_H
