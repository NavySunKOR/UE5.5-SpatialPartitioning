// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPBaseActor.h"
#include "SPHashGridActor.generated.h"

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
class SPATIALPARTITIONING_API ASPHashGridActor : public ASPBaseActor
{
	GENERATED_BODY()
	
public:
	ASPHashGridActor();

protected:
	void InitGridData();
	virtual void InitProperties() override;
	virtual void InitStaticMeshComponents() override;
	virtual void UpdatePartitioningState() override;
	virtual void DrawDebugObjects() override;

	void UpdateAreaStaticMeshComponents(const FName& InAreaID, const bool bCollisionEnable);

	FName GetAreaHashID(const FVector& InLocation) const;
	FIntPoint GetAreaHashIDIntPoint(const FVector& InLocation) const;
	FName MakeAreaHashID(const FIntPoint& InAreaHashPoint) const;
	TSet<FName> GetNeighbourAreaHashIDList(FName InCenterHash) const;

public:
	void RegisterDynamicActors(AActor* InActor);

protected:
	UPROPERTY(VisibleAnywhere,Transient)
	TMap<FName, FSpatialData> StaticMeshHashData; //TMap<AreaHashID,component>

	UPROPERTY(EditAnywhere)
	int32 GridCountX;

	UPROPERTY(EditAnywhere)
	int32 GridCountY;

	TArray<FSpatialDynamicActor> DynamicActors; // 동적 오브젝트들이 있다면, 이 동적 오브젝트들이 있는 위치의 Hash Grid 관련 Component들 또한 활성화 필요

	FHashGridIDSet PlayerAreaHashID;
};
