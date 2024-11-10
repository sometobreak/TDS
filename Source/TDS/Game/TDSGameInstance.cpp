// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSGameInstance.h"

bool UTDSGameInstance::GetWeaponInfoByName(FName NameWeapon, FWeaponInfo &OutInfo)
{
	bool bIsFind = false;
	if (WeaponInfoTable)
	{
		FWeaponInfo* WeaponInfoRow;
		WeaponInfoRow = WeaponInfoTable->FindRow<FWeaponInfo>(NameWeapon, "", false);
		if (WeaponInfoRow)
		{
			bIsFind = true;
			OutInfo = *WeaponInfoRow;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UTDSGameInstance::GetWeaponInfoByName - WeaponTable -NULL"));
	}
		
	return bIsFind;
}

bool UTDSGameInstance::GetDropWeaponInfoByName(FName NameWeapon, FDropWeapon &OutInfo)
{
	bool bIsFind = false;

	if (DropWeaponInfoTable)
	{
		FDropWeapon* DropWeaponInfoRow;
		TArray<FName> DropWeaponNames = DropWeaponInfoTable->GetRowNames();
		
		int8 i = 0;
		while (i < DropWeaponNames.Num() && !bIsFind)
		{
			DropWeaponInfoRow = DropWeaponInfoTable->FindRow<FDropWeapon>(DropWeaponNames[i], "");
			if (DropWeaponInfoRow->WeaponDropInfo.NameItem == NameWeapon)
			{
				OutInfo = (*DropWeaponInfoRow);
				bIsFind = true;
			}
			i++;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UTDSGameInstance::GetDropWeaponInfoByName - DropWeaponInfoTable -NULL"));
	}

	return bIsFind;
}
