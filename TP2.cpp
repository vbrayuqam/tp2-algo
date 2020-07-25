

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
        }
    }

    for( Histoire * histoire : * contenu) {
        ArbreMap<string, double> temp;
        for( Phrase p : * histoire ) {
            for (auto pIt = p.begin(); pIt != p.end(); pIt++) {
                temp[*pIt] = (temp[*pIt] + 1);
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

void faireCalculIdf(vector< Histoire * > * &hist, ArbreMap<string, double> &contenu, ArbreMap<string, double> &contenant) {
    int index = 0;
    for ( Histoire * histoire : * hist) {
        index++;
    }

    for (auto iter = contenu.debut(); iter != contenu.fin(); iter++) {
        contenant[iter.cle()] = log2(index / iter.valeur());
    }
}

void faireCalculsDeuxiemePhase(vector< Histoire * > * &hist, vector<ArbreMap<string, double>> &arbTf, ArbreMap<string, double> &arbIdf, ArbreMap<string, double> &res, vector<string> &requete) {
    int index= 0;
    for ( Histoire * histoire : * hist) {
        double somme = 0;

        for (int i = 0; i < requete.size(); i++) {
            double valTf = 0;
            for(auto iter = arbTf.at(index).debut(); iter != arbTf.at(index).fin(); iter++) {
                if (requete.at(i) == iter.cle()) {
                    valTf = arbTf.at(index)[requete.at(i)];
                }
            }

            double valIdf = 0;
            for(auto iter = arbIdf.debut(); iter != arbIdf.fin(); iter++) {
                if (requete.at(i) == iter.cle()) {
                    valIdf = arbIdf[requete.at(i)];
                }
            }

            somme = somme + (valTf * valIdf);
        }

        res[histoire->titre()] = somme;
        index++;
    }
}

void afficherResultats(ArbreMap<string, double> &res) {
    vector<string> titres;
    vector<double> valeurs;

    for (auto iter = res.debut(); iter != res.fin(); iter++) {
        titres.push_back(iter.cle());
        valeurs.push_back(iter.valeur());
    }

    for (int i = 0; i < 5; i++) {
        int maxIndex = 0;;
        for (int j = 1; j < valeurs.size() ; j++) {
            if (valeurs.at(j) > valeurs.at(maxIndex)) {
                maxIndex = j;
            }
        }

        cout << valeurs.at(maxIndex) << " : " << titres.at(maxIndex) << endl;

        titres.erase(titres.begin() + maxIndex);
        valeurs.erase(valeurs.begin() + maxIndex);
    }
}

int main() {
    bool continuer = true;
    string requeteInit;
    vector< string > motsRequete;
    vector<ArbreMap<string, double>> tfArbres;
    ArbreMap<string, double> idfArbreTransition;
    ArbreMap<string, double> idfArbre;
    ArbreMap<string, double> resultats;

    vector< Histoire * > * histoires = lireDocuments( string( "listeDocument.xml" ) );

    faireCalculsTf(histoires, tfArbres);
    trouverDf(histoires, idfArbreTransition);
    faireCalculIdf(histoires, idfArbreTransition, idfArbre);

    do {
        motsRequete.clear();
        requeteInit = saisirRequete();
        definirMotsRequete(motsRequete, requeteInit);

        if (motsRequete.empty()) {
            continuer = false;
        }
        if (continuer) {
            faireCalculsDeuxiemePhase(histoires, tfArbres, idfArbre, resultats, motsRequete);
            afficherResultats(resultats);
        }
    } while (continuer);

    afficherMsgFin();
    return 0;
}

