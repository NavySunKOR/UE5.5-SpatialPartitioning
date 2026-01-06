// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAlgorithmTestActor.h"
#include "QuadTreeTestActor.generated.h"

struct QuadTreeNodeElement
{
public:
	FString ObjectName;
	FVector2D QTPosition;
};

class QuadTreeNode
{

public:
	QuadTreeNode(FBox2D InBound,int32 InDepth)
	{
		Bound = InBound;
		Depth = InDepth;
	}

	void InsertData(const FVector2D& InTreeElem,const int32& InMaximumDepth)
	{
		if (Bound.IsInside(InTreeElem) == false)
		{
			return;
		}

		if (Depth == InMaximumDepth)
		{
			Items.Add(InTreeElem);

			FVector2D Center;
			FVector2D Range; 
			Bound.GetCenterAndExtents(Center, Range);

			UE_LOG(LogTemp, Warning, TEXT("InTreeElem : %s"), *InTreeElem.ToString())
			UE_LOG(LogTemp,Warning,TEXT("Will be placed in : %s  / extent : %s"), *Center.ToString(),*Range.ToString())
			return;
		}

		if (NW == nullptr)
		{
			Subdivide();
		}

		NW->InsertData(InTreeElem, InMaximumDepth); // Duplicate?
		NE->InsertData(InTreeElem, InMaximumDepth);
		SW->InsertData(InTreeElem, InMaximumDepth);
		SE->InsertData(InTreeElem, InMaximumDepth);
	}

	void GetNearestElements(TArray<FVector2D>& OutData, const FVector2D& InLookAtPos, const int32& InMaximumDepth)
	{
		if (Depth < InMaximumDepth)
		{
			if (bIsSubdivided)
			{
				NW->GetNearestElements(OutData, InLookAtPos, InMaximumDepth); // Duplicate?
				NE->GetNearestElements(OutData, InLookAtPos, InMaximumDepth);
				SW->GetNearestElements(OutData, InLookAtPos, InMaximumDepth);
				SE->GetNearestElements(OutData, InLookAtPos, InMaximumDepth);
			}
			return;
		}

		if (Bound.IsInside(InLookAtPos) )
		{
			for (const FVector2D& Item : Items)
			{
				OutData.Add(Item);
			}
		}
	}

protected:
	void Subdivide()
	{
		//TODO: Calc Bound......

		const FVector2D NewExtent = Bound.GetExtent() / 2.f;
		const FVector2D Center = Bound.GetCenter();

		FVector2D NWCenter = Center;
		NWCenter.X -= NewExtent.X;
		NWCenter.Y += NewExtent.Y;

		const FVector2D NECenter = Center + NewExtent;

		const FVector2D SWCenter = Center - NewExtent;

		FVector2D SECenter = Center;
		SECenter.X += NewExtent.X;
		SECenter.Y -= NewExtent.Y;

		const FBox2D NWBox = FBox2D(NWCenter - NewExtent, NWCenter + NewExtent);
		const FBox2D NEBox = FBox2D(NECenter - NewExtent, NECenter + NewExtent);
		const FBox2D SWBox = FBox2D(SWCenter - NewExtent, SWCenter + NewExtent);
		const FBox2D SEBox = FBox2D(SECenter - NewExtent, SECenter + NewExtent);

		const int32 NextDepth = Depth + 1;
		NW = MakeShared<QuadTreeNode>(NWBox, NextDepth);
		NE = MakeShared<QuadTreeNode>(NEBox, NextDepth);
		SW = MakeShared<QuadTreeNode>(SWBox, NextDepth);
		SE = MakeShared<QuadTreeNode>(SEBox, NextDepth);

		bIsSubdivided = true;
	}

	FBox2D Bound;
	int32 Depth;

	TArray<FVector2D> Items;
	bool bIsSubdivided = false;

	TSharedPtr<QuadTreeNode> NW;
	TSharedPtr<QuadTreeNode> NE;
	TSharedPtr<QuadTreeNode> SW;
	TSharedPtr<QuadTreeNode> SE;
};

//SW(South west) 방향이 0,0 부터 시작하는 쿼드 트리.
class QuadTree
{
public:
	QuadTree()
	{
		MaximumDepth = 10;
		BoundaryWH = FVector2D(pow(2, MaximumDepth), pow(2, MaximumDepth));
		InnerBoundary = FBox2D(FVector2D(0, 0), BoundaryWH);
	}
public:
	int32 MaximumDepth;
	FBox2D WorldBoundary;

protected:
	FBox2D InnerBoundary;
	FVector2D BoundaryWH;
	TSharedPtr<QuadTreeNode> Root;
	

public:
	void AddElement(const FVector& InPosition)
	{
		if (Root == nullptr)
		{
			Root = MakeShared<QuadTreeNode>(InnerBoundary, 0);
		}
		const FVector2D QuadTreeLoc = UnrealWorldToQuadTreeLocation(InPosition);
		Root->InsertData(QuadTreeLoc,MaximumDepth);
	}

	void GetNearestElements(const FVector& SearchPos, TArray<FVector2D>& OutArray)
	{
		const FVector2D QuadTreeLoc = UnrealWorldToQuadTreeLocation(SearchPos);
		Root->GetNearestElements(OutArray, QuadTreeLoc, MaximumDepth);
	}

protected:
	/// <summary>
	/// (0,0,10000,10000) 이 -> (0,0,1024,1024) 로 바뀌거나
	/// (-6000,-3000,7000,10000) -> (0,0,1024,1024) 로 바뀌어야함.
	/// (500,3500,13500,165000) ->
	/// 
	/// </summary>
	/// <param name="InUnrealPos"></param>
	/// <returns></returns>
	const FVector2D UnrealWorldToQuadTreeLocation(const FVector& InUnrealPos) const
	{
		const FVector2D Max = WorldBoundary.Max;
		const FVector2D Min = WorldBoundary.Min;

		const FVector2D Cur = FVector2D(InUnrealPos.X, InUnrealPos.Y);

		return ((Cur - Min) / (Max - Min)) * BoundaryWH;
	}
};

UCLASS()
class SPATIALPARTITIONING_API AQuadTreeTestActor : public ABaseAlgorithmTestActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQuadTreeTestActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void RunAlgorithm() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere)
	FBox2D WorldArea;

	UPROPERTY(EditAnywhere)
	int32 TreeDepth = 10;

	UPROPERTY(EditAnywhere)
	int32 TestCase = 1000;
};
