// Fill out your copyright notice in the Description page of Project Settings.


#include "SPBaseActor.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"

// Sets default values
ASPBaseActor::ASPBaseActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GridCountX = 5;
	GridCountY = 5;

}

// Called when the game starts or when spawned
void ASPBaseActor::BeginPlay()
{
	Super::BeginPlay();
	PlayerChar = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	InitGridData();
	InitStaticMeshComponents();
}

// Called every frame
void ASPBaseActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdatePartitioningState();
}

void ASPBaseActor::SetBoundsAlignActor()
{
	if (BoundSetActor == nullptr)
	{
		return;
	}

	// Expand bounds for the BoundsAlignActor and all primitive components that write to this virtual texture.
	FBox Bounds;
	BoundSetActor->GetActorBounds(false,Center,Extent);
}

void ASPBaseActor::InitGridData()
{
	StartPos = Center;
	StartPos.X -= Extent.X;
	StartPos.Y -= Extent.Y;

	const FVector FullExtent = Extent * 2.f;

	GridWidth = uint32(FullExtent.X / GridCountX);
	GridHeight = uint32(FullExtent.Y / GridCountY);
}
