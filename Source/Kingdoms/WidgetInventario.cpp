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

int UWidgetInventario::GetItemByIndex(int SlotIndex)
{
    return 0;
}

bool UWidgetInventario::CheckHasItem(int ItemID)
{
    return true;
}
