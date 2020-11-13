#include "scenegraph.h"
#include <iostream>

SceneGraph::SceneGraph()
{

}

void SceneGraph::AddRoot(SceneGraphNode root, SceneGraphNode *addrRoot)
{
    this->root=root;
    graph.push_back(root);
    this->addrRoot = addrRoot;
    addrGraph.push_back(addrRoot);
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

void SceneGraph::displaySceneElements(QOpenGLShaderProgram *program, GeometryEngine *geometries, QMatrix4x4 projection, QQuaternion rotation)
{
    if(graph.size()<1){
        std::cout << "ERROR : GRAPH MUST CONTAIN ELEMENTS, AND THE ROOT NEEDS TO BE THE FIRST ELEMENT" << std::endl;
    }
    SceneGraphNode current = root;
    SceneGraphNode *currentAddr = addrRoot;
    std::vector<int> currentOrderedChildren;
    currentOrderedChildren.push_back(0);

    bool ongoing=true;


    QMatrix4x4 matrix;
    //QQuaternion rotation;
    //QMatrix4x4 projection;
    //matrix.translate(0.0, 0.0, -5.0);
    matrix.rotate(rotation);



    // Set modelview-projection matrix
    program->setUniformValue("mvp_matrix", projection * matrix);

    //std::cout <<"(IN FUNC : )&ROOT : " << &root << ", earth.parent() : " << graph[1].getParent() << std::endl;

    while(ongoing){
        //start by rendering the current scene object (for now, it always is a cube)
        matrix.translate(current.getTransform().getTranslation());
        matrix.rotate(current.getTransform().getRotation());
        matrix.scale(current.getTransform().getScaling());

        // Set modelview-projection matrix
        program->setUniformValue("mvp_matrix", projection * matrix);


        geometries->drawCubeGeometry(program);


        for(int i=0;i<graph.size();i++){
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
