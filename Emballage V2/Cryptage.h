string XOR(string chaine)
{
    string Retour = chaine;
    char cle[11] = {'5','h','m','8','1','{','H','!','8','*',' '};

    for (int i = 0; i < Retour.size(); i++) {
        Retour[i] = chaine[i] ^ cle[i % (sizeof(cle) / sizeof(char))];
    }
    return Retour;
}

// Lis un fichier en intégralité
string slurp(ifstream& in) {
    stringstream sstr;
    sstr << in.rdbuf();
    return sstr.str();
}

void fXOR(string Source, string Destination) {
    string  donneesS,
            donneesD;

    // Lecture d'original
    ifstream fSource (Source, ios::binary | ios::in);
    donneesS = slurp(fSource);
    fSource.close();

    // Ecriture de la destination
    ofstream fDest (Destination, ios::binary | ios::trunc | ios::out);
    fDest << XOR(donneesS);
    fDest.close();
}
