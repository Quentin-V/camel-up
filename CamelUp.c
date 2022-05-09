#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define NB_COULEUR 5 // Nombre de couleurs / chameaux

#include "CamelUpTypesAndUtils.h"

// Tableau utile pour les couleurs
const char * couleurs[NB_COULEUR] = {
	"blanc",
	"jaune",
	"orange",
	"bleu",
	"vert"
};

// ******************************
// ***** VARIABLES GLOBALES *****
// ******************************

int nbJoueurs; // Nombre de joueurs

bool pyramide[NB_COULEUR]; // La pyramide qui contient les dés
int tuilesParis[NB_COULEUR]; // Le nombre de tuiles de pari manche de chaque couleur
Chameau chameaux[NB_COULEUR]; // Déclaration des chameaux
Parieur * parieurs; // Pointeur sur Parieur (tableau dynamique contenant les joueurs)
PariCourse ** parisCourse; // Tableau de pointeurs sur PariCourse (tableau pas encore alloué qui contiendra les paris de course de la partie)

// ************************************************
// ***** Utilitaires choix de l'action joueur *****
// ************************************************

/**
 * Affiche le menu de sélection d'action et attend l'entrée de l'utilisateur
 * @return le choix de l'utilisateur entre 1 et 4
 */
int choisirAction() {
	printf("Choisis une action parmi les suivantes:\n");
	printf("\t- 1 : Prendre une tuile pyramide et faire avancer un chameau\n");
	printf("\t- 2 : Faire un pari sur la manche\n");
	printf("\t- 3 : Faire un pari sur la course\n");
	printf("\t- 4 : (Dé)placer la tuile Désert\n");
	printf("Ton choix : ");
	int action;
	while(scanf_s("%d", &action) == 0 || action < 1 || action > 4) { // Tant que la saisie n'est pas valide
		printf("Choix invalide, choisis une action entre 1 et 4 : ");
		clearInputBuffer();
	}
	return action; // On retourne l'action choisie
}

/**
 * Permet de savoir si l'action choisie est valide
 * @param action l'action choisie
 * @param parieur le joueur qui a choisi l'action
 * @return vrai si le joueur peut effectuer l'action, faux sinon
 */
bool actionValide(int action, Parieur parieur) {
	return true; // TODO : Cette fonction
}

// *********************************
// ***** Fonctions des actions *****
// *********************************

/**
 * Gère l'arrivée d'un chameau sur une case dans le cas
 * où il arrive au dessus de la pile s'il y a déjà des chameaux
 * @param chameau le chameau qui bouge
 * @param position la nouvelle case
 */
void arriverChameauDessus(Chameau chameau, int position) {
	for(int i = 0; i < NB_COULEUR; ++i) {
		if(&chameaux[i] == &chameau) continue; // Ne pas effectuer la boucle sur le chameau qui bouge
		// Si le chameau arrive sur une case déjà occupée par un autre chameau, il monte sur la pile
		if(chameaux[i].position == position && chameaux[i].chameauSurLeDos == NULL)
			chameaux[i].chameauSurLeDos = &chameau;
	}
}

/**
 * Gère l'arrivée d'un chameau sur une case après être tombé
 * sur un mirage et donc il doit passer en dessous de la pile
 * s'il y a déjà des chameaux sur la case
 * @param chameau le chameau qui bouge
 * @param position la nouvelle case
 */
void arriverChameauDessous(Chameau chameau, int position) {
	Chameau * chameauDuDessous = NULL;
	for(int i = 0; i < NB_COULEUR; ++i) {
		// Ne pas faire la boucle sur le chameau qui bouge ou si le chameau n'est pas sur la même case
		if(&chameaux[i] == &chameau || chameaux[i].position != position) continue;
		// Si on a pas encore de chameau du dessous ou si le chameau du dessous est sur le dos de celui-ci, on remplace le chameau du dessous
		if(chameauDuDessous == NULL || chameauDuDessous == chameaux[i].chameauSurLeDos) chameauDuDessous = &chameaux[i];
	}
	// On fait passer notre pile de chameaux au dessous des chameaux sur la case d'arrivée
	Chameau * chameauCourant = &chameau;
	while(chameauCourant->chameauSurLeDos != NULL) chameauCourant = chameauCourant->chameauSurLeDos;
	chameau.chameauSurLeDos = chameauDuDessous;
}

/**
 * Donne une tuile pyramide à un joueur et fait avancer un chameau
 * @param parieur le joueur qui tire la tuile pyramide
 */
void prendreTuilePyramide(Parieur parieur) {
	++parieur.tuilesPyramide; // On commence par donner une tuile pyramide au joueur

	int randCouleur;
	do { // On tire au sort la couleur du chameau qui va avancer
		randCouleur = rand() % NB_COULEUR;
	}while(!pyramide[randCouleur]);

	int valeurDe = (rand() % 3) + 1; // Chiffre aléatoire entre 1 et 3
	pyramide[randCouleur] = false; // On sort le dé de la pyramide

	Chameau chameauQuiBouge = chameaux[randCouleur];
	int anciennePosition = chameauQuiBouge.position;

	// Retirer notre chameau du dos d'un chameau s'il est dessus
	for(int i = 0; i < NB_COULEUR; ++i) {
		if(i == randCouleur) continue; // Ne pas effectuer la boucle sur le chameau qui bouge
		// On retire le chameau qui va bouger du dos d'un autre chameau s'il est dessus
		if(chameaux[i].chameauSurLeDos == &chameauQuiBouge) chameaux[i].chameauSurLeDos = NULL;
	}

	chameauQuiBouge.position += valeurDe; // On fait avancer le chameau

	// Faire monter le chameau sur les autres s'il arrive sur une case déjà occupée
	arriverChameauDessus(chameauQuiBouge, chameauQuiBouge.position);

	// On gère le cas où on arrive sur une tuile désert
	// On ne peut pas retomber sur une tuile désert après, car elles ne peuvent pas être placées côte à côte
	for(int i = 0; i < nbJoueurs; ++i) {
		if(parieurs[i].tuileDesert.position != chameauQuiBouge.position) continue; // Si le chameau n'est pas sur la tuile, on passe à la suivante
		TuileDesert tuileDesert = parieurs[i].tuileDesert;
		++parieurs[i].or; // On donne 1 d'or au joueur à qui appartient la tuile
		if(tuileDesert.coteOasis) { // Côté Oasis de la tuile désert
			++chameauQuiBouge.position;
			arriverChameauDessus(chameauQuiBouge, chameauQuiBouge.position);
		}else { // Côté Mirage de la tuile désert
			--chameauQuiBouge.position;
			arriverChameauDessous(chameauQuiBouge, chameauQuiBouge.position);
		}
	}

	if(anciennePosition != chameauQuiBouge.position) { // Si on a pas changé de position, pas besoin
		// On n'oublie pas de faire avancer les chameaux qu'on a sur le dos
		Chameau * chameauCourant = &chameauQuiBouge;
		while(chameauCourant->chameauSurLeDos != NULL) {
			chameauCourant->chameauSurLeDos->position = chameauCourant->position;
			chameauCourant = chameauCourant->chameauSurLeDos;
		}
	}

	printf("Le dé %s est sorti de la pyramide avec une valeur de %d\n", couleurs[randCouleur], valeurDe);
}

/**
 * Permet à un parieur de parier sur le chameau gagnant de la manche en cours
 * @param parieur le joueur qui va parier sur un chameau
 */
void pariManche(Parieur parieur) {
	printf("%s, quelle tuile de pari veux-tu prendre:\n", parieur.nom);
	for(int i = 0; i < NB_COULEUR; ++i) {
		printf("\t- %6s (%d), ", couleurs[i], i+1);
		switch (tuilesParis[i]) {
			case 0:
				printf("plus de tuiles disponibles");
				break;
			case 1:
				printf("reste 1 tuile : (1er -> +2, 2ème -> +1, 3 derniers -> -1)");
				break;
			case 2:
				printf("reste 2 tuile : (1er -> +3, 2ème -> +1, 3 derniers -> -1)");
				break;
			case 3:
				printf("reste 3 tuile : (1er -> +5, 2ème -> +1, 3 derniers -> -1)");
				break;
		}
		printf("\n");
	}
	printf("Quel est ton choix : ");
	int choix;
	//      Tant que saisie incohérente     ou     hors des bornes             ou qu'il n'y a plus de tuiles
	while(scanf_s("%d", &choix) == 0 || choix < 1 || choix > NB_COULEUR || tuilesParis[choix-1] < 1) {
		printf("Choix invalide, choisis une tuile parmi les disponibles : ");
		clearInputBuffer();
	}
	--choix; // On enlève 1 à choix, car l'utilisateur a saisi entre 1 et 5 et on veut entre 0 et 4
	parieur.parisManche[parieur.nbParisManche++] = &(PariManche) {
		.valeurPari = tuilesParis[choix] == 3 ? 5 : tuilesParis[choix],
		.couleur = choix
	};
	printf("Pari sur le chameau %s pour une valeur de %d enregistré\n", couleurs[choix], tuilesParis[choix] == 3 ? 5 : tuilesParis[choix]);
	--tuilesParis[choix]; // On retire une tuile de pari de la couleur choisie
}

/**
 * Permet à un parieur de parier sur la victoire ou défaite d'un chameau à la course
 * @param parieur le joueur qui va parier sur le chameau
 */
void pariCourse(Parieur parieur) {
	printf("%s, sur quel chameau veux tu parier ?\n", parieur.nom);
	for(int i = 0; i < NB_COULEUR; ++i)
		printf("\t - Chameau %-6s (%d) %s\n", couleurs[i], i+1, parieur.tuilesPariCourse[i] ? "" : "(Indisponible)");
	printf("Ton choix : ");
	int choix;
	// Tant que saisie incohérente ou     hors des bornes             ou qu'il n'y a plus de tuiles
	while(scanf_s("%d", &choix) == 0 || choix < 1 || choix > NB_COULEUR || !parieur.tuilesPariCourse[choix]) {
		printf("Choix invalide, choisis une tuile parmi les disponibles : ");
		clearInputBuffer();
	}
	--choix; // On enlève 1 à choix, car l'utilisateur a saisi entre 1 et 5 et on veut entre 0 et 4
	printf("Penses-tu que le chameau %s va gagner ou perdre la course?\n", couleurs[choix]);
	printf("Ton choix (G/P) : ");
	char choixGagnerPerdre;
	while(scanf_s("%c", &choixGagnerPerdre) == 0 || // Tant que saisie incohérente ou que
		(choixGagnerPerdre != 'G' && choixGagnerPerdre != 'g' && // le choix est différent de G ou P
		 choixGagnerPerdre != 'P' && choixGagnerPerdre != 'p')) {
		printf("Choix invalide,tape G pour un pari de victoire et P pour un pari de défaite : ");
		clearInputBuffer();
	}

	parieur.tuilesPariCourse[choix] = false; // On retire la tuile de pari au joueur
	PariCourse pc = (PariCourse) { // On crée le pari de course
		.parieur = parieur,
		.couleur = choix,
		.victorieux = choixGagnerPerdre == 'G' || choixGagnerPerdre == 'g'
	};
	for(int index = 0; index < nbJoueurs * NB_COULEUR; ++index) { // Pour chaque case du tableau parisCourse
		if(parisCourse[index] == NULL) { // Dès qu'on trouve un élément à NULL
			parisCourse[index] = &pc; // On affecte le pari à cette case
			break; // On sort du for
		}
	}
	printf("Pari de course de %s enregistré", parieur.nom);
}

/**
 * Permet de savoir si la position de la tuile désert est correcte
 * @param position la position demandée pour la tuile désert
 * @return true si la position est valide, faux sinon
 */
bool validePositionDesert(int position) {
	if(position == 1) return false; // On ne peut pas poser en 1
	for(int i = 0; i < nbJoueurs; ++i) { // On teste les contraintes avec toutes les tuiles en place
		TuileDesert tuileDesert = parieurs[i].tuileDesert;
		if(tuileDesert.position == -1) continue; // Tuile pas posée
		// Si on pose dans une case adjascente à la tuile en place (position-1 <= tuileDesert.position <= position+1)
		if(position - 1 <= tuileDesert.position && tuileDesert.position <= position + 1) return false;
	}
	return true;
}

/**
 * Permet à un joueur déplacer sa tuile désert
 * @param parieur le joueur qui pose sa tuile
 */
void placerTuileDesert(Parieur parieur) {
	printf("%s, où souhaites-tu placer ta tuile désert ?\n", parieur.nom);
	printf("Ton choix : ");
	int choixPosition;
	while(scanf_s("%d", &choixPosition) == 0 || !validePositionDesert(choixPosition)) {
		printf("Position invalide, choisis une position valide : ");
		clearInputBuffer();
	}
	choixPosition--; // Comme d'hab
	parieur.tuileDesert.position = choixPosition;
	printf("De quel côté veux-tu placer ta tuile désert ?\n");
	printf("Mirage ou Oasis (M/O) : ");
	char choixCote;
	while(scanf_s("%c", &choixCote) == 0 || (choixCote != 'M' && choixCote != 'm' && choixCote != 'O' && choixCote != 'o'))  {
		printf("Choix invalide, tape M pour le côté mirage et O pour le côté oasis : ");
		clearInputBuffer();
	}
}

// ****************************
// ***** Fonction du tour *****
// ****************************

/**
 * Démarre le tour d'un joueur
 * @param parieur le joueur qui joue son tour
 */
void tour(Parieur parieur) {
	printf("C'est au tour de %s!\n", parieur.nom);
	int action;
	do {
		action = choisirAction();
	}while(!actionValide(action, parieur));

	switch (action) {
		case 1: // Action de prendre une tuile pyramide
			prendreTuilePyramide(parieur);
			break;

		case 2:
			pariManche(parieur);
			break;

		case 3:
			pariCourse(parieur);
			break;

		case 4:
			placerTuileDesert(parieur);
			break;

		default: break;
	}
}

// *********************************
// ***** Utilitaires de manche *****
// *********************************

/**
 * Fonction qui reset les variables pour le début des manches
 */
void debutManche() {
	// On remet tous les dés dans la pyramide
	for(int i = 0; i < NB_COULEUR; ++i) {
		pyramide[i] = true; // On remet le dé dans la pyramide
		tuilesParis[i] = 3; // 3 tuiles de paris de chaque au début de manche
	}

	for(int i = 0; i < nbJoueurs; ++i) { // Réinitialisation des paris de manche
		for(int j = 0; j < 3 * NB_COULEUR; ++j)
			parieurs[i].parisManche[j] = NULL;
		parieurs[i].tuileDesert.position = -1; // On retire les tuiles désert du pateau
	}
}

/**
 * Fonction qui s'exécute à la fin de la manche pour effectuer les actions de comptage des points
 */
void finManche() {
	for(int i = 0; i < nbJoueurs; ++i) {
		parieurs[i].or += parieurs[i].tuilesPyramide;
		parieurs[i].tuilesPyramide = 0;
	}
}

/**
 * Permet de savoir si la manche en cours est terminée
 * S'il n'y a plus de dés dans la pyramide, la manche est terminée
 * @return true si la manche est terminée, faux sinon
 */
bool mancheEstTerminee() {
	return !pyramide[0] && !pyramide[1] && !pyramide[2] && !pyramide[3] && !pyramide[4];
}

/**
 * Permet de savoir si la partie est terminée
 * Si un chameau a dépassé la case 16, la partie est finie
 * @return vrai si la partie est terminée, faux sinon
 */
bool partieEstFinie() {
	for(int i = 0; i < NB_COULEUR; ++i)
		if(chameaux[i].position > 16) return true;
	return false;
}

/**
 * Fonction qui demande le nombre de joueurs
 * et qui l'affecte à la variable globale
 */
void demandeNombreJoueurs() {
	printf("Veuillez entrer le nombre de joueurs (Entre 3 et 8) : ");
	// Tant que le scanf est incohérent ou que la valeur entrée n'est pas entre 3 et 8
	while(scanf_s("%d", &nbJoueurs) == 0 || !(nbJoueurs >= 3 && nbJoueurs <= 8)) {
		printf("Saisie invalide, veuillez saisir un nombre entre 3 et 8 : ");
		clearInputBuffer(); // Au cas où la saisie est invalide
	}
	fgetc(stdin); // Traiter le \n restant du buffer après la dernière saisie
	printf("La partie se déroulera avec %d joueurs\n", nbJoueurs);
}

/**
 * Fonction qui initialise les joueurs
 */
void initialiserJoueurs() {
	// Initialisation des joueurs
	parieurs = malloc(nbJoueurs * sizeof(Parieur));
	for(int i = 0; i < nbJoueurs; ++i) {
		printf("Entrer le nom du joueur %d (20 caractères maximum) : ", i+1); // Demande du nom
		// fgets → Comme scanf avec des restrictions
		//  var d'affectation | taille maximale input | input
		fgets(parieurs[i].nom, sizeof(parieurs[i].nom), stdin);
		if(!containsLineBreak(parieurs[i].nom)) clearInputBuffer(); // Clear le buffer si nécessaire
		replaceLineBreak(parieurs[i].nom, '\0'); // Enlève le retour à la ligne du nom du joueur
		parieurs[i].or = 3;
		parieurs[i].tuilesPyramide = 0;
		parieurs[i].nbParisManche = 0;
		for(int j = 0; j < NB_COULEUR; ++j) parieurs[i].tuilesPariCourse[j] = true;
		parieurs[i].tuileDesert = (TuileDesert) { // Initialisation de la tuile désert
				.position = -1,
				.coteOasis = true
		};
	}
}

/**
 * Fonction qui initialise les chameaux
 */
void initialiserChameaux() {
	for(int i = 0; i < NB_COULEUR; ++i) {
		chameaux[i].position = 0;
		chameaux[i].chameauSurLeDos = NULL;
		chameaux[i].couleur = &couleurs[i];
	}
}

/**
 * Fonction qui initialise les paris course
 */
void initialiserParisCourse() {
	parisCourse = malloc(nbJoueurs * NB_COULEUR * sizeof(PariCourse)); // Le tableau pourra contenir 5 paris course par joueur au maximum
	for(int i = 0; i < nbJoueurs * NB_COULEUR; ++i) parisCourse[i] = NULL;
}

/**
 * Effectue toutes les actions nécessaires au début de la partie
 */
void debutDePartie() {
	for(int i = 0; i < NB_COULEUR; ++i) {
		chameaux[i].position = rand()%3 + 1; // On place le chameau au hasard entre la case 1 et 3
		for(int j = i-1; j > 0; --j) { // On vérifie s'il y a déjà un chameau sur la case
			// Si les chameaux ne sont pas sur la même case, on continue
			if(chameaux[j].position != chameaux[i].position) continue;

			if(rand()%2 == 0) // On pose aléatoirement le premier ou deuxième chameau sur le dos de l'autre
				chameaux[i].chameauSurLeDos = &chameaux[j];
			else
				chameaux[j].chameauSurLeDos = &chameaux[i];
		}
	}
}

/**
 * Fonction principale qui gère le jeu
 * @return 0 si tout va bien
 */
int main() {
	srand(time(NULL)); // On définit la seed du random (point de départ de la fonction random) pour les dés etc...

	demandeNombreJoueurs();

	initialiserJoueurs();
	initialiserChameaux();
	initialiserParisCourse();

	debutDePartie();

	// Boucle de partie
	int compteurTour = rand()%nbJoueurs; // Choix aléatoire du premier joueur
	while(!partieEstFinie()) { // partie pas finie
		debutManche();
		while(!mancheEstTerminee()) { // manche en cours
			tour(parieurs[compteurTour++%nbJoueurs]); // On fait le tour d'un joueur
		}
		finManche();
	}

	return 0;
}