/* TP2 - INF3105
   AUTEUR(S):
    1) Vincent Bray - BRAV20069009
*/

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


/*
 * Lit le fichier pour creer la structure des histoires et les retournes dans un vecteur.
 *
 * a_nomFicher: Nom du fichier a lire.
 */
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

/*
 * Saisit la requete au terminal et la retourne en string.
 */
string saisirRequete() {
    string inputReq;
    cout << "\nVeuillez entrer votre requete et appuyer sur la touche ENTER, une requete vide terminera le programme:\n" << endl;
    getline(cin, inputReq);
    return inputReq;
}

/*
 * Definit les mots de la requete.
 *
 * contenant: Un vecteur de string qui recoit les mots de la requete.
 * contenu: Un string qui contient les mots de la requete avant qu'ils soient separes.
 */
void definirMotsRequete(vector<string> &contenant, string &contenu) {
    regex delim("[^\\s.,:;!?\\=\\\\\\/\\_+@#$%&*()\\[\\]\\{\\}éè\\^âîôñç\"'1234567890]+");
    auto deb = sregex_iterator(contenu.begin(), contenu.end(), delim);
    auto fin = sregex_iterator();

    for (sregex_iterator mots = deb; mots != fin; ++mots) {
        contenant.push_back((*mots).str());
    }
}

/*
 * Affiche le message de fin.
 */
void afficherMsgFin() {
    cout << "\nMerci de votre utilisation, a la prochaine.\n" << endl;
}

/*
 * Effectue le calcul tf de la premiere phase.
 *
 * hist: Un vecteur contenant les histoires.
 * arbTf: Un vecteur contenant des ArbreMap qui contiennent le tf des mots.
 * arbIdf: Un arbreMap contenant l'idf des mots.
 * res: Un ArbreMap qui contiendra les resultats.
 * requete: Un vecteur de string contenant les mots de la requete.
 */
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

/*
 * Trouve le df des mots des histoires.
 *
 * contenu: Un vecteur contenant les hisoires.
 * contenant: Un ArbreMap qui recoit le df des mots.
 */
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

/*
 * Effectue le calcul idf de la premiere phase.
 *
 * hist: Un vecteur contenant les histoires.
 * contenu: Un ArbreMap qui contient le df des mots.
 * contenant: Un ArbreMap qui recoit l'idf des mots.
 */
void faireCalculIdf(vector< Histoire * > * &hist, ArbreMap<string, double> &contenu, ArbreMap<string, double> &contenant) {
    int index = 0;
    for ( Histoire * histoire : * hist) {
        index++;
    }

    for (auto iter = contenu.debut(); iter != contenu.fin(); iter++) {
        contenant[iter.cle()] = log2(index / iter.valeur());
    }
}

/*
 * Effectue la deuxieme phase de calculs.
 *
 * hist: Un vecteur contenant les histoires.
 * arbTf: Un vecteur contenant des ArbreMap qui contiennent le tf des mots.
 * arbIdf: Un arbreMap contenant l'idf des mots.
 * res: Un ArbreMap qui contiendra les resultats.
 * requete: Un vecteur de string contenant les mots de la requete.
 */
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

/*
 * Affiche les resultats de le requete.
 *
 * res: Un ArbreMap contenant les resultats.
 */
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

/*
 * Vide le contenu des arbres pour supprimer les noeuds.
 *
 * vecteurArbre: Un vecteur contenant des arbres a vider.
 * arbre1: Un arbre a vider.
 * arbre2: Un arbre a vider.
 * arbre3: Un arbre a vider.
 */
void nettoyerArbres(vector<ArbreMap<string, double>> vecteurArbre, ArbreMap<string, double> &arbre1, ArbreMap<string, double> &arbre2, ArbreMap<string, double> &arbre3) {
    arbre1.vider();
    arbre2.vider();
    arbre3.vider();
    for (int i = 0; i < vecteurArbre.size(); i++) {
        vecteurArbre.at(i).vider();
    }
}

// Fonction main
int main() {
    // Initialisation des variables
    bool continuer = true;
    string requeteInit;
    vector< string > motsRequete;
    vector<ArbreMap<string, double>> tfArbres;
    ArbreMap<string, double> idfArbreTransition;
    ArbreMap<string, double> idfArbre;
    ArbreMap<string, double> resultats;

    // Creation de la structure des histoires
    vector< Histoire * > * histoires = lireDocuments( string( "listeDocument.xml" ) );

    // Premiere phase de calculs
    faireCalculsTf(histoires, tfArbres);
    trouverDf(histoires, idfArbreTransition);
    faireCalculIdf(histoires, idfArbreTransition, idfArbre);

    // Boucle de requete
    do {
        // Requete
        motsRequete.clear();
        requeteInit = saisirRequete();
        definirMotsRequete(motsRequete, requeteInit);

        if (motsRequete.empty()) {
            continuer = false;
        }
        if (continuer) {
            //Deuxieme phase de calculs
            faireCalculsDeuxiemePhase(histoires, tfArbres, idfArbre, resultats, motsRequete);

            //Affichage des sorties
            afficherResultats(resultats);
        }
    } while (continuer);

    // Fin du programme
    nettoyerArbres(tfArbres, idfArbre, idfArbreTransition, resultats);
    afficherMsgFin();
    return 0;
}

