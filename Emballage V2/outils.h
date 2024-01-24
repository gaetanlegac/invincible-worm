vector<string> Explode(string Chaine, string Delimiteur) {
	vector<string> Resultats;
    Chaine += Delimiteur; // Corrige un bug: Dernier élement oublié

    size_t position = 0;
    string element;
    while ((position = Chaine.find(Delimiteur)) != string::npos) {
        element = Chaine.substr(0, position);
        Chaine.erase(0, position + Delimiteur.length());
        Resultats.push_back(element);
    }

	return Resultats;
}

static string trim(string str) {
	size_t startpos = str.find_first_not_of(" \t");
    if( string::npos != startpos )
    {
        str = str.substr( startpos );
    }

	return str;
}

static string getTemp() {
    TCHAR Buff[MAX_PATH];
    GetTempPath(MAX_PATH, Buff);

    return Buff;
}

static string getExeActuel() {
    HMODULE hModule = GetModuleHandleW(NULL);
    WCHAR Chemin[MAX_PATH];
    GetModuleFileNameW(hModule, Chemin, MAX_PATH);
    wstring retourW = Chemin;

    string Retour(retourW.begin(), retourW.end());

    return Retour;
}

string RandomStr(size_t length)
{
	const char* charmap = "abcdefghijklmnopqrstwxyz0123456789";
	const size_t charmapLength = strlen(charmap);
	auto generator = [&](){ return charmap[rand() % charmapLength]; };
	string result;
	result.reserve(length);
	generate_n(back_inserter(result), length, generator);
	return result;
}

// DEBUG
#ifdef Mode_Dev
string intToString(int i) {
     std::ostringstream oss;
     oss << i;
     return oss.str();
}

void Debug(string txt) {
    MessageBox(NULL, txt.c_str(), "Debug", MB_OK);
}

void Debug(int nb) {
    Debug(intToString(nb));
}

void affErreur() {
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID != 0) {
		// Recup du message
		LPSTR messageBuffer = nullptr;
		size_t size = FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, errorMessageID,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPSTR)&messageBuffer, 0, NULL
			);

		// Formatage en chaine
		string message(messageBuffer, size);

		// Nettoyage
		LocalFree(messageBuffer);

		Debug(message);
	}
}
#endif