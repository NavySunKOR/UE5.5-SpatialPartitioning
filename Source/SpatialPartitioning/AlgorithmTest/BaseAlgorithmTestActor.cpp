// Fill out your copyright notice in the Description page of Project Settings.


#include "AlgorithmTest/BaseAlgorithmTestActor.h"

// Sets default values
ABaseAlgorithmTestActor::ABaseAlgorithmTestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseAlgorithmTestActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseAlgorithmTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

#if WITH_EDITOR
void ABaseAlgorithmTestActor::TestAlgorithm()
{
	RunAlgorithm();
}
#endif