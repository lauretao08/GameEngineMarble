#include "scenegraphnode.h"

SceneGraphNode::SceneGraphNode()
{

}

SceneGraphNode::SceneGraphNode(SceneGraphNode *parent)
{
    this->parent = parent;
}

void SceneGraphNode::setTransform(Transform t)
{
    transform=t;
}

Transform SceneGraphNode::getTransform()
{
    return transform;
}

SceneGraphNode *SceneGraphNode::getParent()
{
    return parent;
}



