string CleanCommande(string chaine) {
    // Première quote
    if (chaine.find_first_of('"') == 0)
        chaine.erase(0, 1);

    // Espace à la fin
    chaine.erase(chaine.find_last_not_of(' ') + 1);

    // Dernière quote
    size_t taille = chaine.size();
    if (chaine.rfind('"') == taille-1)
        chaine.pop_back();

    return chaine;
}

void DecompCommande(string commande, /* Sortie*/string &chemin, string &args) {
    // Délimiteur
    string delimiteur = ".exe";
	int tDelimit = delimiteur.size();

    // Décomposition
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
