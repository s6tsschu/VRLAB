// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "Parser.h"
#include <glm/glm.hpp>
#include <queue>
#include "customStructs.h"
#include "CharacterMain.generated.h"





UCLASS()
class VR_API ACharacterMain : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterMain();
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	
	
	std::vector<std::pair<float, std::map<std::string, FTransObject>>> data;

	/*HANDL HANDR HMD DATA*/
	UPROPERTY(BlueprintReadWrite)
	FTransform headT;
	UPROPERTY(BlueprintReadWrite)
	FTransform handLT;
	UPROPERTY(BlueprintReadWrite)
	FTransform handRT;

	/*Vector*/
	UPROPERTY(BlueprintReadWrite)
		FVector elbowVectorR;
	UPROPERTY(BlueprintReadWrite)
		FVector elbowVectorL;
	UPROPERTY(BlueprintReadWrite)
		FVector wristVectorR;
	UPROPERTY(BlueprintReadWrite)
		FVector wristVectorL;
	UPROPERTY(BlueprintReadWrite)
		FVector shoulderVectorR;
	UPROPERTY(BlueprintReadWrite)
		FVector shoulderVectorL;

	/*Iverse Kinematic**/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector forwardDirectionHeadAverage = FVector(0, 0, 0);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector startPosition = FVector(0, 0, 0);
	UPROPERTY(BlueprintReadWrite)
	int currentFrame = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTPose tPose;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector lowerArm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector upperArm;
	UFUNCTION(BlueprintCallable, Category = CharacterFunction)
	static FTransform FTransObjectToFTransform(UPARAM(ref) FTransObject & objectToParse);
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector debugJacobi;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector debugJacobi2;
	
	
	UPROPERTY(BlueprintReadWrite)
	float distanceDebugJacobi;
	UPROPERTY(BlueprintReadWrite)
	FVector distanceJacobi;
	UPROPERTY(BlueprintReadWrite)
	FVector4 changeJacobi;
	FTransform makeTransform(FTransObject & obj);
private:
	void setForwardDirectionHead(FTransform & currentTrans);
	void setForwardDirectionArms(FTransform & leftHand, FTransform & rightHand);
	bool jacobianDistance(glm::vec3 currentPosition, glm::vec3 goalPosition, std::vector<BoneDebug> &bones);
	glm::mat4 getElbowTransformMatrix(const float o0, const float o1, const float o2, const float o3);
	glm::mat4 getHandTransformMatrix(const float o4);
	glm::mat3 pseudoInverse(const glm::mat3 & jacobianMatrix);
	glm::mat3x4 pseudoInverse(const glm::mat4x3 & jacobianMatrix);
	FVector toFVector(const glm::vec3 v);
	FVector4 toFVector4(const glm::vec4 v);
	glm::vec3 toGLMVector(const FVector & v);
	void setJointAngles(glm::vec3 currentPosition, glm::vec3 goalPosition);
	void setJointAnglesFull(glm::vec3 currentPosition, glm::vec3 goalPosition);
	void clampVector(glm::vec3 & vector);
	
	glm::mat3x3 currentJacobian;
	glm::mat4x3 fullJacobian;
	BoneDebug b0;
	BoneDebug b1;
	BoneDebug b2;
	BoneDebug b3;
	BoneDebug b4;
	float debugAngle = 180.0f;
	float debugAngle2 = 0.0f;
	glm::vec3 difference;


	void updateRotation();
	glm::mat3x3 getJacobian(float q1, float q2, float q3);
	glm::mat4x3 getJacobian(float q1, float q2, float q3, float q4);
	void calculateTPose();
	YPR rotationLowerArm;
	YPR rotationUpperArm;
	
	std::queue<FVector> forwardDirectionHead;
	FVector forwardVec = FVector(1, 0, 0);
	int BufferSize = 50;

};
