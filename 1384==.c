#include <stdio.h>
#include <string.h>
#define TOTAL_ZONES 6
struct Zone
{
    char id[10];
    char name[40];
    char type[25];
    float requested;
    float minimum;
    float loss;
    int waiting;
    float priority;
    int order;
    float allocated;
    float lossAmount;
    float delivered;
    float shortage;
    char status[20];
};
/* Function Prototypes */
void loadOriginalData(struct Zone zone[]);
void resetResults(struct Zone zone[]);
float getUrgency(char type[]);
void calculatePriority(struct Zone zone[]);
void sortZones(struct Zone zone[]);
void allocateWater(struct Zone zone[], float available);
void calculateResults(struct Zone zone[]);
void runAllocation(struct Zone zone[], float available);
void displayResult(struct Zone zone[], float available);
void searchZone(struct Zone zone[]);
void findHighestShortage(struct Zone zone[]);
void case1(struct Zone original[]);
void case2(struct Zone original[]);
void case3(struct Zone original[]);
void case4(struct Zone original[]);
void case5(struct Zone original[]);
void case6(struct Zone original[]);
void case7(struct Zone original[]);
/* =========================================================
   LOAD ORIGINAL DATA
   ========================================================= */
void loadOriginalData(struct Zone zone[])
{
    /* Z01 */
    strcpy(zone[0].id, "Z01");
    strcpy(zone[0].name, "Municipal Hospital");
    strcpy(zone[0].type, "Hospital");
    zone[0].requested = 4000;
    zone[0].minimum = 3000;
    zone[0].loss = 2;
    zone[0].waiting = 0;
    /* Z02 */
    strcpy(zone[1].id, "Z02");
    strcpy(zone[1].name, "Central Flood Shelter");
    strcpy(zone[1].type, "Emergency Shelter");
    zone[1].requested = 3500;
    zone[1].minimum = 2500;
    zone[1].loss = 4;
    zone[1].waiting = 1;
    /* Z03 */
    strcpy(zone[2].id, "Z03");
    strcpy(zone[2].name, "Ward 3");
    strcpy(zone[2].type, "Residential");
    zone[2].requested = 4500;
    zone[2].minimum = 2000;
    zone[2].loss = 8;
    zone[2].waiting = 2;
    /* Z04 */
    strcpy(zone[3].id, "Z04");
    strcpy(zone[3].name, "Ward 5");
    strcpy(zone[3].type, "Residential");
    zone[3].requested = 3800;
    zone[3].minimum = 1800;
    zone[3].loss = 12;
    zone[3].waiting = 3;
    /* Z05 */
    strcpy(zone[4].id, "Z05");
    strcpy(zone[4].name, "School Emergency Shelter");
    strcpy(zone[4].type, "Emergency Shelter");
    zone[4].requested = 2600;
    zone[4].minimum = 1600;
    zone[4].loss = 5;
    zone[4].waiting = 1;
    /* Z06 */
    strcpy(zone[5].id, "Z06");
    strcpy(zone[5].name, "Fire and Emergency Service");
    strcpy(zone[5].type, "Emergency Service");
    zone[5].requested = 2000;
    zone[5].minimum = 1500;
    zone[5].loss = 1;
    zone[5].waiting = 0;
    resetResults(zone);
}
/* =========================================================
   RESET CALCULATED VALUES
   ========================================================= */
void resetResults(struct Zone zone[])
{
    int i;
    for (i = 0; i < TOTAL_ZONES; i++)
    {
        zone[i].priority = 0;
        zone[i].order = 0;
        zone[i].allocated = 0;
        zone[i].lossAmount = 0;
        zone[i].delivered = 0;
        zone[i].shortage = 0;
        strcpy(zone[i].status, "Unprocessed");
    }
}
/* =========================================================
   URGENCY SCORE
   ========================================================= */
float getUrgency(char type[])
{
    if (strcmp(type, "Hospital") == 0)
        return 10;
    if (strcmp(type, "Emergency Service") == 0)
        return 10;
    if (strcmp(type, "Emergency Shelter") == 0)
        return 8;
    if (strcmp(type, "Residential") == 0)
        return 5;
    return 0;
}
/* =========================================================
   PRIORITY CALCULATION
   =========================================================
   Proposed strategy:
   Priority =
       Urgency
       + Waiting Score
       + Unresolved Demand Score
       - Loss Penalty
   This is our proposed strategy because the assignment
   does not provide a fixed formula.
   ========================================================= */
void calculatePriority(struct Zone zone[])
{
    int i;
    for (i = 0; i < TOTAL_ZONES; i++)
    {
        float urgency;
        float waitingScore;
        float demandScore;
        float lossPenalty;
        urgency = getUrgency(zone[i].type);
        /* More waiting = more priority */
        waitingScore = zone[i].waiting * 2.0;
        /* Larger gap between request and minimum = more need */
        demandScore =
            (zone[i].requested - zone[i].minimum) / 500.0;
        /* Higher loss gets a small penalty */
        lossPenalty = zone[i].loss * 0.10;
        zone[i].priority =
            urgency +
            waitingScore +
            demandScore -
            lossPenalty;
    }
}
/* =========================================================
   SORT ZONES BY PRIORITY
   ========================================================= */
void sortZones(struct Zone zone[])
{
    int i, j;
    struct Zone temp;
    for (i = 0; i < TOTAL_ZONES - 1; i++)
    {
        for (j = 0; j < TOTAL_ZONES - i - 1; j++)
        {
            if (zone[j].priority < zone[j + 1].priority)
            {
                temp = zone[j];
                zone[j] = zone[j + 1];
                zone[j + 1] = temp;
            }
            /* Tie-breaking rule */
            else if (zone[j].priority == zone[j + 1].priority)
            {
                /* First compare waiting cycles */
                if (zone[j].waiting < zone[j + 1].waiting)
                {
                    temp = zone[j];
                    zone[j] = zone[j + 1];
                    zone[j + 1] = temp;
                }
                /* If waiting is also equal, compare minimum */
                else if (zone[j].waiting == zone[j + 1].waiting)
                {
                    if (zone[j].minimum < zone[j + 1].minimum)
                    {
                        temp = zone[j];
                        zone[j] = zone[j + 1];
                        zone[j + 1] = temp;
                    }
                }
            }
        }
    }
    for (i = 0; i < TOTAL_ZONES; i++)
    {
        zone[i].order = i + 1;
    }
}
/* =========================================================
   WATER ALLOCATION
   ========================================================= */
void allocateWater(struct Zone zone[], float available)
{
    int i;
    for (i = 0; i < TOTAL_ZONES; i++)
    {
        float fullRequirement;
        float minimumRequirement;
        if (available <= 0)
        {
            zone[i].allocated = 0;
            continue;
        }
        /*
           Because of distribution loss, the centre must release
           more water than the destination finally receives.
           Example:
           Requested = 4000 L
           Loss = 2%
           Release required to deliver 4000 L:
           4000 / 0.98
        */
        fullRequirement =
            zone[i].requested /
            (1.0 - zone[i].loss / 100.0);
        minimumRequirement =
            zone[i].minimum /
            (1.0 - zone[i].loss / 100.0);
        /*
           Try to satisfy the complete request first.
        */
        if (available >= fullRequirement)
        {
            zone[i].allocated = fullRequirement;
            available = available - fullRequirement;
        }
        /*
           If complete request is impossible,
           try to satisfy the minimum requirement.
        */
        else if (available >= minimumRequirement)
        {
            zone[i].allocated = minimumRequirement;
            available = available - minimumRequirement;
        }
        /*
           If even the minimum cannot be supplied,
           give the remaining water.
        */
        else
        {
            zone[i].allocated = available;
            available = 0;
        }
    }
}
/* =========================================================
   CALCULATE LOSS, DELIVERY AND SHORTAGE
   ========================================================= */
void calculateResults(struct Zone zone[])
{
    int i;
    for (i = 0; i < TOTAL_ZONES; i++)
    {
        /* Calculate distribution loss */
        zone[i].lossAmount =
            zone[i].allocated * zone[i].loss / 100.0;
        /* Calculate actual delivered water */
        zone[i].delivered =
            zone[i].allocated - zone[i].lossAmount;
        /* Calculate remaining shortage */
        zone[i].shortage =
            zone[i].requested - zone[i].delivered;
        if (zone[i].shortage < 0)
            zone[i].shortage = 0;
        /* Determine service condition */
        if (zone[i].delivered >= zone[i].requested)
        {
            strcpy(zone[i].status, "FULL");
        }
        else if (zone[i].delivered >= zone[i].minimum)
        {
            strcpy(zone[i].status, "MINIMUM");
        }
        else if (zone[i].delivered > 0)
        {
            strcpy(zone[i].status, "BELOW MIN");
        }
        else
        {
            strcpy(zone[i].status, "UNSERVED");
        }
    }
}
/* =========================================================
   COMPLETE ALLOCATION PROCESS
   ========================================================= */
void runAllocation(struct Zone zone[], float available)
{
    resetResults(zone);
    calculatePriority(zone);
    sortZones(zone);
    allocateWater(zone, available);
    calculateResults(zone);
}
/* =========================================================
   DISPLAY RESULT
   ========================================================= */
void displayResult(struct Zone zone[], float available)
{
    int i;
    float totalRequested = 0;
    float totalAllocated = 0;
    float totalDelivered = 0;
    float totalShortage = 0;
    int full = 0;
    int minimum = 0;
    int belowMinimum = 0;
    int unserved = 0;
    printf("\n\n");
    printf("============================================================================================\n");
    printf("                              ALLOCATION RESULT\n");
    printf("============================================================================================\n");
    printf("%-5s %-14s %-9s %-9s %-9s %-10s %-10s %-10s %-10s\n",
           "ID",
           "Zone",
           "Request",
           "Minimum",
           "Priority",
           "Allocated",
           "Delivered",
           "Shortage",
           "Status");
    printf("--------------------------------------------------------------------------------------------\n");
    for (i = 0; i < TOTAL_ZONES; i++)
    {
        printf("%-5s %-14s %-9.0f %-9.0f %-9.2f %-10.0f %-10.0f %-10.0f %-10s\n",
               zone[i].id,
               zone[i].name,
               zone[i].requested,
               zone[i].minimum,
               zone[i].priority,
               zone[i].allocated,
               zone[i].delivered,
               zone[i].shortage,
               zone[i].status);
        totalRequested += zone[i].requested;
        totalAllocated += zone[i].allocated;
        totalDelivered += zone[i].delivered;
        totalShortage += zone[i].shortage;
        if (strcmp(zone[i].status, "FULL") == 0)
            full++;
        else if (strcmp(zone[i].status, "MINIMUM") == 0)
            minimum++;
        else if (strcmp(zone[i].status, "BELOW MIN") == 0)
            belowMinimum++;
        else if (strcmp(zone[i].status, "UNSERVED") == 0)
            unserved++;
    }
    printf("============================================================================================\n");
    printf("\nPriority Order:\n");
    for (i = 0; i < TOTAL_ZONES; i++)
    {
        printf("%d. %s - %s (Priority %.2f)\n",
               zone[i].order,
               zone[i].id,
               zone[i].name,
               zone[i].priority);
    }
    printf("\nSummary\n");
    printf("------------------------------------------\n");
    printf("Total Requested Water        : %.2f L\n",
           totalRequested);
    printf("Total Allocated Water        : %.2f L\n",
           totalAllocated);
    printf("Total Effective Water Delivered : %.2f L\n",
           totalDelivered);
    printf("Total Unresolved Requirement : %.2f L\n",
           totalShortage);
    printf("Remaining Water              : %.2f L\n",
           available - totalAllocated);
    printf("\nService Conditions\n");
    printf("------------------------------------------\n");
    printf("Full Requirement Satisfied   : %d zone(s)\n",
           full);
    printf("Minimum Requirement Satisfied: %d zone(s)\n",
           minimum);
    printf("Below Minimum                : %d zone(s)\n",
           belowMinimum);
    printf("Completely Unserved          : %d zone(s)\n",
           unserved);
}
/* =========================================================
   SEARCH A ZONE
   ========================================================= */
void searchZone(struct Zone zone[])
{
    char id[10];
    int i;
    int found = 0;
    printf("\nEnter Zone ID: ");
    scanf("%s", id);
    for (i = 0; i < TOTAL_ZONES; i++)
    {
        if (strcmp(zone[i].id, id) == 0)
        {
            printf("\nZone Found\n");
            printf("--------------------------------\n");
            printf("Zone ID          : %s\n", zone[i].id);
            printf("Service Zone     : %s\n", zone[i].name);
            printf("Type             : %s\n", zone[i].type);
            printf("Requested        : %.2f L\n", zone[i].requested);
            printf("Minimum          : %.2f L\n", zone[i].minimum);
            printf("Loss             : %.2f %%\n", zone[i].loss);
            printf("Waiting Cycles   : %d\n", zone[i].waiting);
            printf("Priority         : %.2f\n", zone[i].priority);
            printf("Allocated        : %.2f L\n", zone[i].allocated);
            printf("Loss Amount      : %.2f L\n", zone[i].lossAmount);
            printf("Effective Water  : %.2f L\n", zone[i].delivered);
            printf("Shortage         : %.2f L\n", zone[i].shortage);
            printf("Condition        : %s\n", zone[i].status);
            found = 1;
            break;
        }
    }
    if (found == 0)
        printf("\nZone not found.\n");
}
/* =========================================================
   FIND HIGHEST UNRESOLVED REQUIREMENT
   ========================================================= */
void findHighestShortage(struct Zone zone[])
{
    int i;
    int position = 0;
    for (i = 1; i < TOTAL_ZONES; i++)
    {
        if (zone[i].shortage > zone[position].shortage)
        {
            position = i;
        }
    }
    printf("\nHighest Unresolved Requirement\n");
    printf("--------------------------------\n");
    printf("Zone ID  : %s\n", zone[position].id);
    printf("Zone     : %s\n", zone[position].name);
    printf("Shortage : %.2f L\n", zone[position].shortage);
}
/* =========================================================
   CASE 1
   Original dataset + 13,500 L
   ========================================================= */
void case1(struct Zone original[])
{
    struct Zone zone[TOTAL_ZONES];
    int i;
    for (i = 0; i < TOTAL_ZONES; i++)
        zone[i] = original[i];
    printf("\n========== CASE 1 ==========\n");
    printf("Original Dataset\n");
    printf("Available Water: 13,500 L\n");
    runAllocation(zone, 13500);
    displayResult(zone, 13500);
}
/* =========================================================
   CASE 2
   Enough water for all zones
   ========================================================= */
void case2(struct Zone original[])
{
    struct Zone zone[TOTAL_ZONES];
    int i;
    float enoughWater = 0;
    for (i = 0; i < TOTAL_ZONES; i++)
    {
        /*
           Calculate the amount required from the centre
           to deliver the complete requested quantity
           after distribution loss.
        */
        enoughWater +=
            original[i].requested /
            (1.0 - original[i].loss / 100.0);
    }
    /*
       Add a little extra so there is definitely enough.
    */
    enoughWater += 500;
    for (i = 0; i < TOTAL_ZONES; i++)
        zone[i] = original[i];
    printf("\n========== CASE 2 ==========\n");
    printf("Enough Water for Complete Requested Quantity\n");
    printf("Available Water: %.2f L\n", enoughWater);
    runAllocation(zone, enoughWater);
    displayResult(zone, enoughWater);
}
/* =========================================================
   CASE 3
   User gives water below combined minimum requirement
   ========================================================= */
void case3(struct Zone original[])
{
    struct Zone zone[TOTAL_ZONES];
    int i;
    float available;
    for (i = 0; i < TOTAL_ZONES; i++)
        zone[i] = original[i];
    printf("\n========== CASE 3 ==========\n");
    printf("Enter available water in litres: ");
    scanf("%f", &available);
    if (available <= 0)
    {
        printf("Water quantity must be greater than zero.\n");
        return;
    }
    runAllocation(zone, available);
    displayResult(zone, available);
}
/* =========================================================
   CASE 4
   Equal priority
   ========================================================= */
void case4(struct Zone original[])
{
    struct Zone zone[TOTAL_ZONES];
    int i;
    for (i = 0; i < TOTAL_ZONES; i++)
        zone[i] = original[i];
    /*
       Modify Z03 and Z04 so that their priority
       becomes equal.
    */
    zone[2].waiting = 3;
    zone[3].waiting = 3;
    zone[2].requested = 4000;
    zone[3].requested = 4000;
    zone[2].minimum = 2000;
    zone[3].minimum = 2000;
    zone[2].loss = 10;
    zone[3].loss = 10;
    printf("\n========== CASE 4 ==========\n");
    printf("Equal Priority Test\n");
    printf("Z03 and Z04 have been modified to create equal priority.\n");
    runAllocation(zone, 13500);
    displayResult(zone, 13500);
}
/* =========================================================
   CASE 5
   User increases loss of a residential zone
   ========================================================= */
void case5(struct Zone original[])
{
    struct Zone zone[TOTAL_ZONES];
    int i;
    int position = -1;
    char id[10];
    float newLoss;
    for (i = 0; i < TOTAL_ZONES; i++)
        zone[i] = original[i];
    printf("\n========== CASE 5 ==========\n");
    printf("Residential zones are: Z03 and Z04\n");
    printf("Enter residential Zone ID: ");
    scanf("%s", id);
    /* Find the selected zone */
    for (i = 0; i < TOTAL_ZONES; i++)
    {
        if (strcmp(zone[i].id, id) == 0)
        {
            position = i;
            break;
        }
    }
    if (position == -1)
    {
        printf("Zone not found.\n");
        return;
    }
    /* Check whether it is residential */
    if (strcmp(zone[position].type, "Residential") != 0)
    {
        printf("Invalid zone. Please select a residential zone.\n");
        return;
    }
    printf("Current distribution loss: %.2f%%\n",
           zone[position].loss);
    printf("Enter new distribution loss (%%): ");
    scanf("%f", &newLoss);
    if (newLoss < 0 || newLoss >= 100)
    {
        printf("Invalid loss percentage.\n");
        return;
    }
    zone[position].loss = newLoss;
    printf("\nNew loss of %s = %.2f%%\n",
           zone[position].id,
           zone[position].loss);
    runAllocation(zone, 13500);
    displayResult(zone, 13500);
}
/* =========================================================
   CASE 6
   User increases waiting period of an underserved zone
   ========================================================= */
void case6(struct Zone original[])
{
    struct Zone zone[TOTAL_ZONES];
    int i;
    int position = -1;
    char id[10];
    int newWaiting;
    /*
       First run original case to identify underserved zones.
    */
    for (i = 0; i < TOTAL_ZONES; i++)
        zone[i] = original[i];
    runAllocation(zone, 13500);
    printf("\n========== CASE 6 ==========\n");
    printf("Previously underserved zones from Case 1:\n");
    for (i = 0; i < TOTAL_ZONES; i++)
    {
        if (strcmp(zone[i].status, "BELOW MIN") == 0 ||
            strcmp(zone[i].status, "UNSERVED") == 0)
        {
            printf("%s - %s\n",
                   zone[i].id,
                   zone[i].name);
        }
    }
    printf("\nEnter Zone ID from the underserved list: ");
    scanf("%s", id);
    for (i = 0; i < TOTAL_ZONES; i++)
    {
        if (strcmp(zone[i].id, id) == 0)
        {
            position = i;
            break;
        }
    }
    if (position == -1)
    {
        printf("Zone not found.\n");
        return;
    }
    /*
       Check whether selected zone was underserved.
    */
    if (strcmp(zone[position].status, "BELOW MIN") != 0 &&
        strcmp(zone[position].status, "UNSERVED") != 0)
    {
        printf("This zone was not underserved in Case 1.\n");
        return;
    }
    printf("Current waiting cycles: %d\n",
           zone[position].waiting);
    printf("Enter new waiting cycles: ");
    scanf("%d", &newWaiting);
    if (newWaiting < zone[position].waiting)
    {
        printf("New waiting cycle should be greater than the current value.\n");
        return;
    }
    zone[position].waiting = newWaiting;
    printf("\nWaiting cycles of %s changed to %d.\n",
           zone[position].id,
           zone[position].waiting);
    runAllocation(zone, 13500);
    displayResult(zone, 13500);
}
/* =========================================================
   CASE 7
   Independent test - user chooses water amount
   ========================================================= */
void case7(struct Zone original[])
{
    struct Zone zone[TOTAL_ZONES];
    int i;
    float available;
    for (i = 0; i < TOTAL_ZONES; i++)
        zone[i] = original[i];
    printf("\n========== CASE 7 ==========\n");
    printf("Independent Test Case\n");
    printf("Enter any available water quantity: ");
    scanf("%f", &available);
    if (available <= 0)
    {
        printf("Water quantity must be greater than zero.\n");
        return;
    }
    runAllocation(zone, available);
    displayResult(zone, available);
}
/* =========================================================
   MAIN FUNCTION
   ========================================================= */
int main()
{
    struct Zone original[TOTAL_ZONES];
    int choice;
    /* Load assignment data only once */
    loadOriginalData(original);
    do
    {
        printf("\n\n");
        printf("====================================================\n");
        printf("       EMERGENCY POTABLE WATER ALLOCATION\n");
        printf("====================================================\n");
        printf("1. Case 1 - Original Dataset\n");
        printf("2. Case 2 - Enough Water\n");
        printf("3. Case 3 - Below Minimum Requirement\n");
        printf("4. Case 4 - Equal Priority\n");
        printf("5. Case 5 - Increased Distribution Loss\n");
        printf("6. Case 6 - Increased Waiting Period\n");
        printf("7. Case 7 - Independent Test\n");
        printf("8. Search a Zone\n");
        printf("9. Find Highest Unresolved Requirement\n");
        printf("10. Exit\n");
        printf("\nEnter your choice: ");
        scanf("%d", &choice);
        switch (choice)
        {
            case 1:
                case1(original);
                break;
            case 2:
                case2(original);
                break;
            case 3:
                case3(original);
                break;
            case 4:
                case4(original);
                break;
            case 5:
                case5(original);
                break;
            case 6:
                case6(original);
                break;
            case 7:
                case7(original);
                break;
            case 8:
            {
                struct Zone zone[TOTAL_ZONES];
                int i;
                for (i = 0; i < TOTAL_ZONES; i++)
                    zone[i] = original[i];
                runAllocation(zone, 13500);
                searchZone(zone);
                break;
            }
            case 9:
            {
                struct Zone zone[TOTAL_ZONES];
                int i;
                for (i = 0; i < TOTAL_ZONES; i++)
                    zone[i] = original[i];
                runAllocation(zone, 13500);
                findHighestShortage(zone);
                break;
            }
            case 10:
                printf("\nProgram ended.\n");
                break;
            default:
                printf("\nInvalid choice. Please try again.\n");
        }
    } while (choice != 10);
    return 0;
}
