// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPBaseActor.generated.h"

UCLASS(Abstract)
class SPATIALPARTITIONING_API ASPBaseActor : public AActor
{
	GENERATED_BODY()
public:	
	// Sets default values for this actor's properties
	ASPBaseActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Removed SkeletalMeshes because they can move.
	virtual void InitStaticMeshComponents() PURE_VIRTUAL(ASPBaseActor::InitStaticMeshComponents,);
	virtual void UpdatePartitioningState() PURE_VIRTUAL(ASPBaseActor::UpdatePartitioningState, );

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
#if WITH_EDITOR
	UFUNCTION(CallInEditor)
	void SetBoundsAlignActor();
#endif

	void InitGridData();
protected:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere)
	TObjectPtr<AActor> BoundSetActor;
#endif

	UPROPERTY(EditAnywhere)
	int32 GridCountX;

	UPROPERTY(EditAnywhere)
	int32 GridCountY;

	UPROPERTY(EditAnywhere)
	FVector Center;

	UPROPERTY(EditAnywhere)
	FVector Extent;

	FVector StartPos; //LeftTop
	uint32 GridWidth;
	uint32 GridHeight;

	TWeakObjectPtr<class ACharacter> PlayerChar = nullptr;
};
