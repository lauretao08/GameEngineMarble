#include "scenegraph.h"
#include <iostream>

SceneGraph::SceneGraph()
{
    rootdefined = false;
}

void SceneGraph::AddRoot(SceneGraphNode root, SceneGraphNode *addrRoot)
{
    if(rootdefined){
        printf("[SceneGraph::AddRoot] WARNING : Redefining scene root");
    }
    this->root=root;
    graph.push_back(root);
    this->addrRoot = addrRoot;
    addrGraph.push_back(addrRoot);
    rootdefined = true;
}

void SceneGraph::AddNode(SceneGraphNode node, SceneGraphNode *addrNode)
{
    graph.push_back(node);
    addrGraph.push_back(addrNode);
}

void SceneGraph::addRotation(int objectID,QQuaternion rotation)
{
    Transform t = graph[objectID].getTransform();
    t.setRotation(rotation);
    graph[objectID].setTransform(t);
}

SceneGraphNode SceneGraph::getNode(int objectID){
    return graph[objectID];
}

void SceneGraph::displaySceneElements(QOpenGLShaderProgram *program, GeometryEngine *geometries, QMatrix4x4 projection, QQuaternion rotation)
{
    if(graph.size()<1){
        std::cout << "[SceneGraph::displaySceneElements] ERROR : Empty Graph !" << std::endl;
    }
    if(!rootdefined){
        std::cout << "[SceneGraph::displaySceneElements] ERROR : Undefined Graph Root !" << std::endl;
    }
    SceneGraphNode current = root;
    SceneGraphNode *currentAddr = addrRoot;
    std::vector<int> currentOrderedChildren;
    currentOrderedChildren.push_back(0);

    bool ongoing=true;

    QMatrix4x4 matrix;

    while(ongoing){
        matrix.setToIdentity();
        matrix.translate(0.0, 0.0, -5.0);
        matrix.rotate(rotation);

        matrix.translate(current.getTransform().getTranslation());
        matrix.rotate(current.getTransform().getRotation());
        matrix.scale(current.getTransform().getScaling());

        // Set modelview-projection matrix
        program->setUniformValue("mvp_matrix", projection * matrix);

        switch (current.getType()) {
            case objectType::CUBE :
            case objectType::SPHERE :
                geometries->drawGeometry(current.getType(),program);
                break;
            case objectType::UNDEF :
            default :
                break;
        }



        for(unsigned int i=0;i<graph.size();i++){
            //std::cout <<"current addr : " << currentAddr << ", graph["<< i<<"].getParent() : " << graph[i].getParent() << std::endl;
            if(graph[i].getParent()==currentAddr){
                currentOrderedChildren.push_back(i);
            }
        }

        currentOrderedChildren.erase(currentOrderedChildren.begin());
        //then remove current from remaining nodes to render, and pick the next one
        if(currentOrderedChildren.size()>=1){
            current = graph[currentOrderedChildren[0]];
            currentAddr = addrGraph[currentOrderedChildren[0]];
        }else{
            ongoing=false;
        }

    }



}
