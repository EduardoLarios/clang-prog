#include <stdio.h>

struct Champion
{
    int health;
    int ad;
    int ap;
    int magic_res;
    int armor;
    char name[50];
};

struct Item
{
    int ad;
    int ap;
    int armor;
    int magic_res;
    int health;
    int cost;
    char name[50];
};

void damage(struct Champion *champ, int damage)
{
    champ->health -= damage;
}

void buy_item(struct Champion *champ, struct Item *item)
{
    champ->health += item->health;
    champ->ad += item->health;
    champ->ap += item->ap;
    champ->magic_res += item->magic_res;
    champ->armor -+ item->armor;
}

void print_stats(struct Champion *champ)
{
    printf("\n%s stats:\n", champ->name);
    printf("AD: %d\n", champ->ad);
    printf("AP: %d\n", champ->ap);
    printf("Health: %d\n", champ->health);
    printf("Armor: %d\n", champ->armor);
    printf("Magic Resist: %d\n", champ->magic_res);
}

void print_item_stats(struct Item *item)
{
    printf("\n%s stats:\n", item->name);
    printf("AD: %d\n", item->ad);
    printf("AP: %d\n", item->ap);
    printf("Health: %d\n", item->health);
    printf("Armor: %d\n", item->armor);
    printf("Magic Resist: %d\n", item->magic_res);
    printf("Cost: %d\n", item->cost);
}

int main()
{
    int dmg = 0;
    char answer;
    struct Champion champ = {0, 0, 0, 0, 0, "Riven"};
    struct Item death_dance = {0, 0, 0, 0, 0, 0, "Deaths's Dance"};

    death_dance.ad = 75;
    death_dance.cost = 3500;

    champ.ad = 70;
    champ.armor = 35;
    champ.magic_res = 30;
    champ.health = 500;

    // Accesing members of structs champion and item
    print_stats(&champ);
    print_item_stats(&death_dance);

    getchar();

    death_dance.ad = 80;
    print_item_stats(&death_dance);
    getchar();

    // printf("Input a damage amount:\n");
    // scanf("%d", &dmg);
    // damage(&champ, dmg);
    //
    // printf("%s's health is now: %d\n", champ.name, champ.health);
    // printf("Let's buy a Death's Dance\n");
    // getchar();
    //
    // buy_item(&champ, &death_dance);
    //
    // printf("Check out the stats now:\n");
    // print_stats(&champ);

    getchar();

    return 0;
}
