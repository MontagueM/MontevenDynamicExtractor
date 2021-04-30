#pragma once
#pragma comment(lib, "libfbxsdk.lib")
#include <fbxsdk.h>
#include <vector>
#include <fstream>
#include <unordered_map>
#include "helpers.h"

struct DefaultObjectSpaceTransform
{
	std::vector<float> rotation;
	std::vector<float> location;
	float scale;
};


struct DefaultInverseObjectSpaceTransform : public DefaultObjectSpaceTransform
{
};


class Node
{
private:
public:
	uint32_t hash;
	std::string name;
	int32_t parentNodeIndex;
	int32_t firstChildNodeIndex;
	int32_t nextSiblingNodeIndex;
	DefaultObjectSpaceTransform* dost;
	DefaultInverseObjectSpaceTransform* diost;
	FbxNode* fbxNode = nullptr;
};

class Skeleton : public File
{
private:
	std::unordered_map<uint32_t, std::string> boneNames;

	void getSkeletonNames();
	std::vector<Node*> parseSkeleton();
public:
	Skeleton(std::string x, std::string pkgsPath) : File(x, pkgsPath) {};

	std::vector<Node*> get();
};
