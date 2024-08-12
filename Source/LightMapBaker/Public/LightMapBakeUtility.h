// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorActionUtility.h"
#include "LightMapBakeUtility.generated.h"

/**
 * 
 */
UCLASS()
class LIGHTMAPBAKER_API ULightMapBakeUtility : public UActorActionUtility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, CallInEditor) void SetLightMapDensity(float Density);
	UFUNCTION(BlueprintCallable, BlueprintPure) TArray<AActor*> GetSelectedActors();

	// Returns area of all triangles of a mesh (with scale)
	double GetStaticMeshArea(UStaticMesh* Mesh, FVector Scale = FVector(1.f));

	// Returns Minimun LightMap Resolution that is less than given and is a power of two
	int32 GetMinLightMapResolutionFromCurrent(int32 CurrentResolution);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UWorld* GetWorld() const;

	UFUNCTION(BlueprintPure, Category = "Corridor", meta = (Keywords = "corridor vertex mesh meshdata", NativeBreakFunc))
	TArray<FVector> MeshData(const UStaticMeshComponent* StaticMeshComponent);
};
