string CleanCommande(string chaine) {
    // Premi�re quote
    if (chaine.find_first_of('"') == 0)
        chaine.erase(0, 1);

    // Espace � la fin
    chaine.erase(chaine.find_last_not_of(' ') + 1);

    // Derni�re quote
    size_t taille = chaine.size();
    if (chaine.rfind('"') == taille-1)
        chaine.pop_back();

    return chaine;
}

void DecompCommande(string commande, /* Sortie*/string &chemin, string &args) {
    // D�limiteur
    string delimiteur = ".exe";
	int tDelimit = delimiteur.size();

    // D�composition
    int posExe  = commande.find(delimiteur) + tDelimit;
    chemin      = trim(commande.substr(0, posExe));
    args        = (commande.size() > posExe) ? commande.substr(posExe+1) : "";
}

string NomFichier(string chemin) {
    string nomOrig;

    int posSlash = chemin.find_last_of('\\');
    if (posSlash != -1)
        nomOrig = chemin.substr(posSlash + 1);

    return nomOrig;
}
