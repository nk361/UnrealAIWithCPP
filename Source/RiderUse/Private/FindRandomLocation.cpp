// Fill out your copyright notice in the Description page of Project Settings.


#include "RiderUse/Public/FindRandomLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "NPC_AIController.h"
//#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "blackboard_keys.h"

UFindRandomLocation::UFindRandomLocation(FObjectInitializer const& object_initializer)//to display a meaningful name to the node in the blackboard
{
    NodeName = TEXT("Find Random Location");
}

EBTNodeResult::Type UFindRandomLocation::ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* node_memory)
{
    //Get AI controller and its NPC
    ANPC_AIController* const cont = Cast<ANPC_AIController>(owner_comp.GetAIOwner());
    APawn const* npc = cont->GetPawn();

    //obtain npc location to use as an origin location
    FVector const origin = npc->GetActorLocation();
    FNavLocation loc;//used to store the output location

    //Get the navigation system and generate a random location on the NavMesh
    UNavigationSystemV1* const nav_sys = UNavigationSystemV1::GetCurrent(GetWorld());
    if(nav_sys->GetRandomPointInNavigableRadius(origin, search_radius, loc, nullptr))//NavData may not be nullptr if you have multiple NavMeshes
    {
        cont->get_blackboard()->SetValueAsVector(bb_keys::target_location, loc.Location);
    }

    //Finish with success
    FinishLatentTask(owner_comp, EBTNodeResult::Succeeded);
    return EBTNodeResult::Succeeded;//You can pass back failed or other things to move to different nodes of the behavior tree from left to right from selector I think
}