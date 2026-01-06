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

}

// Called when the game starts or when spawned
void ASPBaseActor::BeginPlay()
{
	Super::BeginPlay();
	PlayerChar = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	InitProperties();
	InitStaticMeshComponents();
}

// Called every frame
void ASPBaseActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdatePartitioningState();

	if (bDebug)
	{
		DebugShowTimer += DeltaTime;
		if (DebugShowTimer > DebugShowInterval)
		{
			DrawDebugObjects();
			DebugShowTimer = 0;
		}
	}
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