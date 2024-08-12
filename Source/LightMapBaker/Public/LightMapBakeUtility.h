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
	UPROPERTY(EditAnywhere) bool bEnableLMD = true;
	UFUNCTION(BlueprintCallable, CallInEditor) void SetLightMapDensity(
		UPARAM(DisplayName = "Light Map Density (0-2)", meta = (editcondition="bEnableLMD") ) float LightMapDensity = 0.8,
		int32 MinLightMapRes = 512,
		int32 LightMapCoordinateIndex = 1);
	UFUNCTION(BlueprintCallable, BlueprintPure) TArray<AActor*> GetSelectedActors();

	// Returns area of all triangles of a mesh (with scale)
	double GetStaticMeshArea(UStaticMesh* Mesh, FVector Scale = FVector(1.f));

	// Returns Minimun LightMap Resolution that is less than given and is a power of two
	int32 GetMinLightMapResolutionFromCurrent(int32 CurrentResolution);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UWorld* GetWorld() const;
};
