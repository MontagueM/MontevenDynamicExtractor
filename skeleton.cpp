#include "skeleton.h"

std::vector<Node*> Skeleton::parseSkeleton()
{
    getData();
    uint32_t off;
    memcpy((char*)&off, data + 0x18, 4);
    off += 0x18 + 136;

    uint32_t nodeCount;
    memcpy((char*)&nodeCount, data + off, 4);

    if (nodeCount > 1000)
    {
        printf("Skeleton broken.");
        return std::vector<Node*>();
    }
    uint32_t nodeOffset;
    memcpy((char*)&nodeOffset, data + off + 0x8, 4);
    nodeOffset += off + 0x8 + 0x10;
    uint32_t dostCount;
    memcpy((char*)&dostCount, data + off + 0x10, 4);
    uint32_t dostOffset;
    memcpy((char*)&dostOffset, data + off + 0x18, 4);
    dostOffset += off + 0x18 + 0x10;

    std::vector<Node*> nodes;
    nodes.reserve(nodeCount);

    for (int i = nodeOffset; i < nodeOffset + 0x10 * nodeCount; i += 0x10)
    {
        Node* node = new Node();
        memcpy((char*)&node->hash, data + i, 4);

        // Node name
        if (boneNames.find(node->hash) != boneNames.end())
            node->name = boneNames[node->hash];
        else
            node->name = "unk_" + std::to_string(node->hash);

        // Other data
        memcpy((char*)&node->parentNodeIndex, data + i + 4, 4);
        memcpy((char*)&node->firstChildNodeIndex, data + i + 8, 4);
        memcpy((char*)&node->nextSiblingNodeIndex, data + i + 0xC, 4);
        nodes.push_back(node);
    }

    int j = 0;
    for (int i = dostOffset; i < dostOffset + 0x20 * dostCount; i += 0x20)
    {
        float val;
        Node* node = nodes[j];
        node->dost = new DefaultObjectSpaceTransform();
        node->dost->rotation.reserve(4);
        node->dost->location.reserve(3);
        for (int k = 0; k < 4; k++)
        {
            memcpy((char*)&val, data + i + k * 4, 4);
            node->dost->rotation.push_back(val);
            if (k < 3)
            {
                memcpy((char*)&val, data + i + 0x10 + k * 4, 4);
                node->dost->location.push_back(val);
            }
        }
        memcpy((char*)&node->dost->scale, data + i + 0x1C, 4);
        j++;
    }

    // Some skeletons are given inverse data in place of non-inverse data, so we need to correct for that
    if (bDiostOnly)
    {
        for (auto& node : nodes)
        {
            std::vector<float> invRot = node->dost->rotation;
            invRot[3] *= -1; // Inverse quat
            node->dost->location.push_back(0); // Making a pure quaternion [x, y, z, 0]
            node->dost->location = quatMul(invRot, node->dost->location);
            node->dost->location = quatMul(node->dost->location, node->dost->rotation);
            node->dost->location.pop_back();
        }
    }

    return nodes;
}

std::vector<Node*> Skeleton::get()
{
    //getSkeletonNames();
    return parseSkeleton();
}

std::vector<float> quatMul(std::vector<float> q1, std::vector<float> q2)
{
    std::vector<float> ret = { 0, 0, 0, 0 };
    ret[0] = q1[0] * q2[3] + q1[1] * q2[2] - q1[2] * q2[1] + q1[3] * q2[0];
    ret[1] = -q1[0] * q2[2] + q1[1] * q2[3] + q1[2] * q2[0] + q1[3] * q2[1];
    ret[2] = q1[0] * q2[1] - q1[1] * q2[0] + q1[2] * q2[3] + q1[3] * q2[2];
    ret[3] = -q1[0] * q2[0] - q1[1] * q2[1] - q1[2] * q2[2] + q1[3] * q2[3];
    return ret;
}