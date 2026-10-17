#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_TEXTS 500
#define MAX_BUFFER 2048
#define SAVE_FILE "save.dat"

/* the structure of the player */
typedef struct {
    char name[50];    /* Player's name */
    int hp;
    int max_hp;
    int strength;
    int potions;
    
    int level;        /* For the XP bar */
    int xp;
    int xp_max;
    
    int has_sword;    /* Inventory flags */
    int has_armor;
    int has_shield;
    int has_legendary_sword;
    int has_legendary_armor;
    int monster_killed;

    int checkpoint;   /* For the save system */
    int language_choice; 
} Player;


char *game_texts[MAX_TEXTS]; 

/* FUNCTIONS  */


void clear_buffer() { int c; while ((c = getchar()) != '\n' && c != EOF); }  /*this fonction is for clean or eat the data of the choice of the player save in geatchar  */

void load_game_texts(const char *filename) { 
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("\nCRITICAL ERROR: Cannot open file '%s'.\n", filename);  /* this finction is really the heart for the game*/
        printf("Please ensure the file exists in the same folder.\n");   /*this fonction read and load the text from the files*/
        printf("The game will close now.\n");                       /*he keep the id select in the main and go find the other same id on the files*/
        getchar(); /* Wait for key press */                         /*and after read it the fonction show it on the main  */
        exit(1);
    }
    
    char line[MAX_BUFFER];
    int current_id = 0;
    char buffer[MAX_BUFFER] = "";
    while (fgets(line, MAX_BUFFER, f)) {
        int id_temp;
        if (sscanf(line, "%d:", &id_temp) == 1) {
            if (current_id > 0) game_texts[current_id] = strdup(buffer);
            current_id = id_temp;
            char *text_start = strchr(line, ':');
            if (text_start) strcpy(buffer, text_start + 1);
        } else { strcat(buffer, line); }
    }
    if (current_id > 0) game_texts[current_id] = strdup(buffer);
    fclose(f);
}

void save_game(Player *p) {           /*the fonction for save the games on a new files*/
    FILE *f = fopen(SAVE_FILE, "wb");  /*the fonction open or create a new binary files*/
    if (f) {
        fwrite(p, sizeof(Player), 1, f);    /*and after the fonction take all the fields of the player and past it on the new files */
        fclose(f);
        printf("\n[SYSTEM] Game Autosaved (Checkpoint %d).\n", p->checkpoint);
    }
}

int load_game(Player *p) {
    FILE *f = fopen(SAVE_FILE, "rb");
    if (f) {
        size_t result = fread(p, sizeof(Player), 1, f);
        fclose(f);
        if (result == 1) return 1;
    }
    return 0;
}

void print_text(int id, const char *arg) {       /*search the requested ID for it; if there is no ID, the fonction will return an error message*/
    printf("\n");
    printf("\n______________________________________________________________________________________________________________________________________________________\n");
    if (game_texts[id] != NULL) printf(game_texts[id], arg);
    else printf("[ERROR: Missing Text ID %d]\n", id);
    printf("\n______________________________________________________________________________________________________________________________________________________\n");
    printf("\n");
}

void print_option(int num_choice, int id_text) {
    if (game_texts[id_text] != NULL) printf("%d. %s", num_choice, game_texts[id_text]);
    else printf("%d. [Missing Option ID %d]\n", num_choice, id_text);
}

int get_choice(int max_choice) {
    int choice = 0;
    while (choice < 1 || choice > max_choice) {
        printf("\nChoice / choix 1-%d :  ", max_choice);
        if (scanf("%d", &choice) != 1) clear_buffer();
    }
    clear_buffer();
    return choice;
}

void draw_interface(Player *p) {
    printf("\n========================================\n");
    printf(" %s | Lvl %d | HP: %d/%d | STR: %d | Pots: %d\n", 
           p->name, p->level, p->hp, p->max_hp, p->strength, p->potions);
    
    /* Draw the XP bar */
    printf(" XP: [");
    int bar_length = 20;
    int filled = 0;
    if (p->xp_max > 0) filled = (p->xp * bar_length) / p->xp_max;
    
    for (int i = 0; i < bar_length; i++) {
        if (i < filled) printf("#");
        else printf("-");
    }
    printf("] %d/%d\n", p->xp, p->xp_max);
    printf("========================================\n");
}

void check_level_up(Player *p) {
    if (p->xp >= p->xp_max) {
        p->level++;
        p->xp = p->xp - p->xp_max; 
        p->xp_max = p->xp_max * 1.5; 
        
        p->max_hp += 5;
        p->hp += 10; 
        p->strength += 2;

        printf("\n\n___________LEVEL UP !!! ___________\n");
        printf("You reached level %d !\n", p->level);
        printf(">> Max HP +5\n>> Strength +2\n>> HEAL !\n");
        printf("____________________________________\n\n");
        
        save_game(p);
        printf("[Press Enter to celebrate...]"); getchar(); clear_buffer();
    }
}

void combat(Player *p, char *monster_name, int monster_hp, int monster_dmg, int xp_reward) {
    printf("\n  COMBAT : %s (HP: %d) ⚔️\n", monster_name, monster_hp);
    
    while (p->hp > 0 && monster_hp > 0) {
        draw_interface(p); 
      
        printf("\nmonstre : %s | PV : %d monster\n", monster_name, monster_hp);
        
        printf("\n1. Attack\n2. Super Potion (+30 HP) (%d remaining)\n", p->potions);
        int action = get_choice(2);

        if (action == 1) {
            int dmg = p->strength + (rand() % 3); 
            printf(">> Hit for %d damage!\n", dmg);
            monster_hp -= dmg;
        } else {
            if (p->potions > 0) {
                p->hp += 30; 
                if (p->hp > p->max_hp) p->hp = p->max_hp;
                p->potions--; 
                printf(">> Gulp... +30 HP!\n");
            } else printf(">> Empty bag! No potions!\n");
        }

        if (monster_hp > 0) {
            int mdmg = monster_dmg + (rand() % 2) - 1; if (mdmg < 0) mdmg = 0;
            printf(">> %s attacks : %d damage!\n", monster_name, mdmg);
            p->hp -= mdmg;
        }
    }

    if (p->hp <= 0) {
        print_text(331, p->name);
        printf("\n GAME OVER \n");
        exit(0);
    } else {
        printf("\n VICTORY ! %s defeated.\n", monster_name);
        p->monster_killed++;
        
        /* XP Gain logic */
        printf(">> Gain: %d XP\n", xp_reward);
        p->xp += xp_reward;
        check_level_up(p); 
        
        save_game(p);
    }
}


/* MAIN PROGRAM  */

int main() {
    Player p;
    int choice;
    int language = 1; 
    int game_loaded = 0;

    srand(time(NULL));
    memset(game_texts, 0, sizeof(game_texts));

    /* menu start of the game */
    printf("|======================================================================================================================|\n");
    printf("|===|                                         THE DRAGON-C QUEST FORGOTTEN FORTRESS                                |===|\n");
    printf("|======================================================================================================================|\n");
    FILE *f = fopen(SAVE_FILE, "rb");
    if (f) {
        fclose(f);
        printf("1. New Game / Nouvelle Partie\n");
        printf("\n");       
        printf("2. Continue / Continuer\n");
        int start_choice = get_choice(2);
        
        if (start_choice == 2) {
            /* Try to load the save file to restore progress */
            if (load_game(&p)) {
                game_loaded = 1;
                language = p.language_choice;
                printf("\nSave loaded! Welcome back, Lvl %d %s.\n", p.level, p.name);
            } else {
                printf("\nSave file incompatible. Starting new game.\n");
            }
        }
    }

    if (!game_loaded) {
        printf("1. English\n");
        printf("2. Francais\n");
        printf("choice / choix : ");
        scanf("%d", &language);
        clear_buffer();

        printf("\n(Your name/Votre nom): ");
        fgets(p.name, 50, stdin);
        p.name[strcspn(p.name, "\n")] = 0; 

        /* Initialization of the basic statistics for the hero */
        p.max_hp = 20;
        p.hp = 20;
        p.strength = 3;
        p.potions = 1;
        p.level = 1;      
        p.xp = 0;
        p.xp_max = 100;    

        p.has_sword = 0; p.has_armor = 0; p.has_legendary_sword = 0; p.has_legendary_armor = 0;
        p.monster_killed = 0;
        p.checkpoint = 0; 
        p.language_choice = language;
    }

    /* loading charge files */
    if (language == 2) {
        load_game_texts("histoire_fr.txt");
    } else {
        load_game_texts("histoire_eng.txt");
    }

    /* the start of the games */

    /* Checkpoint 0: Intro */
    if (p.checkpoint == 0) {
        print_text(1, p.name); 
        print_option(1, 2); print_option(2, 3);
        choice = get_choice(2); 

        if (choice == 2) { print_text(4, p.name); return 0; }   /*the happy ending*/

        print_text(10, p.name);
        print_option(1, 11); print_option(2, 12);
        choice = get_choice(2);

        if (choice == 2) { print_text(13, p.name); return 0; }  /*the ending when you get excuted*/

        print_text(20, p.name);
        printf("\n[Press ENTER to continue...]"); getchar(); 
        print_text(30, p.name);
        
        p.has_sword = 1; p.has_armor = 1;                           /*the basically stuff of the player*/
        p.strength += 2; p.max_hp += 5; p.hp = p.max_hp;

        printf("\n[Press ENTER to continue...]"); getchar();

        p.checkpoint = 1;
        save_game(&p); 
    }

    /* Checkpoint 1: Mountain & Entrance */
    if (p.checkpoint == 1) {
        print_text(40, p.name);
        print_option(1, 41); print_option(2, 42);
        choice = get_choice(2);

        if (choice == 2) {
            p.strength += 1; p.max_hp += 1; p.hp = p.max_hp;
            print_text(43, p.name);
            printf("\n[Press ENTER to continue...]"); getchar();
        }

        print_text(50, p.name);
        print_option(1, 51); print_option(2, 52);
        choice = get_choice(2);

        int secret_passage = 0; 
        if (choice == 2) {
            print_text(53, p.name);
            print_option(1, 54); print_option(2, 55); print_option(3, 56);
            int search_choice = get_choice(3);

            if (search_choice == 1) {
                print_text(60, p.name); secret_passage = 1; 
                printf("\n[Press ENTER to continue...]"); getchar();
            } else if (search_choice == 2) {
                p.potions++;
                printf("[System] +1 Potion!\n");
                print_text(61, p.name);
                printf("\n\n[Press ENTER to continue...]"); getchar();
            } else {
                print_text(62, p.name);
                printf("\n[Press ENTER to continue...]"); getchar();
            }
        }

        if (secret_passage) {
            print_text(80, p.name);                                         /*the secret passage to directly on floor -2*/
            printf("\n[Press ENTER to continue...]"); getchar();
            print_text(90, p.name);
            p.checkpoint = 3; 
        } else {
            p.checkpoint = 2; 
        }
        save_game(&p);
    }

    /* Checkpoint 2: Floor -1 (Crypt) */
    if (p.checkpoint == 2) {
        print_text(70, p.name);
        printf("\n[Press ENTER to continue...]"); getchar();
        print_text(71, p.name); 

        print_option(1, 72); print_option(2, 73); print_option(3, 74);
        int crypt_choice = get_choice(3);

        if (crypt_choice == 1) {              /*a room of monster*/
            print_text(100, "Goule"); 
            combat(&p, "Goule", 15, 3, 50); 
            print_text(101, "Goule");
            printf("\n[Press ENTER to continue...]"); getchar();
        } else if (crypt_choice == 2) {
            print_text(120, p.name);
            printf("\n[Press ENTER to continue...]"); getchar();
        } else {
            print_text(110, p.name);
            printf("\n[Press ENTER to continue...]"); getchar();
        }

        print_text(150, "Squelettes");
        combat(&p, "Duo de Squelettes", 25, 4, 70);  /*second room of monster*/
        
        print_text(160, p.name); 
        p.hp = p.max_hp; 
        printf("\n[System] Rest: HP Restored.\n");  /*the room for eat and heal the charteres kitchen*/
        printf("\n[Press ENTER to continue...]"); getchar();

        print_text(170, "Armure Hantee");        /*3eme room of monster*/
        combat(&p, "Armure Hantee", 30, 5, 80);
        
        print_text(180, p.name);
        print_option(1, 181); print_option(2, 182);
        int chest_choice = get_choice(2);
        
        if (chest_choice == 1) {
            print_text(183, "Epee Runique");   /*choice chest one*/
            p.strength += 5;
            printf("[System] Strength +5\n");
        } else {
            print_text(184, "Potion de Force");
            p.potions += 2; p.strength += 2;              /*choise chest two*/
            printf("[System] +2 Potions, Strength +2\n");
        }
        printf("\n[Press ENTER to continue...]"); getchar();

        print_text(190, p.name);
        printf("\n[Press ENTER to continue...]"); getchar();

        p.checkpoint = 3;
        save_game(&p);
    }

    /* Checkpoint 3: Floor -2 (Volcano) */
    if (p.checkpoint == 3) {
        print_text(200, p.name); 
        print_option(1, 201); print_option(2, 202); print_option(3, 203);
        choice = get_choice(3);

        if (choice == 1) {
            print_text(210, p.name);
            combat(&p, "Golem de Magma", 40, 6, 100);  /*fourth room of monster*/
        } else if (choice == 2) {
            print_text(220, p.name);
            print_option(1, 221); print_option(2, 222); /*another room of monster but you can have a avantage if you choices to hit the monster */
            int mimic_action = get_choice(2);
            if (mimic_action == 1) {
                print_text(223, p.name); p.hp -= 5;
                printf("[System] -5 HP!\n");
            } else {
                print_text(224, p.name);
            }
            combat(&p, "Mimic", 35, 7, 100);
        } else {
            print_text(230, p.name);
            print_option(1, 231); get_choice(1);
            print_text(232, p.name);
            p.hp = p.max_hp;
            printf("[System] Miracle! Max HP.\n");
            printf("\n[Press ENTER to continue...]"); getchar();
        }

        print_text(240, p.name);
        printf("\n[Press ENTER to continue...]"); getchar();

        print_text(250, p.name);
        p.hp += 10; if(p.hp > p.max_hp) p.hp = p.max_hp;
        printf("[System] +10 PV.\n");
        printf("\n[Press ENTER to continue...]"); getchar();

        print_text(260, p.name);
        combat(&p, "Gardes Noirs", 50, 6, 150);

        print_text(270, p.name);
        print_option(1, 271); print_option(2, 272);
        int forge_loot = get_choice(2);
        if (forge_loot == 1) {
            print_text(273, "Marteau Volcanique"); p.strength += 8;
        } else {
            print_text(274, "Amulette de Feu"); p.max_hp += 10; p.hp += 10;
        }
        printf("\n[Press ENTER to continue...]"); getchar();

        print_text(280, p.name);
        printf("\n[Press ENTER to continue...]"); getchar();

        p.checkpoint = 4;
        save_game(&p);
    }

    /* Checkpoint 4: Floor -3 (the dragon) */
    if (p.checkpoint == 4) {
        print_text(290, p.name);
        print_option(1, 291); print_option(2, 292);
        choice = get_choice(2);

        if (choice == 1) {
            print_text(300, p.name);
            print_option(1, 301); get_choice(1);              /*room of food for the hero can be at the max hp before the fight*/
            p.max_hp += 20; p.hp = p.max_hp; p.strength += 2;
            printf("[System] Feast! Max HP +20, Strength +2.\n");
        } else {
            print_text(310, p.name);
            print_option(1, 311); print_option(2, 312); print_option(3, 313);
            int luck_chest = get_choice(3);
            if (luck_chest == 1) {              /*room of chest but you can take only one chest */
                print_text(314, p.name);
                p.has_legendary_sword = 1; p.strength += 20;
                printf("[System] MASSIVE STRENGTH GAINED!\n");
            } else if (luck_chest == 2) {
                print_text(315, p.name);
                combat(&p, "Mimic Ancien", 60, 8, 200);
            } else {
                print_text(316, p.name);
                p.has_legendary_armor = 1; p.max_hp += 50; p.hp = p.max_hp;
                printf("[System] MASSIVE DEFENSE GAINED!\n");
            }
            printf("\n[Press ENTER to continue...]"); getchar();
        }

        p.checkpoint = 5;
        save_game(&p);
    }

    /* Checkpoint 5: Final Boss */
    if (p.checkpoint == 5) {
        print_text(320, p.name);
        printf("\n[SYSTEM] BOSS FIGHT START !!!\n");
        printf("\n[Press ENTER to continue...]"); getchar();

        int dragon_hp = 200;
        if (p.has_legendary_sword) dragon_hp = 100;
        int dragon_dmg = 12;
        if (p.has_legendary_armor) dragon_dmg = 6;

        combat(&p, "DRAGON ANCESTRAL", dragon_hp, dragon_dmg, 5000);

        print_text(330, p.name);
        printf("|=============================================================================================================================|\n");
        printf("|===|                                                      THE END                                                        |===|\n");
        printf("|=============================================================================================================================|\n");
        printf("Merci d'avoir joue / Thanks for playing!  LANIEZ gabriel KWIZERA henry\n");
        
        p.checkpoint = 0;
        save_game(&p);
    }

    getchar();
    return 0;
}