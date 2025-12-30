// Fill out your copyright notice in the Description page of Project Settings.


#include "SpatialPartitioningHashGridActor.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

//Include보다는 Exclude가 더 빠를 것으로 예상하여 Exclude로 처리
//모든 물체가 안 움직인다는(Static) 전제 하에 아래의 로직들이 처리됨
#define EXCLUDE_FROM_SPATIAL_PARTITIONING FName("ExcludeSP")

void ASpatialPartitioningHashGridActor::InitStaticMeshComponents()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Actors);

	//액터 찾을 때 조건이 좀더 좁다면, 훨씬 더 빨리 찾음.
	for (AActor*& Act : Actors)
	{
		if (Act->ActorHasTag(EXCLUDE_FROM_SPATIAL_PARTITIONING))
		{
			continue;
		}

		TArray<UStaticMeshComponent*> Components;
		Act->GetComponents<UStaticMeshComponent>(Components);

		for (UStaticMeshComponent*& Comp : Components)
		{
			if (Comp->ComponentHasTag(EXCLUDE_FROM_SPATIAL_PARTITIONING))
			{
				continue;
			}
			const FName areaID = GetAreaHashID(Comp->GetComponentLocation());

			if (StaticMeshHashData.Contains(areaID) == false)
			{
				StaticMeshHashData.Add(areaID, FSpatialData());
			}

			const ECollisionEnabled::Type CollisionType = Comp->GetCollisionEnabled();
			if (Comp->GetCollisionEnabled() == ECollisionEnabled::NoCollision)
			{
				continue;
			}

			FSpatialElement pair;
			pair.bSimulatePhysics = Comp->IsSimulatingPhysics();
			pair.StaticMeshComponent = Comp;
			pair.CollisionEnabledType = CollisionType;

			pair.StaticMeshComponent->SetSimulatePhysics(false);
			pair.StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			StaticMeshHashData[areaID].StaticMeshHashData.Add(pair);
		}
	}
}

void ASpatialPartitioningHashGridActor::UpdatePartitioningState()
{
	if (PlayerChar.Get() == nullptr)
	{
		return;
	}

	//Active는 활성화, Fail은 비활성화
	//현재 있는 액터들의 ActiveHashIDList들을 수집함
	//Need to optimize.... (If Center grid is same as prev, then don't add to ActiveHashIDList)
	TSet<FName> ActiveHashIDList;
	ActiveHashIDList.Reserve(9 + DynamicActors.Num() * 9);

	const FName CurPlayerAreaHashID = GetAreaHashID(PlayerChar->GetActorLocation());
	if (PlayerAreaHashID.CenterHashGridID.IsNone())
	{
		PlayerAreaHashID.CenterHashGridID = CurPlayerAreaHashID;
		PlayerAreaHashID.NeighbourHashGridIDList = GetNeighbourAreaHashIDList(PlayerAreaHashID.CenterHashGridID);
	}

	ActiveHashIDList.Add(CurPlayerAreaHashID);
	for (const FName& HashGridID : PlayerAreaHashID.NeighbourHashGridIDList)
	{
		ActiveHashIDList.Add(HashGridID);
	}

	for (FSpatialDynamicActor& ActorData : DynamicActors)
	{
		const FName CurHashID = GetAreaHashID(ActorData.Actor->GetActorLocation());
		if (ActorData.AreaHashID.CenterHashGridID.IsNone())
		{
			ActorData.AreaHashID.CenterHashGridID = CurHashID;
			ActorData.AreaHashID.NeighbourHashGridIDList = GetNeighbourAreaHashIDList(ActorData.AreaHashID.CenterHashGridID);
		}

		ActiveHashIDList.Add(CurHashID);
		for (const FName& HashGridID : ActorData.AreaHashID.NeighbourHashGridIDList)
		{
			ActiveHashIDList.Add(HashGridID);
		}
	}

	//만약 현재 HashID와 이미 저장된 HashID가 다르다면, 이미 저장된 HashID는 FailHashIDList에 넣음
	//Need to optimize.... (If Center grid is same as prev, then don't add to DeactiveHashIDList)
	TSet<FName> DeactiveHashIDList;
	if (PlayerAreaHashID.CenterHashGridID.IsEqual(CurPlayerAreaHashID) == false)
	{
		DeactiveHashIDList.Add(PlayerAreaHashID.CenterHashGridID);
		for (const FName& HashGridID : PlayerAreaHashID.NeighbourHashGridIDList)
		{
			DeactiveHashIDList.Add(HashGridID);
		}

		PlayerAreaHashID.CenterHashGridID = CurPlayerAreaHashID;
		PlayerAreaHashID.NeighbourHashGridIDList = GetNeighbourAreaHashIDList(PlayerAreaHashID.CenterHashGridID);
	}

	for (FSpatialDynamicActor& ActorData : DynamicActors)
	{
		const FName CurHashID = GetAreaHashID(ActorData.Actor->GetActorLocation());

		FHashGridIDSet& AreaHashID = ActorData.AreaHashID;
		if (AreaHashID.CenterHashGridID.IsEqual(CurHashID) == false)
		{
			DeactiveHashIDList.Add(AreaHashID.CenterHashGridID);
			for (const FName& HashGridID : AreaHashID.NeighbourHashGridIDList)
			{
				DeactiveHashIDList.Add(HashGridID);
			}

			AreaHashID.CenterHashGridID = CurHashID;
			AreaHashID.NeighbourHashGridIDList = GetNeighbourAreaHashIDList(AreaHashID.CenterHashGridID);
		}
	}

	//Deactive 먼저(DeactiveList에 Active 항목이 있을 수 있으나, 고려하지 않음. Active 항목을 Iterate 하는 비용의 가치가 없다고 판단하였기 때문이다.)
	for (const FName& HashID: DeactiveHashIDList)
	{
		UpdateAreaStaticMeshComponents(HashID, false);
	}

	//그 다음 Active
	for (const FName& HashID : ActiveHashIDList)
	{
		UpdateAreaStaticMeshComponents(HashID, true);
	}
}

FName ASpatialPartitioningHashGridActor::GetAreaHashID(const FVector& InLocation) const
{
	int32 IntLocX = (int32)InLocation.X;
	int32 IntLocY = (int32)InLocation.Y;

	int32 DivX = (int32)(IntLocX / GridWidth);
	int32 DivY = (int32)(IntLocY / GridHeight);

	FString HashID = FString();
	HashID.Append(FString::FromInt(DivX));
	HashID.Append(",");
	HashID.Append(FString::FromInt(DivY));

	FName name(HashID);
	return name;
}

TSet<FName> ASpatialPartitioningHashGridActor::GetNeighbourAreaHashIDList(FName InCenterHash) const
{
	TArray<FString> Parser;
	InCenterHash.ToString().ParseIntoArray(Parser, TEXT(","));
	int32 DivX = FCString::Atoi(*Parser[0]);
	int32 DivY = FCString::Atoi(*Parser[1]);

	TSet<FName> NeighbourList;
	NeighbourList.Reserve(8);

	{
		TArray<TPair<int32, int32>> NeighbourDivList;
		NeighbourDivList.Reserve(8);
		NeighbourDivList.Add(TPair<int32,int32>(DivX,DivY+1)); //Up
		NeighbourDivList.Add(TPair<int32, int32>(DivX - 1, DivY + 1)); // LeftUp
		NeighbourDivList.Add(TPair<int32, int32>(DivX-1, DivY)); //Left
		NeighbourDivList.Add(TPair<int32, int32>(DivX - 1, DivY - 1)); //LeftDown
		NeighbourDivList.Add(TPair<int32, int32>(DivX, DivY - 1)); //Down
		NeighbourDivList.Add(TPair<int32, int32>(DivX + 1, DivY - 1)); //RightDown
		NeighbourDivList.Add(TPair<int32, int32>(DivX + 1, DivY)); //Right
		NeighbourDivList.Add(TPair<int32, int32>(DivX + 1, DivY + 1)); //RightUp

		for (TPair<int32,int32> NeighBour : NeighbourDivList)
		{
			if ((NeighBour.Key >= 0 && NeighBour.Key <= GridCountX) &&
				(NeighBour.Value >= 0 && NeighBour.Value <= GridCountY))
			{
				FString HashID = FString();
				HashID.Append(FString::FromInt(NeighBour.Key));
				HashID.Append(",");
				HashID.Append(FString::FromInt(NeighBour.Value));
				NeighbourList.Add(FName(HashID));
			}
		}
	}
	
	return NeighbourList;
}

void ASpatialPartitioningHashGridActor::UpdateAreaStaticMeshComponents(const FName& InAreaID, const bool bCollisionEnable)
{
	if (StaticMeshHashData.Contains(InAreaID) == false)
	{
		return;
	}

	TArray<FSpatialElement>& Elements = StaticMeshHashData[InAreaID].StaticMeshHashData;

	for (FSpatialElement& Elem : Elements)
	{
		if (bCollisionEnable)
		{
			Elem.StaticMeshComponent->SetCollisionEnabled(Elem.CollisionEnabledType);
			Elem.StaticMeshComponent->SetSimulatePhysics(Elem.bSimulatePhysics);
		}
		else
		{
			Elem.StaticMeshComponent->SetSimulatePhysics(false);
			Elem.StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void ASpatialPartitioningHashGridActor::RegisterDynamicActors(AActor* InActor)
{
	FSpatialDynamicActor SPActor;
	SPActor.Actor = InActor;
	SPActor.AreaHashID.CenterHashGridID = GetAreaHashID(InActor->GetActorLocation());
	DynamicActors.Add(SPActor);
}
