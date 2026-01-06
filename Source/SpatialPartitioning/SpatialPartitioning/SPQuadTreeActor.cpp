// Fill out your copyright notice in the Description page of Project Settings.


#include "SpatialPartitioning/SPQuadTreeActor.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

void ASPQuadTreeActor::InitProperties()
{
	QT = MakeShared<QuadTree>();
	FVector2D TwoDCenter = FVector2D(Center);
	FVector2D TwoDExtent = FVector2D(Extent);

	FBox2D Boundary = FBox2D(TwoDCenter - TwoDExtent, TwoDCenter + TwoDExtent);
	QT->WorldBoundary = Boundary;
	QT->MaximumDepth = MaximumDepth;
}

void ASPQuadTreeActor::InitStaticMeshComponents()
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
			FBox CompBound = Comp->Bounds.GetBox();
			FVector CompExtent = CompBound.GetExtent();
			FVector LT = CompBound.GetCenter();
			LT.X -= CompExtent.X;
			LT.Y += CompExtent.Y;
			FVector RT = CompBound.GetCenter();
			RT += CompExtent;
			FVector LB = CompBound.GetCenter();
			LB -= CompExtent;
			FVector RB = CompBound.GetCenter();
			RB.X += CompExtent.X;
			RB.Y -= CompExtent.Y;

			UE_LOG(LogTemp, Warning, TEXT("Actor : %s"), *Comp->GetOwner()->GetActorLabel())
			QT->AddElement(Comp, Comp->GetComponentLocation());
			UE_LOG(LogTemp, Warning, TEXT("Comp->GetComponentLocation() : %s"), *LT.ToString())
			QT->AddElement(Comp, LT);
			UE_LOG(LogTemp,Warning,TEXT("LT : %s"), *LT.ToString())
			QT->AddElement(Comp, RT);
			UE_LOG(LogTemp, Warning, TEXT("RT : %s"), *RT.ToString())
			QT->AddElement(Comp, LB);
			UE_LOG(LogTemp, Warning, TEXT("LB : %s"), *LB.ToString())
			QT->AddElement(Comp, RB);
			UE_LOG(LogTemp, Warning, TEXT("RB : %s"), *RB.ToString())
		}
	}
}

void ASPQuadTreeActor::UpdatePartitioningState()
{

}

void ASPQuadTreeActor::DrawDebugObjects()
{
	FVector ActorLocation = PlayerChar->GetActorLocation();
	FVector2D ActorLoc2D = FVector2D(ActorLocation);
	TArray<QuadTreeNodeElement> QTElements;
	QT->GetNearestElements(ActorLocation, QTElements);

	for (QuadTreeNodeElement& QTElem : QTElements)
	{
		FBox Bound = QTElem.Component->Bounds.GetBox();
		DrawDebugBox(GetWorld(), Bound.GetCenter(), Bound.GetExtent(), FColor::Blue,false, DebugShowInterval);
	}

	TArray<FBox2D> Divisions;
	QT->GetAllSubdivisions(Divisions);

	UE_LOG(LogTemp, Warning, TEXT("Pos : %s"), *ActorLocation.ToString())

	for (FBox2D& Div : Divisions)
	{
		const FVector2D& Extent2D = Div.GetExtent();
		FVector DivExtent;
		DivExtent.X = Extent2D.X;
		DivExtent.Y = Extent2D.Y;
		DivExtent.Z = 100;


		const FVector2D& Center2D = Div.GetCenter();
		FVector DivCenter;
		DivCenter.X = Center2D.X;
		DivCenter.Y = Center2D.Y;
		DivCenter.Z = 100;
	
		DrawDebugBox(GetWorld(), DivCenter, DivExtent, FColor::Red, false, DebugShowInterval);
	}
}
