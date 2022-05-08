//
// Created by Quentin on 08/05/2022.
//

#ifndef CAMELUP_CAMELUPTYPESANDUTILS_H
#define CAMELUP_CAMELUPTYPESANDUTILS_H

#include <stdbool.h>
#include <stdio.h>

/**
 * Déclaration des types
 */
typedef struct Chameau {
	int position; // Position du chameau sur le plateau
	char ** couleur; // Chaine de caractère de la couleur du chameau
	struct Chameau * chameauSurLeDos; // Pointeur sur le chameau qui est sur son dos s'il existe
} Chameau;

typedef struct PariManche {
	int couleur;
	int valeurPari;
} PariManche;

typedef struct TuileDesert {
	bool coteOasis;
	int position;
} TuileDesert;

// Structure représentant le joueur
typedef struct Parieur {
	char nom[21]; // 20 caracères + \0 de fin de chaine
	int or;
	int tuilesPyramide;
	int nbParisManche;
	PariManche *parisManche[NB_COULEUR * 3]; // Tableau de pointeurs sur des PariManche
	bool tuilesPariCourse[NB_COULEUR];
	TuileDesert tuileDesert;
} Parieur;

typedef struct PariCourse {
	int couleur;
	bool victorieux;
	Parieur parieur;
} PariCourse;

// Un booléen c'est VRAI ou FAUX
bool containsLineBreak(const char * str) {
	for(int i = 0; str[i] != '\0'; ++i) { // On assume que la chaine est bien formée (donc pas de bufferoverflow)
		if(str[i] == '\n') return true;
	}
	return false;
}

void removeLineBreak(char * str) {
	for(int i = 0; str[i] != '\0'; ++i) // On assume que la chaine est bien formée (donc pas de bufferoverflow)
		if(str[i] == '\n') {
			str[i] = '\0';
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
