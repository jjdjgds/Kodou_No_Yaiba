#include "stdafx.h"
#include "MapLoader.hpp"


bool MapLoader::LoadMap(const FilePath& path, Array<int>& outMap, int& outWidth, int& outHeight)
{
	//Print << U"🔍 Loading map from: " << path;

	TextReader reader(path);
	if (!reader.isOpen())
	{
		return false;
	}
	Optional<String> optLine = reader.readLine();
	if (!optLine)
	{
		Print << U"❌ Map file is empty.";
		return false;
	}
	String firstLine = *optLine;
	Array<String> headerTokens = firstLine.split(U' ');

	if (headerTokens.size() < 2)
	{
		Print << U"❌ Invalid header format in map file.";
		return false;
	}
	outWidth = Parse<int>(headerTokens[0]);
	outHeight = Parse<int>(headerTokens[1]);

	for (int y = 0; y < outHeight; ++y)
	{
		Optional<String> lineOpt = reader.readLine();
		if (!lineOpt)
		{
			Print << U"❌ Unexpected end of file at row " << y;
			return false;
		}
		Array<String> tokens = lineOpt->split(U' ');
		for (int x = 0; x < outWidth && x < tokens.size(); ++x)
		{
			outMap << Parse<int>(tokens[x]);
		}
	}
	//Print << U"✅ Map loaded: " << path;
	return true;
}
