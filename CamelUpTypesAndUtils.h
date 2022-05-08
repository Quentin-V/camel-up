#ifndef CAMELUP_CAMELUPTYPESANDUTILS_H
#define CAMELUP_CAMELUPTYPESANDUTILS_H

#include <stdbool.h>
#include <stdio.h>

// *********************************
// ***** Déclaration des types *****
// *********************************

/**
 * Type du chameau
 * Il contient une position, une couleur et le chameau qu'il a sur le dos s'il en a un
 */
typedef struct Chameau {
	int position; // Position du chameau sur le plateau
	char ** couleur; // Chaine de caractère de la couleur du chameau
	struct Chameau * chameauSurLeDos; // Pointeur sur le chameau qui est sur son dos s'il existe
} Chameau;

/**
 * Type qui définit un pari sur une manche
 * Il contient la couleur du chameau concerné par le pari et la valeur du pari
 */
typedef struct PariManche {
	int couleur;
	int valeurPari;
} PariManche;

/**
 * Type de la tuile désert
 * Elle contient sa position et son côté (Oasis ou Mirage)
 */
typedef struct TuileDesert {
	bool coteOasis;
	int position;
} TuileDesert;

/**
 * Type représentant le joueur
 * Il a un nom, un nombre de livres égyptiennes
 * Un nombre de tuiles pyramide en sa possession
 * Le nombre de PariManche qu'il a en cours
 * Une liste contenant ses PariManche
 * Une liste pour sa possession de tuiles de PariCourse
 * Sa tuile désert
 */
typedef struct Parieur {
	char nom[21]; // 20 caractères + \0 de fin de chaine
	int or;
	int tuilesPyramide;
	int nbParisManche;
	PariManche *parisManche[NB_COULEUR * 3]; // Tableau de pointeurs sur des PariManche
	bool tuilesPariCourse[NB_COULEUR];
	TuileDesert tuileDesert;
} Parieur;

/**
 * Type du PariCourse
 * Il a la couleur du chameau
 * Une indication sur le type de pari (Victoire ou défaite)
 */
typedef struct PariCourse {
	int couleur;
	bool victorieux;
	Parieur parieur;
} PariCourse;

/**
 * Permet de savoir si la chaine passée en paramètre contient un '\n'
 * @param str la chaine de caractères à analyser
 * @return vrai si la chaine contient '\n', faux sinon
 */
bool containsLineBreak(const char * str) {
	for(int i = 0; str[i] != '\0'; ++i) { // On présume que la chaine est bien formée (donc pas de buffer overflow)
		if(str[i] == '\n') return true;
	}
	return false;
}

/**
 * Remplace le premier '\n' trouvé dans une chaine par un caractère de remplacement
 * Permet de retirer le '\n' final d'une entrée utilisateur
 * @param str la chaine dans laquelle il faut remplacer le '\n'
 * @param replacement le caractère de remplacement
 */
void replaceLineBreak(char * str, char replacement) {
	for(int i = 0; str[i] != '\0'; ++i) // On présume que la chaine est bien formée (donc pas de buffer overflow)
		if(str[i] == '\n') {
			str[i] = replacement;
			return;
		}
}

void clearInputBuffer() { // Permet de libérer le buffer de saisie
	int ch;
	do {
		ch = fgetc(stdin); // Lis un caractère de l'input standard (le clavier)
	}while(ch != '\n' && ch != EOF); // Tant que ce n'est pas \n ou EOF (Fin de fichier)
}
#endif //CAMELUP_CAMELUPTYPESANDUTILS_H
