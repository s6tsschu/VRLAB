// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include "customStructs.h"
/**
 * 
 */

class VR_API Parser
{
	public:
		Parser();
		static std::vector < std::pair<float, std::map<std::string, FTransObject>>> parseXml(std::string url);
		~Parser();
	private:
		static void parseRotTransScale(std::ifstream & File, FTransObject & obj);
		static void parseRotation(std::ifstream & File, FTransObject & obj);
		static void parseTranslation(std::ifstream & File, FTransObject & obj);
		static void parseScale(std::ifstream & File, FTransObject & obj);
		static void parseXYZ(std::ifstream & File, float & x, float & y, float & z);
		static void parseXYZW(std::ifstream & File, float & x, float & y, float & z, float & w);
};
