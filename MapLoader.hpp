#pragma once
#include <Siv3D.hpp>
#include <vector>

class MapLoader
{
public:
	static bool LoadMap(const FilePath& path, Array<int>& outMap, int& outWidth, int& outHeight);
};

