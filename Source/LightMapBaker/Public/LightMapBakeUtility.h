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
	UFUNCTION(BlueprintCallable) void SetLightMapDensity(UStaticMesh* Mesh, float Density);
	UFUNCTION(BlueprintCallable, BlueprintPure) TArray<AActor*> GetSelectedActors();
	UFUNCTION(BlueprintCallable) void SetMinLightMapRes(int32 Resolution);
	double GetStaticMeshMeshArea(UStaticMesh* Mesh);
	int32 GetMinLightMapResolutionFromCurrent(int32 CurrentResolution);
};
