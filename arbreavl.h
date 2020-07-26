/* UQAM / Département d'informatique
   INF3105 - Structures de données et algorithmes
   Squelette pour classe générique ArbreAVL<T> pour le Lab6 et le TP2.

   AUTEUR(S):
    1) Vincent Bray - BRAV20069009
*/

#if !defined(__ARBREAVL_H__)
#define __ARBREAVL_H__
#include <cassert>
#include "pile.h"
#include <algorithm>

/*
 * Classe representant un Arbre AVL.
 */
template <class T>
class ArbreAVL {
  public:
    ArbreAVL();
    ArbreAVL(const ArbreAVL&);
    ~ArbreAVL();

    /*
    * Classe representant l'iterateur de l'ArbreAVL.
    */
    class Iterateur;

    void inserer(const T&);
    bool contient(const T&) const;
    bool vide() const;
    void vider();
    void enlever(const T&);

    Iterateur debut() const;
    Iterateur fin() const;
    Iterateur rechercher(const T&) const;
    Iterateur rechercherEgalOuSuivant(const T&) const;
    Iterateur rechercherEgalOuPrecedent(const T&) const;

    const T& operator[](const Iterateur&) const;
    T& operator[](const Iterateur&);

    ArbreAVL& operator = (const ArbreAVL&);

  private:
    /*
    * Struct representant un noeud de l'ArbreAVL.
    */
    struct Noeud{
        Noeud(const T&);
        T contenu;
        int equilibre;
        Noeud *gauche,
              *droite;
    };
    Noeud* racine;

    bool inserer(Noeud*&, const T&);
    void rotationGaucheDroite(Noeud*&);
    void rotationDroiteGauche(Noeud*&);
    void vider(Noeud*&);
    void copier(const Noeud*, Noeud*&) const;
    bool enlever(Noeud*&, const T& e);
    const T *trouver(const T &element, const Noeud *n) const;

  public:
    class Iterateur{
      public:
        Iterateur(const ArbreAVL& a);
        Iterateur(const Iterateur& a);
        Iterateur(const ArbreAVL& a, Noeud* c);

        operator bool() const;
        bool operator!() const;
        bool operator==(const Iterateur&) const;
        bool operator!=(const Iterateur&) const;
        
        const T& operator*() const;

        Iterateur& operator++();
        Iterateur operator++(int);
        Iterateur& operator = (const Iterateur&);
      private:
        const ArbreAVL& arbre_associe;
        Noeud* courant;
        Pile<Noeud*> chemin;
        
      friend class ArbreAVL;
    };
};



/*
 * Construit un noeud.
 *
 * c: Contenu du noeud.
 */
template <class T>
ArbreAVL<T>::Noeud::Noeud(const T& c)
 : contenu(c), equilibre(0), gauche(nullptr), droite(nullptr)
{
}

/*
 * Construit un ArbreAVL.
 */
template <class T>
ArbreAVL<T>::ArbreAVL() 
 : racine(nullptr)
{
}

/*
 * Construit une copie d'un ArbreAVL.
 *
 * autre: L'arbreAVL a copier.
 */
template <class T>
ArbreAVL<T>::ArbreAVL(const ArbreAVL<T>& autre)
 : racine(nullptr)
{
    copier(autre.racine, racine);
}

/*
 * Detruit un ArbreAVL.
 */
template <class T>
ArbreAVL<T>::~ArbreAVL()
{
    vider(racine);
}

/*
 * Retourne true si l'arbre contient l'element, sinon retourne false.
 *
 * element: L'element qui est possiblement contenu.
 */
template <class T>
bool ArbreAVL<T>::contient(const T& element) const
{
    return (trouver(element, racine) != nullptr);
}

/*
 * Cherche un element a travers un sous-arbre.
 *
 * element: L'element a trouver.
 * n: Premier noeud du sous-arbre.
 */
template <class T>
const T* ArbreAVL<T>::trouver(const T& element, const Noeud* n) const {
    if (n == nullptr) return nullptr;
    if (n->contenu < element) return trouver(element, n->droite);
    if (n->contenu > element) return trouver(element, n->gauche);
    return &(n->contenu);
}

/*
 * Insere un element dans l'arbre.
 *
 * element: L'element a inserer.
 */
template <class T>
void ArbreAVL<T>::inserer(const T& element)
{
    inserer(racine, element);
}

/*
 * Insere un noeud dans l'arbre.
 *
 * noeud: Le premier noeud de l'arbre.
 * element: L'element que le noeud contiendra.
 */
template <class T>
bool ArbreAVL<T>::inserer(Noeud*& noeud, const T& element)
{
    if(noeud==NULL)
    {
        noeud = new Noeud(element);
        return true;
    }
    if(element < noeud->contenu){
        if(inserer(noeud->gauche, element))
        {
            noeud->equilibre++;
            if(noeud->equilibre == 0)
                return false;
            if(noeud->equilibre == 1)
                return true;
            assert(noeud->equilibre==2);
            if(noeud->gauche->equilibre == -1) {
                rotationDroiteGauche(noeud->gauche);
            }
            rotationGaucheDroite(noeud);
        }
        return false;
    }
    else if(noeud->contenu < element) {
        if (inserer(noeud->droite, element)) {
            noeud->equilibre--;
            if (noeud->equilibre == 0) return false;
            if (noeud->equilibre == -1) return true;
            assert(noeud->equilibre == -2);
            if (noeud->droite->equilibre == 1) {
                rotationGaucheDroite(noeud->droite);
            }
            rotationDroiteGauche(noeud);
        }
        return false;
    } else {
        noeud->contenu = element;
        return false;
    }
}

/*
 * Fait une rotation vers la droite sur un noeud.
 *
 * racinesousarbre: La racine du sous-abre sur lequel est faite la rotation.
 */
template <class T>
void ArbreAVL<T>::rotationGaucheDroite(Noeud*& racinesousarbre)
{
    Noeud *temp = racinesousarbre->gauche;

    int  ea = temp->equilibre;
    int  eb = racinesousarbre->equilibre;
    int  neb = - (ea > 0 ? ea : 0) - 1 + eb;
    int  nea = ea + (neb < 0 ? neb : 0) - 1;
    temp->equilibre = nea;
    racinesousarbre->equilibre = neb;

    racinesousarbre->gauche = temp->droite;
    temp->droite = racinesousarbre;
    racinesousarbre = temp;
}

/*
 *Fait une rotation vers la gauche sur un noeud.
 *
 * racinesousarbre: La racine du sous-abre sur lequal est faite la rotation.
 */
template <class T>
void ArbreAVL<T>::rotationDroiteGauche(Noeud*& racinesousarbre)
{
    Noeud *temp = racinesousarbre->droite;

    int  ea = temp->equilibre;
    int  eb = racinesousarbre->equilibre;
    int  neb = - (ea > 0 ? ea : 0) + 1 + eb;
    int  nea = ea + (neb > 0 ? neb : 0) + 1;
    temp->equilibre = nea;
    racinesousarbre->equilibre = neb;

    racinesousarbre->droite = temp->gauche;
    temp->gauche = racinesousarbre;
    racinesousarbre = temp;
}

/*
 * Retourne true si l'arbre est vide, sinon retourne false.
 */
template <class T>
bool ArbreAVL<T>::vide() const
{
    return (racine == nullptr);
}

/*
 * Vide le contenu de l'arbre.
 */
template <class T>
void ArbreAVL<T>::vider(){
  vider(racine);
  racine = nullptr;
}

/*
 * Vide le contenu d'un noeud et detruit le noeud.
 *
 * noeud: Le noeud sur lequel l'operation est faire.
 */
template <class T>
void ArbreAVL<T>::vider(Noeud*& noeud)
{
    if (noeud != nullptr) {
        vider(noeud->gauche);
        vider(noeud->droite);

        delete noeud;
        noeud = nullptr;
    }
}

/*
 * Fait une copie d'un sous-arbre.
 *
 * source: Racine du sous-arbre a copier.
 * noeud: Racine du sous-arbre qui recoit la copie.
 */
template <class T>
void ArbreAVL<T>::copier(const Noeud* source, Noeud*& noeud) const {
    if(noeud==NULL)
    {
        noeud = new Noeud(source->contenu);
    }
    noeud->contenu = source->contenu;
    noeud->equilibre = source->equilibre;
    if (source->gauche != nullptr) {
        copier(source->gauche, noeud->gauche);
    }
    if (source->droite != nullptr) {
        copier(source->droite, noeud->droite);
    }


}

/*
 * Pas implementer.
 * Enleve un noeud de l'arbre.
 *
 * element: L'element qui doit etre enleve.
 */
template <class T>
void ArbreAVL<T>::enlever(const T& element) {
    enlever(racine, element);
}

/*
 * Pas implementer.
 * Enleve un noeud d'un sous-arbre.
 *
 * element: L'element qui doit etre enleve.
 * noeud: La racine du sous-arbre.
 *
 */
template <class T>
bool ArbreAVL<T>::enlever(Noeud*& noeud, const T& element) {
    if(element < noeud->contenu)
    {
        if(enlever(noeud->gauche, element))
        {
            // ...
        }
    }
    else if(element > noeud->contenu)
    {
        // ...
    }
    else if(element == noeud->contenu)
    {
        if (noeud->gauche==NULL && noeud->droite==NULL)
        {
            delete noeud;
            noeud = NULL;
            return true;
        }
        else
        {
            // ...
            return true;
        }
    }
    
}

/*
 * Retourne un iterateur au debut de l'arbre.
 */
template <class T>
typename ArbreAVL<T>::Iterateur ArbreAVL<T>::debut() const {
    Iterateur iter(*this);
    iter.courant = racine;
    if (iter.courant != nullptr) {
        while (iter.courant->gauche != nullptr) {
            iter.chemin.empiler(iter.courant);
            iter.courant = iter.courant->gauche;
        }
    }
    return iter;
}

/*
 * Retourne un iterateur a la fin de l'arbre.
 */
template <class T>
typename ArbreAVL<T>::Iterateur ArbreAVL<T>::fin() const {
    return Iterateur(*this);
}

/*
 * Retourne un iterateur sur le noeud de la recherche.
 *
 * e: Valeur de la recherche.
 */
template <class T>
typename ArbreAVL<T>::Iterateur ArbreAVL<T>::rechercher(const T& e) const {
    Iterateur iter(*this);
    Noeud* n = racine;

    while (n) {
        if (e < n->contenu) {
            iter.chemin.empiler(n);
            n = n->gauche;
        } else if (n->contenu < e) {
            n = n->droite;
        } else {
            iter.courant = n;
            return iter;
        }
    }

    iter.chemin.vider();
    return iter;
}

/*
 * Retourne un iterateur sur le noeud de la recherche, si la valeur est absente retourne l'iterateur sur le noeud suivant.
 *
 * e: Valeur de la recherche.
 */
template <class T>
typename ArbreAVL<T>::Iterateur ArbreAVL<T>::rechercherEgalOuSuivant(const T& e) const {
    Noeud* n = racine;
    Noeud* d = nullptr;

    while(n) {
        if (e < n->contenu) {
            d = n;
            n = n->gauche;
        } else if (n->contenu < e) {
            n = n->droite;
        } else {
            return rechercher(e);
        }

    }

    if (d != nullptr) {
        return rechercher(d->contenu);
    }
    return Iterateur(*this);
}

/*
 * Retourne un iterateur sur le noeud de la recherche, si la valeur est absente retourne l'iterateur sur le noeud precedent.
 *
 * e: Valeur de la recherche.
 */
template <class T>
typename ArbreAVL<T>::Iterateur ArbreAVL<T>::rechercherEgalOuPrecedent(const T& e) const {
    Noeud* n = racine;
    Noeud* d = nullptr;

    while(n) {
        if (e < n->contenu) {
            n = n->gauche;
        } else if (n->contenu < e) {
            d = n;
            n = n->droite;
        } else {
            return rechercher(e);
        }

    }

    if (d != NULL) {
        return rechercher(d->contenu);
    }
    return Iterateur(*this);
}

/*
 * Retourne une valeur.
 *
 * iterateur: L'iterateur de l'arbre.
 */
template <class T>
const T& ArbreAVL<T>::operator[](const Iterateur& iterateur) const {
    assert(&iterateur.arbre_associe == this);
    assert(iterateur.courant);
    return iterateur.courant->contenu;
}

/*
 * Assigne une valeur.
 *
 * iterateur: L'iterateur de l'arbre.
 */
template <class T>
T& ArbreAVL<T>::operator[](const Iterateur& iterateur) {
    assert(&iterateur.arbre_associe == this);
    assert(iterateur.courant);
    return iterateur.courant->contenu;
}

/*
 * Fait une copie d'un ArbreAVL.
 *
 * autre: L'ArbreAVL a copier.
 */
template <class T>
ArbreAVL<T>& ArbreAVL<T>::operator=(const ArbreAVL& autre) {
    if(this==&autre) return *this;
    vider();
    copier(autre.racine, racine);
    return *this;
}

/*
 * Construit un iterateur.
 *
 * a: L'Arbre associe.
 */
template <class T>
ArbreAVL<T>::Iterateur::Iterateur(const ArbreAVL& a)
 : arbre_associe(a), courant(nullptr) {
}

/*
 * Construit un iterateur.
 *
 * a: L'iterateur de l'arbre associe.
 */
template <class T>
ArbreAVL<T>::Iterateur::Iterateur(const ArbreAVL<T>::Iterateur& a)
: arbre_associe(a.arbre_associe) {
    courant = a.courant;
    chemin = a.chemin;
}

/*
 * Fait le pre increment de l'iterateur.
 */
template <class T>
typename ArbreAVL<T>::Iterateur& ArbreAVL<T>::Iterateur::operator++() {
    assert(courant);
    Noeud* suivant = courant->droite;
    while (suivant) {
        chemin.empiler(suivant);
        suivant = suivant->gauche;
    }
    if (!chemin.vide()) {
        courant = chemin.depiler();
    } else {
        courant = nullptr;
    }
    return *this;
}

/*
 * Fait le post increment de l'iterateur.
 */
template <class T>
typename ArbreAVL<T>::Iterateur ArbreAVL<T>::Iterateur::operator++(int) {
    Iterateur copie(*this);
    operator++();
    return copie;
}

/*
 * Retourne la valeur true/false de l'iterateur.
 */
template <class T>
ArbreAVL<T>::Iterateur::operator bool() const {
    return courant!= nullptr;
}

/*
 * Retourne l'inverse la valeur true/false de l'iterateur.
 */
template <class T>
bool ArbreAVL<T>::Iterateur::operator!() const {
    return courant== nullptr;
}

/*
 * Retourne true si le noeud iterer est egal au noeud itere de o.
 *
 * o: Iterateur utlise dans la comparaison.
 */
template <class T>
bool ArbreAVL<T>::Iterateur::operator==(const Iterateur& o) const {
    assert(&arbre_associe==&o.arbre_associe);
    return courant==o.courant;
}

/*
 * Retourne true si l'iterateur n'egale pas a l'iterateur o:
 *
 * o: Iterateur a comparer.
 */
template <class T>
bool ArbreAVL<T>::Iterateur::operator!=(const Iterateur& o) const {
    assert(&arbre_associe==&o.arbre_associe);
    return courant!=o.courant;
}

/*
 * Retourne le contenu du noeud itere.
 */
template <class T>
const T& ArbreAVL<T>::Iterateur::operator *() const {
    assert(courant!= nullptr);
    return courant->contenu;
}

/*
 * Copie le le noeud courant et le chemin d'un iterateur.
 *
 * autre: L'iterateur a copier.
 */
template <class T>
typename ArbreAVL<T>::Iterateur& ArbreAVL<T>::Iterateur::operator = (const Iterateur& autre){
    assert(&arbre_associe==&autre.arbre_associe);
    courant = autre.courant;
    chemin = autre.chemin;
    return *this;
}

#endif
