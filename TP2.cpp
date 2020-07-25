

#include "DocumentXML.hpp"
#include "ElementXML.hpp"
#include "Histoire.hpp"
#include "Lecteur.hpp"
#include "arbremap.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <regex>


using namespace std;



vector< Histoire * > *
lireDocuments( string a_nomFichier )
{
    vector< Histoire * > * histoires = new vector< Histoire * >();
    DocumentXML * listeFichiers = lireFichierXML( a_nomFichier );

    assert( nullptr != listeFichiers );
    ElementXML * courant = listeFichiers->racine();
    assert( nullptr != courant );

    courant = courant->prochainElement();

    assert( nullptr != courant );
    for( NoeudXML * contenu : * courant )
    {
        assert( nullptr != contenu );
        if( contenu->estElement() )
        {
            ElementXML * element = ( ElementXML * )contenu;

            assert( nullptr != element );
            DocumentXML * doc = lireFichierXML( element->attribut( string( "fichier" ) ) );

            assert( nullptr != doc );
            vector< Histoire * > * h = extraireHistoires( * doc );

            assert( nullptr != h );
            assert( nullptr != histoires );
            histoires->insert( histoires->end(), h->begin(), h->end() );
        }
    }

    return histoires;
}

string saisirRequete() {
    string inputReq;
    cout << "\nVeuillez entrer votre requete et appuyer sur la touche ENTER, une requete vide terminera le programme:\n" << endl;
    getline(cin, inputReq);
    return inputReq;
}

void definirMotsRequete(vector<string> &contenant, string &contenu) {
    regex delim("[^\\s.,:;!?\\=\\\\\\/\\_+@#$%&*()\\[\\]\\{\\}éè\\^âîôñç\"'1234567890]+");
    auto deb = sregex_iterator(contenu.begin(), contenu.end(), delim);
    auto fin = sregex_iterator();

    for (sregex_iterator mots = deb; mots != fin; ++mots) {
        contenant.push_back((*mots).str());
    }
}

void afficherMsgFin() {
    cout << "\nMerci de votre utilisation, a la prochaine.\n" << endl;
}

void faireCalculsTf(vector< Histoire * > * &contenu,  vector<ArbreMap<string, double>> &contenant) {
    for( Histoire * histoire : * contenu) {
        ArbreMap<string, double> temp;
        for( Phrase p : * histoire ) {
            for (auto pIt = p.begin(); pIt != p.end(); pIt++) {
                temp[*pIt] = 0;
            }

            for (auto pIt2 = p.begin(); pIt2 != p.end(); pIt2++) {
                temp[*pIt2] = temp[*pIt2] + 1;
            }
        }
        contenant.push_back(temp);
    }
}

void trouverDf(vector< Histoire * > * &contenu, ArbreMap<string, double> &contenant) {
    ArbreMap<string, double> temp;
    for ( Histoire * histoire : * contenu) {
        for( Phrase p : * histoire ) {
            for (auto pIt = p.begin(); pIt != p.end(); pIt++) {
                temp[*pIt] = 1;
            }
        }
    }

    for (auto iter = temp.debut(); iter != temp.fin(); iter++) {
        contenant[iter.cle()] = 0;
    }

    for ( Histoire * histoire : * contenu) {
        ArbreMap<string, double> temp2;
        for( Phrase p : * histoire ) {
            for (auto pIt = p.begin(); pIt != p.end(); pIt++) {
                temp2[*pIt] = 1;
            }
        }

        for (auto iter2 = temp2.debut(); iter2 != temp2.fin(); iter2++) {
            contenant[iter2.cle()] = contenant[iter2.cle()] + 1;
        }
    }
}

void faireCalculIdf(vector< Histoire * > * &histoires, ArbreMap<string, double> &contenu, ArbreMap<string, double> &contenant) {
    int index = 0;
    for ( Histoire * histoire : * histoires) {
        index++;
    }

    for (auto iter = contenu.debut(); iter != contenu.fin(); iter++) {
        contenant[iter.cle()] = log2(index / iter.valeur());
    }
}

int main() {
    // Variables
    bool continuer = true;
    string requeteInit;
    vector< string > motsRequete;
    vector<ArbreMap<string, double>> tfArbres;
    ArbreMap<string, double> idfArbreTransition;
    ArbreMap<string, double> idfArbre;

    // Gestion des histoires
    vector< Histoire * > * histoires = lireDocuments( string( "listeDocument.xml" ) );

    // Premier calculs
    faireCalculsTf(histoires, tfArbres);
    trouverDf(histoires, idfArbreTransition);
    faireCalculIdf(histoires, idfArbreTransition, idfArbre);

    // Boucle principale
    do {
        motsRequete.clear();
        requeteInit = saisirRequete();
        definirMotsRequete(motsRequete, requeteInit);

        // Ajustement de la variable pour continuer la boucle
        if (motsRequete.empty()) {
            continuer = false;
        }
        if (continuer) {
            // Deuxieme phase de calculs
            // Affichage des sorties
        }
    } while (continuer);

    // Fin du programme
    afficherMsgFin();
    return 0;
}

