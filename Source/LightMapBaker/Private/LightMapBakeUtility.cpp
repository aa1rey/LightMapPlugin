// Fill out your copyright notice in the Description page of Project Settings.


#include "LightMapBakeUtility.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/ObjectLibrary.h"
//#include "GeometryScript/MeshUVFunctions.h"
//#include "GeometryScript/MeshAssetFunctions.h"
#include "EditorFramework/AssetImportData.h"

void ULightMapBakeUtility::BakeLightMap_Implementation(int32 LightMapRes)
{
	// Get Editor World
	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
	if (!EditorWorld) return;

	// Get all static mesh actors in world
	TArray<AActor*> EditorActors;
	UGameplayStatics::GetAllActorsOfClass(EditorWorld, AStaticMeshActor::StaticClass(), EditorActors);
	if (EditorActors.IsEmpty()) return;

	//auto ObjectLibrary = UObjectLibrary::CreateLibrary(UStaticMesh::StaticClass(), false, false);
	//if (!ObjectLibrary) return;
	//TArray<FAssetData> AssetDatas;

	int32 MinLightMapRes;
	// Find the nearest PowerOfTwo number for the Min LightMap Resolution
	{
		int32 PowerOfTwo = 2;
		while ( !(LightMapRes >= PowerOfTwo && LightMapRes <= PowerOfTwo * 2) )
			PowerOfTwo *= 2;

		MinLightMapRes = LightMapRes - PowerOfTwo < PowerOfTwo * 2 - LightMapRes ? PowerOfTwo : PowerOfTwo * 2;
	}

	// Iterate through found meshes and setting them properties
	for (AActor* actor : EditorActors)
	{
		if (UStaticMeshComponent* sm_component = actor->GetComponentByClass<UStaticMeshComponent>())
		{
			UStaticMesh* MeshAsset = sm_component->GetStaticMesh().Get();
			MeshAsset->SetLightMapResolution(LightMapRes);

			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 60.f, FColor::Red, *FString::Printf(
				TEXT("Asset: %s | Density: %f | Resolution: %i"),
				*MeshAsset->GetName(),
				MeshAsset->GetLightmapUVDensity(),
				LightMapRes));

			/*EGeometryScriptOutcomePins Pins;
			FGeometryScriptMeshReadLOD RequestedLOD;
			FGeometryScriptCopyMeshFromAssetOptions Options;
			UDynamicMesh* NewDynamicMesh = UGeometryScriptLibrary_StaticMeshFunctions::CopyMeshFromStaticMesh(MeshAsset, {}, {}, {}, Pins);
			
			if (NewDynamicMesh)
			{
				double MeshArea, UVArea;
				FBox MeshBounds;
				FBox2D UVBounds;
				bool bIsValidUVSet, bUVUnset;

				UGeometryScriptLibrary_MeshUVFunctions::GetMeshUVSizeInfo(
					NewDynamicMesh,
					0,
					{},
					MeshArea,
					UVArea,
					MeshBounds,
					UVBounds,
					bIsValidUVSet,
					bUVUnset);

				MeshAsset->SetLightmapUVDensity(MeshArea / pow(LightMapRes, 2) / UVArea);

				GEngine->AddOnScreenDebugMessage(INDEX_NONE, 60.f, FColor::Orange, *FString::Printf(
					TEXT("Density: %f"), MeshAsset->GetLightmapUVDensity()
				));
			}*/
		}
	}
}
