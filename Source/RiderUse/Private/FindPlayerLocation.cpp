// Fill out your copyright notice in the Description page of Project Settings.


#include "FindPlayerLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "NPC_AIController.h"
//#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "blackboard_keys.h"

UFindPlayerLocation::UFindPlayerLocation(FObjectInitializer const& object_initializer)//to display a meaningful name to the node in the blackboard
{
    NodeName = TEXT("Find Player Location");
}

EBTNodeResult::Type UFindPlayerLocation::ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* node_memory)
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Finding location near player"));
    
    //Get the player's character and the NPC's controller
    ACharacter* const player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);//Player index is probably for multiplayer, picking which user
    ANPC_AIController* const cont = Cast<ANPC_AIController>(owner_comp.GetAIOwner());

    //Get player location to use as an origin
    FVector const player_location = player->GetActorLocation();
    if(search_random)
    {
        FNavLocation loc;

        //Get the navigation system and generate a random location near the player
        UNavigationSystemV1* const nav_sys = UNavigationSystemV1::GetCurrent(GetWorld());
        if(nav_sys->GetRandomPointInNavigableRadius(player_location, search_radius, loc, nullptr))//if it is true, it stores the location in loc, I think the last param is for multiple navmeshes and that it's mentioned in FindRandomLocation
        {
            cont->get_blackboard()->SetValueAsVector(bb_keys::target_location, loc.Location);
        }
    }
    else
    {
        cont->get_blackboard()->SetValueAsVector(bb_keys::target_location, player_location);
    }

    //Finish with success
    FinishLatentTask(owner_comp, EBTNodeResult::Succeeded);
    return EBTNodeResult::Succeeded;
}