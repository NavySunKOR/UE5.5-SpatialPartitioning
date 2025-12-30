// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpatialPartitioningBaseActor.h"
#include "SpatialPartitioningHashGridActor.generated.h"

/**
 * 
 */
USTRUCT()
struct FHashGridIDSet
{
	GENERATED_BODY()
public:
	FHashGridIDSet()
	{
		CenterHashGridID = FName();
		NeighbourHashGridIDList.Reserve(8);
	}
	FName CenterHashGridID;
	TSet<FName> NeighbourHashGridIDList;
};

USTRUCT()
struct FSpatialElement
{
	GENERATED_BODY()
public:
	UPROPERTY(Transient)
	TWeakObjectPtr<UStaticMeshComponent> StaticMeshComponent = nullptr;

	bool bSimulatePhysics = false;
	ECollisionEnabled::Type CollisionEnabledType = ECollisionEnabled::NoCollision;
};

USTRUCT()
struct FSpatialData
{
	GENERATED_BODY()
public:
	TArray<FSpatialElement> StaticMeshHashData;
};

USTRUCT()
struct FSpatialDynamicActor
{
	GENERATED_BODY()
public:
	TWeakObjectPtr<AActor> Actor;
	FHashGridIDSet AreaHashID;
};

UCLASS()
class SPATIALPARTITIONING_API ASpatialPartitioningHashGridActor : public ASpatialPartitioningBaseActor
{
	GENERATED_BODY()
	
protected:
	virtual void InitStaticMeshComponents() override;
	virtual void UpdatePartitioningState() override;
	FName GetAreaHashID(const FVector& InLocation) const;
	TSet<FName> GetNeighbourAreaHashIDList(FName InCenterHash) const;

	void UpdateAreaStaticMeshComponents(const FName& InAreaID, const bool bCollisionEnable);

public:
	void RegisterDynamicActors(AActor* InActor);

protected:
	UPROPERTY(VisibleAnywhere,Transient)
	TMap<FName, FSpatialData> StaticMeshHashData; //TMap<AreaHashID,component>

	TArray<FSpatialDynamicActor> DynamicActors; // 동적 오브젝트들이 있다면, 이 동적 오브젝트들이 있는 위치의 Hash Grid 관련 Component들 또한 활성화 필요

	FHashGridIDSet PlayerAreaHashID;
};
