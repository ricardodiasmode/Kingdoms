// Fill out your copyright notice in the Description page of Project Settings.

#include "WidgetInventario.h"

void UWidgetInventario::NativeConstruct()
{
    Super::NativeConstruct();

    GenerateSlots(NumberOfSlots);
}

void UWidgetInventario::GenerateSlots(int SlotsToCreate)
{
    for (int i = 0; i < SlotsToCreate; i++)
    {
        UWidgetSlotInventario* WidgetInstance = CreateWidget<UWidgetSlotInventario>(this, UWidgetSlotInventario::StaticClass());
        Slots.Add(WidgetInstance);
        Slots[i]->AddToViewport();
    }
}

FS_Item UWidgetInventario::GetItemByIndex(int SlotIndex)
{
    FS_Item a;
    return a;
}

bool UWidgetInventario::CheckHasItem(int ItemID)
{
    return true;
}
