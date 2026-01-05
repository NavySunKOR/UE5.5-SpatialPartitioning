// Fill out your copyright notice in the Description page of Project Settings.


#include "AlgorithmTest/QuadTreeTestActor.h"

// Sets default values
AQuadTreeTestActor::AQuadTreeTestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	WorldArea = FBox2D(FVector2D(-2000, -2000), FVector2D(2000, 2000));
}

// Called when the game starts or when spawned
void AQuadTreeTestActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AQuadTreeTestActor::RunAlgorithm()
{
	TSharedPtr<QuadTree> QT = MakeShared<QuadTree>();
	QT->WorldBoundary = WorldArea;
	QT->MaximumDepth = TreeDepth;

	for (int32 i = 0; i < TestCase; ++i)
	{
		FVector RandPos = FVector();
		RandPos.X = FMath::RandRange(WorldArea.Min.X, WorldArea.Max.X);
		RandPos.Y = FMath::RandRange(WorldArea.Min.Y, WorldArea.Max.Y);
		RandPos.Z = 100.f;
		QT->AddElement(RandPos);
	}

	for (int32 i = 0; i < 20; ++i)
	{
		FVector RandPos = FVector();
		RandPos.X = FMath::RandRange(WorldArea.Min.X, WorldArea.Max.X);
		RandPos.Y = FMath::RandRange(WorldArea.Min.Y, WorldArea.Max.Y);
		RandPos.Z = 100.f;

		TArray<FVector2D> Results;
		QT->GetNearestElements(RandPos, Results);

		for (const FVector2D& Result : Results)
		{
			UE_LOG(LogTemp,Warning,TEXT("%s"),*Result.ToString())
		}

		UE_LOG(LogTemp, Warning, TEXT(""))
		UE_LOG(LogTemp, Warning, TEXT(""))
	}

}

// Called every frame
void AQuadTreeTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}