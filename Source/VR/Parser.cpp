// Fill out your copyright notice in the Description page of Project Settings.

#include "VR.h"
#include "Parser.h"

#include <algorithm>

Parser::Parser()
{
}

std::vector<std::pair<float, std::map<std::string, FTransObject>>> Parser::parseXml(std::string url)
{
	std::vector<std::pair<float, std::map<std::string, FTransObject>>> animation;
	std::vector<float> test;
	std::ifstream File(url);
	std::string time("\"time\": ");
	std::string leftHand("\"LeftHand\"");
	std::string rightHand("\"RightHand\"");
	std::string hmd("\"HMD\"");
	int offsetTime = 3;
	
	if (File.is_open()) {
		std::string line;
		std::getline(File, line);
		while (!File.eof())
		{

			size_t dt =  line.find(time);
			if (dt != std::string::npos)
			{
				size_t numberEnd = line.find(",");
				int numberSize = numberEnd - (dt + time.length());
				std::string timeStamp = line.substr(dt + time.length() , numberSize);
				float timeValue = atof(timeStamp.c_str());
				getline(File, line);
				std::map<std::string, FTransObject> map;
				while (line.find("]") == std::string::npos) {
					if (line.find(leftHand) != std::string::npos) {
						FTransObject obj;
						parseRotTransScale(File, obj);
						map["LeftHand"] = obj;
					}
					if (line.find(rightHand) != std::string::npos) {
						FTransObject obj;
						parseRotTransScale(File, obj);
						map["RightHand"] = obj;
					}
					if (line.find(hmd) != std::string::npos) {
						FTransObject obj;
						parseRotTransScale(File, obj);
						map["HMD"] = obj;
					}
					getline(File, line);
				}
				animation.push_back(std::pair<float, std::map<std::string, FTransObject>>(timeValue, map));

			}
			getline(File, line);
		}
	}
	return animation;
}


Parser::~Parser()
{
}

void Parser::parseRotTransScale(std::ifstream & File, FTransObject & obj)
{
	std::string line;
	getline(File, line);
	while (line.find("{") == std::string::npos) {
		getline(File, line);
	}
	while (line.find("}") == std::string::npos) {
		if (line.find("rotation") != std::string::npos) {
			parseRotation(File, obj);
		}
		if (line.find("translation") != std::string::npos) {
			parseTranslation(File, obj);
		}
		if (line.find("scale3D") != std::string::npos) {
			parseScale(File, obj);
		}
		getline(File, line);
	}
}

void Parser::parseRotation(std::ifstream & File, FTransObject & obj)
{
	parseXYZW(File, obj.rotX, obj.rotY, obj.rotZ,obj.rotW);

}

void Parser::parseTranslation(std::ifstream & File, FTransObject & obj)
{
	parseXYZ(File, obj.transX, obj.transY, obj.transZ);
}

void Parser::parseScale(std::ifstream & File, FTransObject & obj)
{
	parseXYZ(File, obj.scaleX, obj.scaleY, obj.scaleZ);
}

void Parser::parseXYZ(std::ifstream & File, float & x, float & y, float & z)
{
	std::string line;
	std::string X("x\": "), Y("y\": "), Z("z\": ");
	getline(File, line);
	while (line.find("}") == std::string::npos) {
		size_t pos = line.find("x");
		if (pos != std::string::npos) {
			int length = line.find(",") - (pos + X.length());
			std::string value = line.substr(pos + X.length(), length);
			x = std::atof(value.c_str());
		}
		pos = line.find("y");
		if (pos != std::string::npos) {
			
			int length = line.find(",") - (pos + Y.length());
			std::string value = line.substr(pos + Y.length(), length);
			y = std::atof(value.c_str());
		}
		pos = line.find("z");
		if (pos != std::string::npos) {
			std::string value = line.substr(pos + Z.length());
			value.erase(std::remove(value.begin(), value.end(), ','), value.end());
			value.erase(std::remove(value.begin(), value.end(), '\r'), value.end());
			z = std::atof(value.c_str());
		}
		getline(File, line);
	}
}
void Parser::parseXYZW(std::ifstream & File, float & x, float & y, float & z, float & w) {
	std::string line;
	std::string X("x\": "), Y("y\": "), Z("z\": "), W("w\": ");
	getline(File, line);
	while (line.find("}") == std::string::npos) {
		size_t pos = line.find("x");
		if (pos != std::string::npos) {
			int length = line.find(",") - (pos + X.length());
			std::string value = line.substr(pos + X.length(), length);
			x = std::atof(value.c_str());
		}
		pos = line.find("y");
		if (pos != std::string::npos) {

			int length = line.find(",") - (pos + Y.length());
			std::string value = line.substr(pos + Y.length(), length);
			y = std::atof(value.c_str());
		}
		pos = line.find("z");
		if (pos != std::string::npos) {
			std::string value = line.substr(pos + Z.length());
			value.erase(std::remove(value.begin(), value.end(), ','), value.end());
			value.erase(std::remove(value.begin(), value.end(), '\r'), value.end());
			z = std::atof(value.c_str());
		}
		pos = line.find("w");
		if (pos != std::string::npos) {
			std::string value = line.substr(pos + W.length());
			value.erase(std::remove(value.begin(), value.end(), ','), value.end());
			value.erase(std::remove(value.begin(), value.end(), '\r'), value.end());
			w = std::atof(value.c_str());
		}
		getline(File, line);
	}

}