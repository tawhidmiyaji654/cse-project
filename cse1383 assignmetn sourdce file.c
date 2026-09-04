
#include <stdio.h>

struct Medicine
{
    char name[30];
    int stock;
    int dailyReq;
    int minStock;
    int expiryDays;
    int essentiality;

    float coverage;
    int condition;
    int priority;
};

/* Medicine information stored in the system */

struct Medicine meds[100] =
{
    {"Oral Saline", 120, 35, 80, 45, 3, 0.0, 0, 0},
    {"Paracetamol", 300, 40, 100, 120, 2, 0.0, 0, 0},
    {"Insulin", 60, 12, 50, 30, 3, 0.0, 0, 0},
    {"Amoxicillin", 200, 25, 80, 20, 3, 0.0, 0, 0},
    {"Antacid", 250, 18, 60, 15, 1, 0.0, 0, 0},
    {"Cetirizine", 180, 15, 50, 90, 1, 0.0, 0, 0}
};

int n = 6;


/* Search medicine by name */

int searchMedicine(char name[])
{
    for (int i = 0; i < n; i++)
    {
        int j = 0;

        while (name[j] != '\0' &&
               meds[i].name[j] != '\0')
        {
            if (name[j] != meds[i].name[j])
            {
                break;
            }

            j++;
        }

        if (name[j] == '\0' &&
            meds[i].name[j] == '\0')
        {
            return i;
        }
    }

    return -1;
}


/* Analyze medicine */

void analyzeMedicine(int i)
{
    /* Calculate stock coverage */

    if (meds[i].dailyReq > 0)
    {
        meds[i].coverage =
            (float)meds[i].stock / meds[i].dailyReq;
    }
    else
    {
        meds[i].coverage = 0;
    }


    /*
       Condition:

       5 = Critical
       4 = Urgent
       3 = Expiry
       2 = Reorder
       1 = Sufficient
    */


    /* Shortage + expiry concern */

    if (meds[i].stock < meds[i].minStock &&
        meds[i].expiryDays <= 30)
    {
        meds[i].condition = 5;
    }

    /* Essential medicine with low stock */

    else if (meds[i].stock < meds[i].minStock &&
             meds[i].essentiality == 3)
    {
        meds[i].condition = 4;
    }

    /* Approaching expiry */

    else if (meds[i].expiryDays <= 30)
    {
        meds[i].condition = 3;
    }

    /* Low stock or low coverage */

    else if (meds[i].stock < meds[i].minStock ||
             meds[i].coverage < 10 ||
             meds[i].stock <= meds[i].minStock * 1.2)
    {
        meds[i].condition = 2;
    }

    /* Sufficient stock */

    else
    {
        meds[i].condition = 1;
    }


    /* Calculate priority */

    if (meds[i].condition == 5)
    {
        meds[i].priority = 50;
    }
    else if (meds[i].condition == 4)
    {
        meds[i].priority = 40;
    }
    else if (meds[i].condition == 3)
    {
        meds[i].priority = 30;
    }
    else if (meds[i].condition == 2)
    {
        meds[i].priority = 20;
    }
    else
    {
        meds[i].priority = 10;
    }

    meds[i].priority =
        meds[i].priority +
        meds[i].essentiality * 10 -
        (int)meds[i].coverage;
}


/* Display condition */

void showCondition(int condition)
{
    if (condition == 5)
    {
        printf("Critical");
    }
    else if (condition == 4)
    {
        printf("Urgent");
    }
    else if (condition == 3)
    {
        printf("Expiry");
    }
    else if (condition == 2)
    {
        printf("Reorder");
    }
    else
    {
        printf("Sufficient");
    }
}


/* Sort medicines according to priority */

void sortMedicines()
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            analyzeMedicine(j);
            analyzeMedicine(j + 1);

            if (meds[j].priority < meds[j + 1].priority)
            {
                struct Medicine temp;

                temp = meds[j];
                meds[j] = meds[j + 1];
                meds[j + 1] = temp;
            }
        }
    }
}


/*
   Show complete medicine report.

   The report is saved in output.txt.
   The same report is also shown on the screen.
*/

void showReport()
{
    FILE *fp = fopen("output.txt", "w");

    if (fp == NULL)
    {
        printf("\nError: Could not create output.txt\n");
        return;
    }

    sortMedicines();

    printf("\n");
    printf("MEDICINE STOCK MANAGEMENT REPORT\n");
    printf("\n");

    fprintf(fp, "MEDICINE STOCK MANAGEMENT REPORT\n\n");


    printf("%-18s %-8s %-10s %-10s %-9s %-10s %-10s %-12s %-8s\n",
           "Medicine",
           "Stock",
           "DailyReq",
           "MinStock",
           "Expiry",
           "Essential",
           "Coverage",
           "Condition",
           "Priority");


    fprintf(fp, "%-18s %-8s %-10s %-10s %-9s %-10s %-10s %-12s %-8s\n",
            "Medicine",
            "Stock",
            "DailyReq",
            "MinStock",
            "Expiry",
            "Essential",
            "Coverage",
            "Condition",
            "Priority");


    printf("------------------------------------------------------------------------------------------\n");

    fprintf(fp,
            "------------------------------------------------------------------------------------------\n");


    for (int i = 0; i < n; i++)
    {
        analyzeMedicine(i);

        printf("%-18s %-8d %-10d %-10d %-9d %-10d %-10.1f ",
               meds[i].name,
               meds[i].stock,
               meds[i].dailyReq,
               meds[i].minStock,
               meds[i].expiryDays,
               meds[i].essentiality,
               meds[i].coverage);


        fprintf(fp, "%-18s %-8d %-10d %-10d %-9d %-10d %-10.1f ",
                meds[i].name,
                meds[i].stock,
                meds[i].dailyReq,
                meds[i].minStock,
                meds[i].expiryDays,
                meds[i].essentiality,
                meds[i].coverage);


        showCondition(meds[i].condition);


        /* Save condition to file */

        if (meds[i].condition == 5)
        {
            fprintf(fp, "Critical");
        }
        else if (meds[i].condition == 4)
        {
            fprintf(fp, "Urgent");
        }
        else if (meds[i].condition == 3)
        {
            fprintf(fp, "Expiry");
        }
        else if (meds[i].condition == 2)
        {
            fprintf(fp, "Reorder");
        }
        else
        {
            fprintf(fp, "Sufficient");
        }


        printf("%*s", 12 - 0, "");

        fprintf(fp, "%*s", 12 - 0, "");


        printf("%d\n", meds[i].priority);

        fprintf(fp, "%d\n", meds[i].priority);
    }


    printf("------------------------------------------------------------------------------------------\n");

    fprintf(fp,
            "------------------------------------------------------------------------------------------\n");


    printf("Medicines are sorted according to management priority.\n");

    fprintf(fp,
            "Medicines are sorted according to management priority.\n");


    fclose(fp);


    printf("\nReport saved successfully in output.txt\n");
}


/* Add new stock */

void addStock()
{
    char name[30];
    int amount;
    int index;


    printf("\nEnter medicine name: ");
    scanf(" %[^\n]", name);


    index = searchMedicine(name);


    if (index == -1)
    {
        printf("Medicine not found.\n");
        return;
    }


    printf("Enter new stock quantity: ");
    scanf("%d", &amount);


    if (amount <= 0)
    {
        printf("Invalid stock quantity.\n");
        return;
    }


    meds[index].stock =
        meds[index].stock + amount;


    analyzeMedicine(index);


    printf("\nStock added successfully.\n");

    printf("Medicine: %s\n",
           meds[index].name);

    printf("Updated Stock: %d\n",
           meds[index].stock);

    printf("Updated Coverage: %.1f days\n",
           meds[index].coverage);


    printf("Updated Condition: ");

    showCondition(meds[index].condition);

    printf("\n");
}


/* Issue medicine */

void issueMedicine()
{
    char name[30];
    int amount;
    int index;


    printf("\nEnter medicine name: ");
    scanf(" %[^\n]", name);


    index = searchMedicine(name);


    if (index == -1)
    {
        printf("Medicine not found.\n");
        return;
    }


    printf("Enter issue quantity: ");
    scanf("%d", &amount);


    if (amount <= 0)
    {
        printf("Invalid quantity.\n");
        return;
    }


    if (amount > meds[index].stock)
    {
        printf("Insufficient stock.\n");
        return;
    }


    meds[index].stock =
        meds[index].stock - amount;


    analyzeMedicine(index);


    printf("\nMedicine issued successfully.\n");

    printf("Medicine: %s\n",
           meds[index].name);

    printf("Remaining Stock: %d\n",
           meds[index].stock);


    printf("Updated Coverage: %.1f days\n",
           meds[index].coverage);


    printf("Updated Condition: ");

    showCondition(meds[index].condition);

    printf("\n");
}


/* Search medicine and show details */

void searchMedicineReport()
{
    char name[30];
    int index;


    printf("\nEnter medicine name: ");
    scanf(" %[^\n]", name);


    index = searchMedicine(name);


    if (index == -1)
    {
        printf("Medicine not found.\n");
        return;
    }


    analyzeMedicine(index);


    printf("\nMedicine Found\n");

    printf("-----------------------------\n");


    printf("Name: %s\n",
           meds[index].name);

    printf("Stock: %d\n",
           meds[index].stock);

    printf("Daily Requirement: %d\n",
           meds[index].dailyReq);

    printf("Minimum Stock: %d\n",
           meds[index].minStock);

    printf("Days to Expiry: %d\n",
           meds[index].expiryDays);

    printf("Essentiality: %d\n",
           meds[index].essentiality);

    printf("Coverage: %.1f days\n",
           meds[index].coverage);


    printf("Condition: ");

    showCondition(meds[index].condition);

    printf("\n");


    printf("Priority: %d\n",
           meds[index].priority);


    printf("-----------------------------\n");
}


/* Add a completely new medicine */

void addNewMedicine()
{
    if (n >= 100)
    {
        printf("Medicine storage is full.\n");
        return;
    }


    printf("\nEnter new medicine name: ");

    scanf(" %[^\n]",
          meds[n].name);


    printf("Enter stock: ");

    scanf("%d",
          &meds[n].stock);


    printf("Enter daily requirement: ");

    scanf("%d",
          &meds[n].dailyReq);


    printf("Enter minimum stock: ");

    scanf("%d",
          &meds[n].minStock);


    printf("Enter days to expiry: ");

    scanf("%d",
          &meds[n].expiryDays);


    printf("Enter essentiality (3=High, 2=Medium, 1=Low): ");

    scanf("%d",
          &meds[n].essentiality);


    meds[n].coverage = 0;

    meds[n].condition = 0;

    meds[n].priority = 0;


    analyzeMedicine(n);


    n++;


    printf("\nNew medicine added successfully.\n");
}


/* Main program */

int main()
{
    int choice;


    do
    {
        printf("\n");

        printf("1. Show Medicine Report\n");

        printf("2. Add New Stock\n");

        printf("3. Issue Medicine\n");

        printf("4. Search Medicine\n");

        printf("5. Add New Medicine\n");

        printf("6. Exit\n");


        printf("\nEnter your choice: ");

        scanf("%d",
              &choice);


        if (choice == 1)
        {
            showReport();
        }

        else if (choice == 2)
        {
            addStock();
        }

        else if (choice == 3)
        {
            issueMedicine();
        }

        else if (choice == 4)
        {
            searchMedicineReport();
        }

        else if (choice == 5)
        {
            addNewMedicine();
        }

        else if (choice == 6)
        {
            printf("\nProgram ended.\n");
        }

        else
        {
            printf("\nInvalid choice.\n");
        }


    } while (choice != 6);


    return 0;
}
