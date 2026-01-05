// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseAlgorithmTestActor.generated.h"

UCLASS()
class SPATIALPARTITIONING_API ABaseAlgorithmTestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseAlgorithmTestActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
#if WITH_EDITOR
	UFUNCTION(CallInEditor)
	void TestAlgorithm();
#endif

	virtual void RunAlgorithm() PURE_VIRTUAL(ABaseAlgorithmTestActor::RunAlgorithm,);


};
