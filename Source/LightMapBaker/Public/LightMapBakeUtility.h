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
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent) void BakeLightMap(int32 MinLightMapResolution);
	void BakeLightMap_Implementation(int32 MinLightMapResolution);
	
};
