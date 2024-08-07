// Fill out your copyright notice in the Description page of Project Settings.


#include "LightMapBakeUtility.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/ObjectLibrary.h"
#include "EditorFramework/AssetImportData.h"

void ULightMapBakeUtility::BakeLightMap_Implementation(int32 MinLightMapResolution)
{
	// Get Editor World
	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
	if (!EditorWorld) return;

	// Get all static mesh actors in world
	TArray<AActor*> EditorActors;
	UGameplayStatics::GetAllActorsOfClass(EditorWorld, AStaticMeshActor::StaticClass(), EditorActors);
	if (EditorActors.IsEmpty()) return;

	auto ObjectLibrary = UObjectLibrary::CreateLibrary(UStaticMesh::StaticClass(), false, false);
	if (!ObjectLibrary) return;
	TArray<FAssetData> AssetDatas;

	// Find the nearest PowerOfTwo number for the user Resolution
	{
		int32 PowerOfTwo = 2;
		while ( !(MinLightMapResolution >= PowerOfTwo && MinLightMapResolution <= PowerOfTwo * 2) )
			PowerOfTwo *= 2;

		MinLightMapResolution = MinLightMapResolution - PowerOfTwo < PowerOfTwo * 2 - MinLightMapResolution ? PowerOfTwo : PowerOfTwo * 2;
	}

	// Iterate through found meshes and setting them properties
	for (AActor* actor : EditorActors)
	{
		if (UStaticMeshComponent* sm_component = actor->GetComponentByClass<UStaticMeshComponent>())
		{
			UStaticMesh* MeshAsset = sm_component->GetStaticMesh().Get();
			MeshAsset->SetLightMapResolution(MinLightMapResolution / 2);
		}
	}

}
