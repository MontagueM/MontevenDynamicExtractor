#pragma once
#include <vector>
#include "helpers.h"

class Dynamic : public Model
{
private:
	std::vector<File> dyn2s;
	std::vector<File> dyn3s;
	void* fbxModel;

	void getDyn3Files();
	void parseDyn3s();
	void getSubmeshes();
	void addSubmeshToFbx();
	void createMesh();
	void save();
	std::vector<std::vector<float_t>> trimVertsData();

public:
	Dynamic(std::string x) : Model(x)
	{
	}

	void get();
	void pack();

};