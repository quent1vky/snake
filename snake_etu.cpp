#include <iostream>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib> 
#include <ctime> 
#include "time.h"

using namespace std;

#define SIZE_MAXI 200
#define LARGEUR 50
#define HAUTEUR 25

int TEMPS = 200000;

struct fruit {
    int fruitX;
    int fruitY;
};

enum direction { STOP = 0, GAUCHE, DROITE, HAUT, BAS };

struct jeu {
    bool jeuEnCours;
    int teteX;
    int teteY;
    int score;
    int tailleQueue;
    int acc;
    int tmp;
    int bonus;
    direction d;
    fruit f;
};

jeu initialiser();
void dessiner(jeu j, int tabX[], int tabY[]);
string entrerDirection();
jeu deplacer(jeu j, int queueX[], int queueY[]);
int userInput();
void SNAKE();



int main(){
    jeu j;
    int queueX[SIZE_MAXI], queueY[SIZE_MAXI];
    string direction;
    srand(time(NULL));

    j = initialiser();


    while (!userInput()) {
        dessiner(j,queueX, queueY);
        usleep(j.tmp);
    }

    while (j.jeuEnCours) {
        dessiner(j, queueX, queueY);
        j = deplacer(j, queueX, queueY);


        //Le serpent avance tant que la partie est en cours
        if (userInput()) {

            direction = entrerDirection();

            if (direction == "HAUT" && j.d != BAS)
                j.d = HAUT;
            else if (direction == "GAUCHE" && j.d != DROITE)
                j.d = GAUCHE;
            else if (direction == "BAS" && j.d != HAUT)
                j.d = BAS;
            else if (direction == "DROITE" && j.d != GAUCHE)
                j.d = DROITE;
        }

        usleep(j.tmp);

    }

    cout << "Game Over! Votre score est: " << j.score << endl;  // Fin du jeu
    return 0;

}

void SNAKE(){
    printf("  SSSS  N   N  AAAAAAA K   K  EEEEE \n");
    printf(" S      NN  N  A     A K  K   E \n");
    printf("  SSS   N N N  AAAAAAA K K    EEEE \n");
    printf("      S N  NN  A     A KK     E \n");
    printf("  SSSS  N   N  A     A K  K   EEEEE \n");

}




jeu initialiser() {
    jeu j;
    fruit f;
    j.jeuEnCours = true;
    j.teteX = LARGEUR/2;
    j.teteY = HAUTEUR/2;
    j.score = 0;
    j.tailleQueue = 1; 
    j.acc = 0;
    j.tmp = TEMPS;
    j.bonus = 5;
    

    //déclaration des directions
    j.d = GAUCHE; 
    j.d = HAUT;
    j.d = BAS;
    j.d = DROITE;
    j.d = STOP;

    //déclaration des fruits
    
    j.f.fruitX = rand() % (HAUTEUR - 2)+1;
    j.f.fruitY = rand() % (HAUTEUR - 2)+1;


    return j;
}

void dessiner(jeu j, int tabX[], int tabY[]) {
    system("clear");
    dessinerEnSnake();
    int i, k, w, acc;
    char c;
    for (i = 0; i < HAUTEUR; i++) {
        for (k = 0; k < LARGEUR; k++) {
            c = ' ';
            if (i == 0 || i == HAUTEUR - 1 || k == 0 || k == LARGEUR - 1) {  // position des rebords de la map
                c = '*';
            } else {
                if (i == j.teteY && k == j.teteX) {
                    c = '@';
                } else if (i == j.f.fruitY && k == j.f.fruitX) {
                    if (j.acc == 10) {
                        c = '$';  // Affiche le bonus "$" lorsque l'accumulateur atteint 10
                        j.acc = 0;
                    } else {
                        c = 'x';  // Affiche le fruit régulier
                    }
                } else {
                    for (w = 0; w < j.tailleQueue; w++) {
                        if (tabX[w] == k && tabY[w] == i) {
                            c = 'o';
                            break;
                        }
                    }
                }
            }
            cout << c;
        }
        printf("\n");
    }
    cout << "Vitesse serpent : " << j.tmp/100;
    printf("\n");
    cout << "Votre score est: " << j.score << endl;
}



string entrerDirection() {
    // On récupère le caractère entrée sur le clavier
    char c = getchar();

    //On gère les differents cas suivants les touches saisies sur le claviers
    switch (c) {
        case 'z':
            return "HAUT";
        case 'q':
            return "GAUCHE";
        case 's':
            return "BAS";
        case 'd':
            return "DROITE";
        default:
            return "";
    } 
}





jeu deplacer(jeu j, int queueX[], int queueY[]) {
    int tmpX, tmpY, stockX, stockY,i,acc;
    bool fruit;

    // Déplacez le serpent en fonction de la direction actuelle
    switch (j.d) {
        case GAUCHE:
            j.teteX--; // Le serpent se déplace à gauche sur l'abscisse
            break;
        case DROITE:
            j.teteX++; // Le serpent se déplace à droite sur l'abscisse
            break;
        case HAUT:
            j.teteY--; // Le serpent se déplace en haut sur l'ordonnée
            break;
        case BAS:
            j.teteY++; // Le serpent se déplace en bas sur l'ordonnée
            break;
        default:
            break;
    }


    // Si la tête du snake touche le bord, la partie est fini
    if (j.teteY <= 0 || j.teteY >= HAUTEUR - 1  || j.teteX <= 0 || j.teteX >= LARGEUR - 1)
        j.jeuEnCours = false;


    // Si la tête du snake touche la queue, la partie est fini
    for (i=0;i<SIZE_MAXI;i++){
        if (j.teteX == queueX[i] && j.teteY == queueY[i]){
            j.jeuEnCours = false;
        }
    }
    

    if (j.teteX == j.f.fruitX && j.teteY == j.f.fruitY) {
        if (j.acc == 10) {  // Lorsque l'accumulateur atteint 9, ajouter le bonus
            j.score += j.bonus;
            j.bonus += 2;
            j.acc = 0;  // On réinitialise l'accumulateur après avoir ajouté le bonus
            j.tmp -= 5000; // On diminue du temps 

        }else {
            j.tmp -= 1500;
            j.score++;
            j.tailleQueue++;
            j.acc++;
        }
        


        // On génère la position des coordonnées du nouveau fruit

    do {
        j.f.fruitX = rand() % (HAUTEUR - 2) + 1; // j.f.fruitX = [1, Hauteur-2]
        j.f.fruitY = rand() % (HAUTEUR - 2) + 1; // j.f.fruitY = [1, Hauteur-2]

        fruit = true; // Réinitialisez fruit à true à chaque itération

        // On vérifie si le fruit généré se trouve sur la queue du serpent
        for (i = 0; i < SIZE_MAXI; i++) {
            if (queueX[i] == j.f.fruitX && queueY[i] == j.f.fruitY) {
                fruit = false; 
            }
        }
    } while ((j.f.fruitX == j.teteX && j.f.fruitY == j.teteY) || !fruit);

    }


    //Décalage du reste

    tmpX = queueX[0];
    tmpY = queueY[0];
    queueX[0] = j.teteX;
    queueY[0] = j.teteY;

    //Décalage du reste
    for (i=1;i<j.tailleQueue ;i++){
        stockX = queueX[i];
        stockY = queueY[i];
        queueX[i] = tmpX;
        queueY[i] = tmpY;
        tmpX = stockX;
        tmpY = stockY;
    }



    // Mettez à jour les positions de la queue du serpent

    return j;
}


//http://cboard.cprogramming.com/c-programming/63166-kbhit-linux.html
int userInput() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}


