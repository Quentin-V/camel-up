#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

#define NB_COULEUR 5 // Nombre de couleurs / chameaux
#define TAILLE_PLATEAU 16// Nombre de cases sur le plateau

#include "CamelUpTypesAndUtils.h"

// Tableau utile pour les couleurs
const char * couleurs[NB_COULEUR] = {
	"Blanc",
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
PariCourse * parisCourse; // Tableau de pointeurs sur PariCourse (tableau pas encore alloué qui contiendra les paris de course de la partie)
int nbParisCourse;
Case casesPlateau[TAILLE_PLATEAU];

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
	fgetc(stdin); // Clear le '\n' restant du buffer
	return action; // On retourne l'action choisie
}

/**
 * Permet de savoir si l'action choisie est valide
 * @param action l'action choisie
 * @param parieur le joueur qui a choisi l'action
 * @return vrai si le joueur peut effectuer l'action, faux sinon
 */
bool actionValide(int action, Parieur * parieur) {
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
void arriverChameauDessus(Chameau * chameau, int position) {
	Chameau * chameauSurMemeCase = NULL;
	for(int i = 0; i < NB_COULEUR; ++i) { // On essaie de trouver un chameau sur la case d'arrivée
		if(&chameaux[i] == chameau) continue; // Ne pas effectuer la boucle sur le chameau qui bouge
		if(chameaux[i].position == position) {
			chameauSurMemeCase = &chameaux[i];
			break; // On a trouvé un chameau, on peut sortie de la boucle
		}
	}
	if(chameauSurMemeCase == NULL) return; // Si la case d'arrivée est vide, pas besoin de monter sur un chameau
	Chameau * chameauDuHaut = trouverChameauDuHaut(chameauSurMemeCase);
	chameauDuHaut->chameauSurLeDos = chameau;
	chameau->chameauDessous = chameauDuHaut;
}

/**
 * Gère l'arrivée d'un chameau sur une case après être tombé
 * sur un mirage et donc il doit passer en dessous de la pile
 * s'il y a déjà des chameaux sur la case
 * @param chameau le chameau qui bouge
 * @param position la nouvelle case
 */
void arriverChameauDessous(Chameau * chameau, int position) {
	Chameau * chameauSurMemeCase = NULL;
	for(int i = 0; i < NB_COULEUR; ++i) {
		// Ne pas faire la boucle sur le chameau qui bouge ou si le chameau n'est pas sur la même case
		if(&chameaux[i] == chameau) continue;
		if(chameaux[i].position == position) {
			chameauSurMemeCase = &chameaux[i];
			break; // On a trouvé un chameau on peut sortir de la boucle
		}
	}
	// On fait passer notre pile de chameaux au dessous des chameaux sur la case d'arrivée
	Chameau * chameauDuBas = trouverChameauDuBas(chameauSurMemeCase);
	Chameau * chameauDuHautPileCourante = trouverChameauDuHaut(chameau);
	chameauDuBas->chameauDessous = chameauDuHautPileCourante;
	chameauDuHautPileCourante->chameauSurLeDos = chameauDuBas;
}

/**
 * Donne une tuile pyramide à un joueur et fait avancer un chameau
 * @param parieur le joueur qui tire la tuile pyramide
 */
void prendreTuilePyramide(Parieur * parieur) {
	++parieur->tuilesPyramide; // On commence par donner une tuile pyramide au joueur

	int randCouleur;
	do { // On tire au sort la couleur du chameau qui va avancer
		randCouleur = rand() % NB_COULEUR;
	}while(!pyramide[randCouleur]);

	int valeurDe = (rand() % 3) + 1; // Chiffre aléatoire entre 1 et 3
	pyramide[randCouleur] = false; // On sort le dé de la pyramide

	Chameau * chameauQuiBouge = &chameaux[randCouleur];
	int anciennePosition = chameauQuiBouge->position;

	// Retirer notre chameau du dos d'un chameau s'il est dessus
	if(chameauQuiBouge->chameauDessous != NULL) {
		Chameau * chameauDessous = chameauQuiBouge->chameauDessous;
		chameauDessous->chameauSurLeDos = NULL;
		chameauQuiBouge->chameauDessous = NULL;
	}

	chameauQuiBouge->position += valeurDe; // On fait avancer le chameau

	// Faire monter le chameau sur les autres s'il arrive sur une case déjà occupée
	arriverChameauDessus(chameauQuiBouge, chameauQuiBouge->position);

	printf("Le dé %s est sorti de la pyramide avec une valeur de %d\n", couleurs[randCouleur], valeurDe);

	// On gère le cas où on arrive sur une tuile désert
	// On ne peut pas retomber sur une tuile désert après, car elles ne peuvent pas être placées côte à côte
	for(int i = 0; i < nbJoueurs; ++i) {
		if(parieurs[i].tuileDesert.position != chameauQuiBouge->position) continue; // Si le chameau n'est pas sur la tuile, on passe à la suivante
		TuileDesert tuileDesert = parieurs[i].tuileDesert;
		++parieurs[i].or; // On donne 1 d'or au joueur à qui appartient la tuile
		printf("Le chameau est tombé sur une tuile désert, %s gagne immédiatement une livre égyptienne\n", parieurs[i].nom);
		if(tuileDesert.coteOasis) { // Côté Oasis de la tuile désert
			++chameauQuiBouge->position;
			arriverChameauDessus(chameauQuiBouge, chameauQuiBouge->position);
			printf("Le chameau avance d'une case supplémentaire\n");
		}else { // Côté Mirage de la tuile désert
			--chameauQuiBouge->position;
			arriverChameauDessous(chameauQuiBouge, chameauQuiBouge->position);
			printf("Le chameau recule d'une case\n");
		}
	}

	if(anciennePosition != chameauQuiBouge->position) { // Si on a pas changé de position, pas besoin
		// On n'oublie pas de faire avancer les chameaux qu'on a sur le dos
		Chameau * chameauCourant = chameauQuiBouge;
		while(chameauCourant->chameauSurLeDos != NULL) {
			chameauCourant->chameauSurLeDos->position = chameauCourant->position;
			chameauCourant = chameauCourant->chameauSurLeDos;
		}
	}
}

/**
 * Permet à un parieur de parier sur le chameau gagnant de la manche en cours
 * @param parieur le joueur qui va parier sur un chameau
 */
void pariManche(Parieur * parieur) {
	printf("%s, quelle tuile de pari veux-tu prendre:\n", parieur->nom);
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
	 fgetc(stdin); // Traitement du \n restant
	--choix; // On enlève 1 à choix, car l'utilisateur a saisi entre 1 et 5 et on veut entre 0 et 4
	parieur->parisManche[parieur->nbParisManche++] = (PariManche) {
			.couleur = choix,
			.valeurPari = tuilesParis[choix] == 3 ? 5 : tuilesParis[choix]
	};
	printf("Pari sur le chameau %s pour une valeur de %d enregistré\n", couleurs[choix], tuilesParis[choix] == 3 ? 5 : tuilesParis[choix]+1);
	--tuilesParis[choix]; // On retire une tuile de pari de la couleur choisie
}

/**
 * Permet à un parieur de parier sur la victoire ou défaite d'un chameau à la course
 * @param parieur le joueur qui va parier sur le chameau
 */
void pariCourse(Parieur * parieur) {
	printf("%s, sur quel chameau veux tu parier ?\n", parieur->nom);
	for(int i = 0; i < NB_COULEUR; ++i)
		printf("\t - Chameau %-6s (%d) %s\n", couleurs[i], i+1, parieur->tuilesPariCourse[i] ? "" : "(Indisponible)");
	printf("Ton choix : ");
	int choix;
	// Tant que saisie incohérente          ou     hors des bornes             ou qu'il n'y a plus de tuiles
	while(scanf_s("%d", &choix) == 0 || choix < 1 || choix > NB_COULEUR || !parieur->tuilesPariCourse[choix-1]) {
		printf("Choix invalide, choisis une tuile parmi les disponibles : ");
		clearInputBuffer();
	}
	fgetc(stdin); // Traiter le '\n' restant
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
	fgetc(stdin); // Traiter le '\n' restant

	parieur->tuilesPariCourse[choix] = false; // On retire la tuile de pari au joueur
	PariCourse pc = (PariCourse) { // On crée le pari de course
		.parieur = parieur,
		.couleur = choix,
		.victorieux = choixGagnerPerdre == 'G' || choixGagnerPerdre == 'g'
	};
	parisCourse[nbParisCourse++] = pc;
	printf("Pari de course de %s enregistré\n", parieur->nom);
}

/**
 * Permet de savoir si la position de la tuile désert est correcte
 * @param position la position demandée pour la tuile désert
 * @return true si la position est valide, faux sinon
 */
bool validePositionDesert(int position, Parieur * parieur) {
	if(position == 0 || position > 15) return false; // On ne peut pas poser en 1 ni au dela du plateau
	for(int i = 0; i < nbJoueurs; ++i) { // On teste les contraintes avec toutes les tuiles en place
		if(&parieurs[i] == parieur) continue; // On ne vérifie pas sa propre tuile
		TuileDesert tuileDesert = parieurs[i].tuileDesert;
		if(tuileDesert.position == -1) continue; // Tuile pas posée
		// Si on pose dans une case adjascente à la tuile en place (position-1 <= tuileDesert.position <= position+1)
		if(position - 1 <= tuileDesert.position && tuileDesert.position <= position + 1) return false;
	}
	// Pour ne pas placer sur un chameau
	for(int i = 0; i < NB_COULEUR; ++i) if(position == chameaux[i].position) return false;
	return true;
}

/**
 * Permet à un joueur déplacer sa tuile désert
 * @param parieur le joueur qui pose sa tuile
 */
void placerTuileDesert(Parieur * parieur) {
	printf("%s, où souhaites-tu placer ta tuile désert ?\n", parieur->nom);
	printf("Ton choix : ");
	int choixPosition;
	while(scanf_s("%d", &choixPosition) == 0 || !validePositionDesert(choixPosition-1, parieur)) {
		printf("Position invalide, choisis une position valide : ");
		clearInputBuffer();
	}
	fgetc(stdin); // On traite le \n restant du buffer
	choixPosition--; // Comme d'hab
	parieur->tuileDesert.position = choixPosition;
	printf("De quel côté veux-tu placer ta tuile désert ?\n");
	printf("Mirage ou Oasis (M/O) : ");
	char choixCote;
	while(scanf_s("%c", &choixCote) == 0 || (choixCote != 'M' && choixCote != 'm' && choixCote != 'O' && choixCote != 'o'))  {
		printf("Choix invalide, tape M pour le côté mirage et O pour le côté oasis : ");
		clearInputBuffer();
	}
	fgetc(stdin); // Comme d'hab

	// On affecte le côté choisi de la carte
	parieur->tuileDesert.coteOasis = choixCote == 'O' || choixCote == 'o';
	printf("Tuile désert placée à la position %d sur le côté %s!\n", parieur->tuileDesert.position+1, parieur->tuileDesert.coteOasis ? "Oasis" : "Mirage");
}

// ****************************
// ***** Fonction du tour *****
// ****************************

/**
 * Démarre le tour d'un joueur
 * @param parieur le joueur qui joue son tour
 */
void tour(Parieur * parieur) {
	printf("C'est au tour de %s!\n", parieur->nom);
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

// **********************************************
// ***** Utilitaires de manche et de partie *****
// **********************************************

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
		parieurs[i].nbParisManche = 0;
		parieurs[i].tuileDesert.position = -1; // On retire les tuiles désert du pateau
	}
}

/**
 * Fonction qui s'exécute à la fin de la manche pour effectuer les actions de comptage des points
 */
void finManche() {
	int classement[5];
	trouverClassement(chameaux, classement);

	printf("Fin de la manche!\n");
	printf("Classement : \n");
	for(int i = 0; i < NB_COULEUR; ++i) {
		printf("\t%d : %s\n", i+1, couleurs[classement[i]]);
	}
	printf("Les joueurs gagnent une livre égyptienne pour chaque tuile pyramide en leur possession\n");
	printf("Résultat des paris :\n");
	for(int i = 0; i < nbJoueurs; ++i) {
		// On donne une livre par tuile pyramide
		parieurs[i].or += parieurs[i].tuilesPyramide;
		parieurs[i].tuilesPyramide = 0;
		// On calcule le résultat des paris
		printf("\t%s : ", parieurs[i].nom);
		printf("%s\n", parieurs[i].nbParisManche == 0 ? "Pas de paris" : "");
		for(int j = 0; j < parieurs[i].nbParisManche; ++j) {
			PariManche pm = parieurs[i].parisManche[j];
			printf("\t\t Pari sur le chameau %s : ", couleurs[pm.couleur]);
			if(pm.couleur == classement[0] || pm.couleur == classement[1]) { // Pari gagné
				bool premier = pm.couleur == classement[0];
				int gain = premier ? pm.valeurPari : 1;
				printf("Gagné (+%d livre(s) égyptienne(s))", gain);
				parieurs[i].or += gain;
			}else { // Pari perdu
				printf("Perdu (-1 livre égyptienne");
			}
			printf("\n");
		}
	}
	attendreInput("Appuyez sur entrée pour passer à la manche suivante");
}

/**
 * Effectue toutes les actions nécessaires au début de la partie
 */
void debutPartie() {
	for(int i = 0; i < NB_COULEUR; ++i) {
		chameaux[i].position = rand() % 3; // On place le chameau au hasard entre la case 1 et 3 (position 0 à 2)
		for(int j = i-1; j >= 0; --j) { // On vérifie s'il y a déjà un chameau sur la case
			// Si les chameaux ne sont pas sur la même case, on continue
			if(chameaux[j].position != chameaux[i].position) continue;

			bool vaDessus = rand() % 2 == 0;
			if(vaDessus) { // On pose aléatoirement le nouveau chameau par dessus ou en dessous du groupe de chameau sur la case
				Chameau * chameauDuHaut = trouverChameauDuHaut(&chameaux[j]);
				chameauDuHaut->chameauSurLeDos = &chameaux[i];
				chameaux[i].chameauDessous = chameauDuHaut;
				break;
			}else {
				Chameau * chameauDuBas = trouverChameauDuBas(&chameaux[j]);
				chameauDuBas->chameauDessous = &chameaux[i];
				chameaux[i].chameauSurLeDos = chameauDuBas;
				break;
			}
		}
	}
}

void finPartie() {
	printf("\n\nPartie terminée!\n");

	int classement[5];
	trouverClassement(chameaux, classement);

	int nbParisGagnes = 0;
	printf("Comptage des paris de victoire de course :\n");
	for(int i = 0; i < nbParisCourse; ++i) {
		if(!parisCourse[i].victorieux) continue;
		PariCourse pc = parisCourse[i];
		int gainOuPerte = classement[0] != pc.couleur ? -1 : nbParisGagnes == 0 ? 8 : nbParisGagnes == 1 ? 5 : nbParisGagnes == 2 ? 3 : nbParisGagnes == 3 ? 2 : 1;
		printf("\tPari de victoire de %s sur le chameau %s : %s (%+d livres égyptiennes)\n",
			   pc.parieur->nom,
			   couleurs[pc.couleur],
			   classement[0] == pc.couleur ? "Gagné" : "Perdu",
			   gainOuPerte
			   );
		pc.parieur->or += gainOuPerte;
		if(gainOuPerte > 0) ++nbParisGagnes;
	}

	nbParisGagnes = 0;
	printf("Comptage des paris de défaite de course :\n");
	for(int i = 0; i < nbParisCourse; ++i) {
		if(parisCourse[i].victorieux) continue;
		PariCourse pc = parisCourse[i];
		int gainOuPerte = classement[4] != pc.couleur ? -1 : nbParisGagnes == 0 ? 8 : nbParisGagnes == 1 ? 5 : nbParisGagnes == 2 ? 3 : nbParisGagnes == 3 ? 2 : 1;
		printf("\tPari de défaite de %s sur le chameau %s : %s (%+d livres égyptiennes)\n",
		       pc.parieur->nom,
		       couleurs[pc.couleur],
		       classement[4] == pc.couleur ? "Gagné" : "Perdu",
		       gainOuPerte
		);
		pc.parieur->or += gainOuPerte;
		if(gainOuPerte > 0) ++nbParisGagnes;
	}

	Parieur * vainqueur = &parieurs[0];
	for(int i = 1; i < nbJoueurs; ++i)
		if(parieurs[i].or > vainqueur->or) vainqueur = &parieurs[i];
	printf("Vainqueur de la partie : %s\n", vainqueur->nom);
	printf("Infos de tous les joueurs : \n");
	for(int i = 0; i < nbJoueurs; ++i) {
		printf("\t%s : %d livres égyptiennes\n", parieurs[i].nom, parieurs[i].or);
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
		if(chameaux[i].position >= 16) return true;
	return false;
}

// *******************************************
// ***** Utilitaires de d'initialisation *****
// *******************************************

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
		chameaux[i].couleur = i;
		chameaux[i].chameauSurLeDos = NULL;
		chameaux[i].chameauDessous = NULL;
	}
}

/**
 * Fonction qui initialise les paris course
 */
void initialiserParisCourse() {
	parisCourse = malloc(nbJoueurs * NB_COULEUR * sizeof(PariCourse)); // Le tableau pourra contenir 5 paris course par joueur au maximum
	nbParisCourse = 0;
}

/**
 * Initialise la plateau de jeu
 */
void initialiserPlateau() {
	for(int i = 0; i < TAILLE_PLATEAU; ++i) {
		Case c = (Case) {
				.lignes = {
						malloc(9 * sizeof(char)),
						malloc(9 * sizeof(char)),
						malloc(9 * sizeof(char)),
						malloc(9 * sizeof(char)),
						malloc(9 * sizeof(char))
				}
		};
		// On met toutes les lignes à des lignes vides pour commencer
		for(int j = 0; j < NB_COULEUR; ++j) sprintf(c.lignes[j], "%s", ligneVide);
		casesPlateau[i] = c;
	}
}

/**
 * Fonction principale qui gère le jeu
 * @return 0 si tout va bien
 */
int main() {
	// Permet d'afficher les unicodes et accents etc...
	system("chcp 65001");

	srand(time(NULL)); // On définit la seed du random (point de départ de la fonction random) pour les dés etc...

	demandeNombreJoueurs();

	initialiserJoueurs();
	initialiserChameaux();
	initialiserParisCourse();
	initialiserPlateau();

	debutPartie();

	// Boucle de partie
	int compteurTour = rand()%nbJoueurs; // Choix aléatoire du premier joueur
	while(!partieEstFinie()) { // partie pas finie
		debutManche();
		while(!mancheEstTerminee() && !partieEstFinie()) { // manche en cours
			afficherPlateau();
			tour(&parieurs[compteurTour++%nbJoueurs]); // On fait le tour d'un joueur
			attendreInput("Appuyez sur entrée pour passer au tour suivant...");
		}
		afficherPlateau();
		finManche();
	}

	finPartie();

	return 0;
}

// *********************************
// ***** Fonctions d'affichage *****
// *********************************

/**
 * Genère les lignes des cases pour l'affichage console
 * @param position la position, de la case à générer
 */
Case genererCase(int position) {
	Case c = casesPlateau[position];

	int nbChameauxSurCase = 0;
	Chameau * chameauBasDeCase = NULL;
	TuileDesert * tuileSurCase = NULL;
	Parieur * proprietaireTuile = NULL;
	for(int i = 0; i < NB_COULEUR; ++i) {
		if(chameaux[i].position%16 == position) { // Si un chameau est sur la position
			// On affecte le chameau de bas de case au premier trouvé
			if(chameauBasDeCase == NULL) chameauBasDeCase = &chameaux[i];
			++nbChameauxSurCase;
		}
	}
	for(int i = 0; i < nbJoueurs; ++i) {
		if(parieurs[i].tuileDesert.position == position) { // Si une tuile désert est sur cette position
			tuileSurCase = &parieurs[i].tuileDesert;
			proprietaireTuile = &parieurs[i];
			break;
		}
	}
	if(tuileSurCase != NULL) { // Si la case a une tuile
		sprintf(c.lignes[2], " %6s  ", tuileSurCase->coteOasis ? "Oasis" : "Mirage");
		if(strlen(proprietaireTuile->nom) > 7)
			sprintf(c.lignes[3], " %.6s. ", proprietaireTuile->nom);
		else
			sprintf(c.lignes[3], " %7s ", proprietaireTuile->nom);
		return c; // Pas besoin de faire les chameaux, car ils ne peuvent pas être sur la même case
	}
	// On vérifie que le chameau est bien celui du bas
	chameauBasDeCase = trouverChameauDuBas(chameauBasDeCase);

	// On vide les cases (car le contenu peut avoir changé)
	for(int i = 0; i < NB_COULEUR; ++i) sprintf(c.lignes[i], "%s", ligneVide);

	switch (nbChameauxSurCase) {
		case 1:
			sprintf(c.lignes[2], ligneFormat, couleurs[chameauBasDeCase->couleur][0]);
			break;
		case 4:
			sprintf(c.lignes[0], ligneFormat, couleurs[chameauBasDeCase->chameauSurLeDos->chameauSurLeDos->chameauSurLeDos->couleur][0]);
			// Pas de break, car on va exécuter les mêmes lignes que pour 3 et 2 chameaux
		case 3:
			sprintf(c.lignes[1], ligneFormat, couleurs[chameauBasDeCase->chameauSurLeDos->chameauSurLeDos->couleur][0]);
			// Pas de break, car on va exécuter les 2 mêmes lignes que pour 2 chameaux
		case 2:
			sprintf(c.lignes[2], ligneFormat, couleurs[chameauBasDeCase->chameauSurLeDos->couleur][0]);
			sprintf(c.lignes[3], ligneFormat, couleurs[chameauBasDeCase->couleur][0]);
			break;
		case 5:
			sprintf(c.lignes[0], ligneFormat, couleurs[chameauBasDeCase->chameauSurLeDos->chameauSurLeDos->chameauSurLeDos->chameauSurLeDos->couleur][0]);
			sprintf(c.lignes[1], ligneFormat, couleurs[chameauBasDeCase->chameauSurLeDos->chameauSurLeDos->chameauSurLeDos->couleur][0]);
			sprintf(c.lignes[2], ligneFormat, couleurs[chameauBasDeCase->chameauSurLeDos->chameauSurLeDos->couleur][0]);
			sprintf(c.lignes[3], ligneFormat, couleurs[chameauBasDeCase->chameauSurLeDos->couleur][0]);
			sprintf(c.lignes[4], ligneFormat, couleurs[chameauBasDeCase->couleur][0]);
			break;
		default: break; // Normalement on devrait pas y arriver
	}
	return c;
}

void afficherPlateau() {
	bool caseEstVide[TAILLE_PLATEAU];
	for(int i = 0; i < TAILLE_PLATEAU; ++i) caseEstVide[i] = true;
	for(int i = 0; i < NB_COULEUR; ++i) // On dit que la case à la position du chameau n'est pas vide
		caseEstVide[chameaux[i].position%16] = false;
	for(int i = 0; i < nbJoueurs; ++i) // Si la tuile est sur une case, alors cette case n'est pas vide
		if(parieurs[i].tuileDesert.position != -1) caseEstVide[parieurs[i].tuileDesert.position] = false;

	for(int i = 0; i < TAILLE_PLATEAU; ++i) {
		if(caseEstVide[i])
			for(int j = 0; j < NB_COULEUR; ++j)
				sprintf(casesPlateau[i].lignes[j], "%s", ligneVide);
		else genererCase(i);
	}

	printf(formatPlateau,
		   // 5 cases en haut du plateau (11 à 15)
		   casesPlateau[10].lignes[0], casesPlateau[11].lignes[0], casesPlateau[12].lignes[0], casesPlateau[13].lignes[0], casesPlateau[14].lignes[0],
		   casesPlateau[10].lignes[1], casesPlateau[11].lignes[1], casesPlateau[12].lignes[1], casesPlateau[13].lignes[1], casesPlateau[14].lignes[1],
		   casesPlateau[10].lignes[2], casesPlateau[11].lignes[2], casesPlateau[12].lignes[2], casesPlateau[13].lignes[2], casesPlateau[14].lignes[2],
		   casesPlateau[10].lignes[3], casesPlateau[11].lignes[3], casesPlateau[12].lignes[3], casesPlateau[13].lignes[3], casesPlateau[14].lignes[3],
		   casesPlateau[10].lignes[4], casesPlateau[11].lignes[4], casesPlateau[12].lignes[4], casesPlateau[13].lignes[4], casesPlateau[14].lignes[4],

		   // Cases 10 et 16
		   casesPlateau[9].lignes[0], casesPlateau[15].lignes[0],
		   casesPlateau[9].lignes[1], casesPlateau[15].lignes[1],
		   casesPlateau[9].lignes[2], casesPlateau[15].lignes[2],
		   casesPlateau[9].lignes[3], casesPlateau[15].lignes[3],
		   casesPlateau[9].lignes[4], casesPlateau[15].lignes[4],

		   // Cases 9 et 1
		   casesPlateau[8].lignes[0], casesPlateau[0].lignes[0],
		   casesPlateau[8].lignes[1], casesPlateau[0].lignes[1],
		   casesPlateau[8].lignes[2], casesPlateau[0].lignes[2],
		   casesPlateau[8].lignes[3], casesPlateau[0].lignes[3],
		   casesPlateau[8].lignes[4], casesPlateau[0].lignes[4],

		   // Cases 8 et 2
		   casesPlateau[7].lignes[0], casesPlateau[1].lignes[0],
		   casesPlateau[7].lignes[1], casesPlateau[1].lignes[1],
		   casesPlateau[7].lignes[2], casesPlateau[1].lignes[2],
		   casesPlateau[7].lignes[3], casesPlateau[1].lignes[3],
		   casesPlateau[7].lignes[4], casesPlateau[1].lignes[4],

		   // 5 Cases du bas (3 à 7)
		   casesPlateau[6].lignes[0], casesPlateau[5].lignes[0], casesPlateau[4].lignes[0], casesPlateau[3].lignes[0], casesPlateau[2].lignes[0],
		   casesPlateau[6].lignes[1], casesPlateau[5].lignes[1], casesPlateau[4].lignes[1], casesPlateau[3].lignes[1], casesPlateau[2].lignes[1],
		   casesPlateau[6].lignes[2], casesPlateau[5].lignes[2], casesPlateau[4].lignes[2], casesPlateau[3].lignes[2], casesPlateau[2].lignes[2],
		   casesPlateau[6].lignes[3], casesPlateau[5].lignes[3], casesPlateau[4].lignes[3], casesPlateau[3].lignes[3], casesPlateau[2].lignes[3],
		   casesPlateau[6].lignes[4], casesPlateau[5].lignes[4], casesPlateau[4].lignes[4], casesPlateau[3].lignes[4], casesPlateau[2].lignes[4]
		   );
}