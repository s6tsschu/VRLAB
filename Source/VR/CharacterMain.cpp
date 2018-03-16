// Fill out your copyright notice in the Description page of Project Settings.

#include "VR.h"
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include "Logging.h"
#include "CharacterMain.h"
#define squared(x)	powf(x,2)
#define Lambda	0.04f


// Sets default values
ACharacterMain::ACharacterMain()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	data = Parser::parseXml("C:/Users/Thomas/Desktop/uni/MASTER!/Lab/AufnahmenUE4/AufnahmenUE4/TiPose");
	
	
	b0 = BoneDebug(PI/2.0f, 0, 0,PI/2);
	b1 = BoneDebug(-PI/2.0f, 0, 0, -PI/2.0f);
	b2 = BoneDebug(-PI/2.0f, 50.0f, 0, PI/2.0f);
	b3 = BoneDebug(PI, 0,0, -PI/2.0f);
	b4 = BoneDebug(0, -50.0f, 0, 0);
	

	currentJacobian = getJacobian(b0.omega, b1.omega,b2.omega);
	fullJacobian = getJacobian(0, 0, 0, 0);
}


// Called when the game starts or when spawned
void ACharacterMain::BeginPlay()
{
	calculateTPose();
	Super::BeginPlay();
	
}

// Called every frame
void ACharacterMain::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	/*elapsedTime += DeltaTime;
	
	int next = currentFrame + 1;
	if (next >= data.size()) {
		next = 0;
		elapsedTime -= data[data.size() - 1].first;
	}
	while (data[next].first <= elapsedTime) {
		next++;
		if (next >= data.size()) {
			next = 0;
			elapsedTime -= data[data.size() - 1].first;
		}
	}
	if (next == 0) {
		currentFrame = 0;
	}
	else {
		currentFrame = next - 1;
	}*/

	
	
	std::map<std::string,FTransObject> current = data[currentFrame].second;
	this->headT = makeTransform(current["HMD"]);
	setForwardDirectionHead(this->headT);
	this->handLT = makeTransform(current["LeftHand"]);
	this->handRT = makeTransform(current["RightHand"]);
	updateRotation();
	currentFrame++;
}

// Called to bind functionality to input
void ACharacterMain::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

FTransform ACharacterMain::makeTransform(FTransObject & obj)
{
	return FTransform(FRotator(FQuat(obj.rotX, obj.rotY, obj.rotZ, obj.rotW)), FVector(obj.transX, obj.transY, obj.transZ));
}




void ACharacterMain::setForwardDirectionHead(FTransform & currentTrans)
{

	
	if (forwardDirectionHead.size() == 50){
		forwardDirectionHeadAverage = (forwardDirectionHeadAverage - forwardDirectionHead.front())/(forwardDirectionHeadAverage - forwardDirectionHead.front()).Size();
		forwardDirectionHead.pop();

	}
	FVector currentVec = FVector::VectorPlaneProject(currentTrans.Rotator().RotateVector(forwardVec),FVector(0,0,1)) / BufferSize;
	forwardDirectionHead.push(currentVec);
	forwardDirectionHeadAverage = (forwardDirectionHeadAverage + forwardDirectionHead.back())/(forwardDirectionHeadAverage + forwardDirectionHead.back()).Size();
}

bool ACharacterMain::jacobianDistance(glm::vec3 currentPosition, glm::vec3 goalPosition, std::vector<BoneDebug> &bones)
{
	glm::mat4 r1 = bones.at(0).worldTransformationMatrix;
	glm::mat4 r2 = bones.at(1).worldTransformationMatrix;
	glm::mat4 r3 = bones.at(2).worldTransformationMatrix;
	currentPosition = glm::vec3(r1*r2*r3 * glm::vec4(currentPosition, 1.0f));
	return false;
}

glm::mat4 ACharacterMain::getElbowTransformMatrix(const float o0, const float o1, const float o2, const float o3)
{
	return glm::rotate(o0, glm::vec3(0, 0, 1)) * glm::rotate(o1, glm::vec3(0, 1, 0)) * glm::rotate(o2, glm::vec3(1, 0, 0)) * glm::translate(glm::vec3(100, 0, 0)) * glm::rotate(o3, glm::vec3(0, 0, 1));
}

glm::mat4 ACharacterMain::getHandTransformMatrix(const float o4)
{
	return glm::translate(glm::vec3(100, 0, 0));
}

glm::mat3 ACharacterMain::pseudoInverse(const glm::mat3 & jacobianMatrix)
{
	glm::mat3 jacobianPseudo = glm::mat3(1.0f);
	glm::mat3 jacobianTransposed = glm::transpose(jacobianMatrix);
	if (glm::determinant(jacobianMatrix) != 0) {
		jacobianPseudo = glm::inverse(jacobianTransposed * jacobianMatrix ) * jacobianTransposed;
	}
	else if (glm::determinant(jacobianTransposed) != 0) {
		jacobianPseudo = jacobianTransposed * glm::inverse(jacobianMatrix * jacobianTransposed );
	}
	
	
	return jacobianPseudo;
}

glm::mat3x4 ACharacterMain::pseudoInverse(const glm::mat4x3 & jacobianMatrix)
{
	glm::mat3x4 jacobianPseudo = glm::mat3x4(1.0f);
	glm::mat3x4 jacobianTransposed = glm::transpose(jacobianMatrix);
	
	jacobianPseudo = glm::inverse(jacobianTransposed * jacobianMatrix + 0.03f*glm::mat4(1.0f) )* jacobianTransposed;
	
	
	return jacobianPseudo;
}

FVector ACharacterMain::toFVector(const glm::vec3 v)
{	
	return FVector(v.x, v.y, v.z);
}

FVector4 ACharacterMain::toFVector4(const glm::vec4 v)
{
	return FVector4(v.x,v.y,v.z,v.w);
}

glm::vec3 ACharacterMain::toGLMVector(const FVector & v)
{
	return glm::vec3((float)v.X,(float)v.Y,(float)v.Z);
}

void ACharacterMain::setJointAngles(glm::vec3 currentPosition, glm::vec3 goalPosition)
{

	glm::vec3 p = toGLMVector(toFVector(goalPosition) - debugJacobi);
	
	//distanceJacobi = FVector(p.x, p.y, p.z);
	//glm::mat3x4 jacobianPseudo = pseudoInverse(currentJacobian);
	//Logging::printVector(p);
	glm::mat3 jacobianPseudo = pseudoInverse(currentJacobian);
	difference = 0.02f * jacobianPseudo * p;
	distanceJacobi = toFVector(difference);
	
	//distanceDebugJacobi = (goalPosition - glm::vec3(newPosition1)).length();
	b0.increaseOmega(difference[0]);
	b1.increaseOmega(difference[1]);
	b2.increaseOmega(difference[2]);
	//currentJacobian = getJacobian(b0.omega,b1.omega,b2.omega);

	glm::mat4 to3 = b0.worldTransformationMatrix * b1.worldTransformationMatrix * b2.worldTransformationMatrix;
	glm::vec4 newPosition1 = to3 * glm::vec4(currentPosition, 1.0f);
	glm::vec3 newPosition = glm::vec3(newPosition1);
	
	
	distanceDebugJacobi = glm::length(goalPosition - newPosition);
	debugJacobi = toFVector(newPosition);
	//currentJacobian = getJacobian(b0.omega, b1.omega, b2.omega);
	
		
	
	/*
	*/
	
	

}

void ACharacterMain::setJointAnglesFull(glm::vec3 currentPosition, glm::vec3 goalPosition)
{
	for (int i = 0; i < 200; i++)
	{
		glm::vec3 p = toGLMVector(toFVector(goalPosition) - debugJacobi2);
		glm::mat3x4 jacobianPseudo = pseudoInverse(fullJacobian);
		glm::vec4 change = jacobianPseudo * p;
		changeJacobi = toFVector4(change);
		b0.increaseOmega(change[0]);
		b1.increaseOmega(change[1]);
		b2.increaseOmega(change[2]);
		b3.increaseOmega(change[3]);

		glm::mat4 to4 = b0.worldTransformationMatrix * b1.worldTransformationMatrix * b2.worldTransformationMatrix *b3.worldTransformationMatrix * b4.worldTransformationMatrix;
		glm::vec4 newPosition1 = to4 * glm::vec4(currentPosition, 1.0f);
		glm::vec3 newPosition = glm::vec3(newPosition1);


		distanceDebugJacobi = glm::length(goalPosition - newPosition);
		debugJacobi2 = toFVector(newPosition);
		//currentJacobian = getJacobian(b0.omega, b1.omega, b2.omega);
		if (distanceDebugJacobi <= 4) {

			debugAngle += 2;
			
			//debugGoal = FVector(goalPosition.x, goalPosition.y, goalPosition.z);
			fullJacobian = getJacobian(0, 0, 0, 0);
			break;
		}
		
	}
	if (debugAngle >= 360) {
		debugAngle = (int)debugAngle % 360;
	}
	debugAngle += 0.3;
	debugAngle2 =+ 0.4f;
	
}

void ACharacterMain::clampVector(glm::vec3 & vector)
{
	if (glm::length(vector) > 99.0f) {
		vector = 99.0f * glm::normalize(vector);
	}
}

void ACharacterMain::updateRotation()
{
	
	
	/***************************************/
	glm::vec4 hand = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::mat4 elbowMatrix = b0.worldTransformationMatrix * b1.worldTransformationMatrix * b2.worldTransformationMatrix;
	glm::vec4 elbowPosition = elbowMatrix *  glm::vec4(glm::vec3(0.0f),1.0f);
	debugJacobi = FVector(elbowPosition.x, elbowPosition.y, elbowPosition.z);
	debugJacobi2 = toFVector(glm::vec3(elbowMatrix * b3.worldTransformationMatrix* b4.worldTransformationMatrix *  hand));
	
	
	
	//setJointAngles(hand,glm::vec3(nP));
	//setJointAnglesFull(glm::vec3(hand), toGLMVector(this->hand_lT.GetTranslation()));
	
}

glm::mat3x3 ACharacterMain::getJacobian(float q1, float q2, float q3)
{
	
	glm::mat4 a2 = b0.worldTransformationMatrix * b1.worldTransformationMatrix;
	glm::mat4 a3 = a2 * b2.worldTransformationMatrix;
	glm::vec3 transPart = glm::vec3(glm::column(a3,3));

	glm::vec3 vX = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), transPart);
	glm::vec3 vY = glm::cross(glm::mat3(b0.worldTransformationMatrix) * glm::vec3(0.0f, 0.0f, 1.0f), transPart);
	glm::vec3 vZ = glm::cross(glm::mat3(a2)*glm::vec3(0.0f, 0.0f, 1.0f), transPart);
	return glm::mat3x3(vX,vY,vZ);
}

glm::mat4x3 ACharacterMain::getJacobian(float q1, float q2, float q3, float q4)
{
	glm::mat4 a2 = b0.worldTransformationMatrix * b1.worldTransformationMatrix;
	glm::mat4 a3 = a2 * b2.worldTransformationMatrix;
	glm::mat4 a4 = a3 * b3.worldTransformationMatrix * b4.worldTransformationMatrix;
	glm::vec3 transPartEnd = glm::vec3(glm::column(a4, 3));
	glm::vec3 transPartA3 = glm::vec3(glm::column(a3, 3));
	glm::vec3 vX = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), transPartEnd);
	glm::vec3 vY = glm::cross(glm::mat3(b0.worldTransformationMatrix) * glm::vec3(0.0f, 0.0f, 1.0f), transPartEnd);
	glm::vec3 vZ = glm::cross(glm::mat3(a2)*glm::vec3(0.0f, 0.0f, 1.0f), transPartEnd);
	glm::vec3 vW = glm::cross(glm::mat3(a3)*glm::vec3(0.0f, 0.0f, 1.0f), transPartEnd - transPartA3);
	
	return glm::mat4x3(vX,vY,vZ,vW);
	
}

void ACharacterMain::calculateTPose()
{
	
	if (data.size() == 0) {
		return;
	}
	/*index of tPose*/
	int index = 0;
	/*calculate max distance between both hands = TPose*/
	float maxHandDistance = 0;
	for (int i = 0; i < data.size(); i++)
	{
		std::map<std::string, FTransObject> current = data.at(i).second;
		FTransObject * leftHand = &current["LeftHand"];
		FTransObject * rightHand = &current["RightHand"];

		FVector leftPos = FVector(leftHand->transX, leftHand->transY, leftHand->transZ);
		FVector rightPos = FVector(rightHand->transX, rightHand->transY, rightHand->transZ);
		float distance = (leftPos - rightPos).Size();
		if (distance > maxHandDistance) {
			index = i;
			maxHandDistance = distance;
		}
		
	}
	std::map<std::string, FTransObject> tPoseData = data.at(index).second;
	tPose.leftHand.parseObject(tPoseData["LeftHand"]);
	tPose.rightHand.parseObject(tPoseData["RightHand"]);
	tPose.head.parseObject(tPoseData["HMD"]);
	tPose.frameIndex = index;
}

FTransform ACharacterMain::FTransObjectToFTransform(FTransObject & objectToParse)
{
	FTransform trans(FQuat(objectToParse.rotX, objectToParse.rotY, objectToParse.rotZ, objectToParse.rotW),FVector(objectToParse.transX, objectToParse.transY, objectToParse.transZ));
	return trans;
}

