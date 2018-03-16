// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**
 * 
 */
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include "customStructs.generated.h"


/*Struct for reading Recordings*/
USTRUCT(BlueprintType)
struct FTransObject
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float transX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float transY;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float transZ;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float rotX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float rotY;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float rotZ;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float rotW;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float scaleX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float scaleY;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float scaleZ;
	void parseObject(FTransObject obj) {
		transX = obj.transX;
		transY = obj.transY;
		transZ = obj.transZ;
		rotX = obj.rotX;
		rotY = obj.rotY;
		rotZ = obj.rotZ;
		rotW = obj.rotW;
		scaleX = obj.scaleX;
		scaleY = obj.scaleY;
		scaleZ = obj.scaleZ;
	};
	FTransObject() {
		transX = transY = transZ = rotX = rotY = rotZ = rotW = 0.0f;
		scaleX = scaleY = scaleZ = 1.0f;
	};
	
	
	
};
/*TPose Struct*/
USTRUCT(BlueprintType)
struct FTPose {

	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTransObject leftHand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTransObject rightHand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTransObject head;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int frameIndex;
	FTPose() {
		leftHand = rightHand = head = FTransObject();
	}
};

struct YPR {
	float yaw = 0.0f;
	float pitch = 0.0f;
	float roll = 0.0f;
	void addYaw(float yawIncrease) {
		yaw += yawIncrease;
		if (yaw >= 360) {
			yaw = (int)yaw % 360;
		}
	}

};
struct BoneDebug {
	BoneDebug * parent = nullptr;
	BoneDebug * child = nullptr;
	glm::mat4 worldTransformationMatrix;
	float omega = 0.0f;
	float d = 0.0f;
	float a = 0.0f;
	float alpha = 0.0f;
	BoneDebug(float o, float d, float da, float A) :omega(o), d(d), a(da), alpha(A) {
		setTransformationMatrix();
	};
	BoneDebug() {};
	void setTransformationMatrix() {
		
		this->worldTransformationMatrix =  getWorldTransformationMatrix();
	}
	void setTransformationMatrix(const glm::mat4 & frame0) {

		this->worldTransformationMatrix = frame0 * getWorldTransformationMatrix();
	}
	void increaseOmega(float omega) {
		//if (omega > 2.0f * PI) {
			//omega = glm::radians(omega);
		//}
		this->omega += omega;		
		setTransformationMatrix();
	}
	glm::mat4 getWorldTransformationMatrix() {
		return glm::rotate(omega, glm::vec3(0, 0, 1)) * glm::translate(glm::vec3(0, 0, d)) * glm::translate(glm::vec3(a, 0, 0))  * glm::rotate(alpha, glm::vec3(1, 0, 0));
		//return glm::rotate(alpha, glm::vec3(1, 0, 0)) * glm::translate(glm::vec3(a, 0, 0)) * glm::translate(glm::vec3(0, 0, d))  * glm::rotate(omega, glm::vec3(0, 0, 1));
	}
	
};



