/* UQAM / Département d'informatique
   INF3105 - Structures de données et algorithmes
   Squelette pour classe générique ArbreMap<K,V> pour le Lab8 et le TP2.

   AUTEUR(S):
    1) Vincent Bray - BRAV20069009
*/

#if !defined(__ARBREMAP_H__)
#define __ARBREMAP_H__

#include "arbreavl.h"

/*
 * Classe representant un Arbre Map.
 */
template <class K, class V>
class ArbreMap
{
  private:
    /*
    * Classe representant une entree cle/valeur dans l'ArbreMap.
    */
    class Entree{
        public:
            Entree(const K& c) : cle(c), valeur() {}
            K cle;
            V valeur;
            bool operator < (const Entree& e) const { return cle < e.cle;}
            bool operator > (const Entree& e) const { return cle > e.cle;}
            bool operator == (const Entree& e) const { return cle == e.cle;}
    };
    ArbreAVL<Entree> entrees;
  public:
    bool contient(const K&) const;
    void enlever(const K&);
    void vider();
    bool vide() const;

    const V& operator[] (const K&) const;
    V& operator[] (const K&);

    /*
    * Classe representant l'iterateur de l'ArbreMap.
    */
    class Iterateur {
        public:
            Iterateur(const ArbreMap& a) : iter(a.entrees.debut()) {}
            Iterateur(typename ArbreAVL<Entree>::Iterateur i) : iter(i) {}
            operator bool() const;
            bool operator!() const;

            Iterateur& operator++();
            Iterateur operator++(int);

            const K& cle() const;
            const V& valeur() const;

        private:
            typename ArbreAVL<Entree>:: Iterateur iter;
    };
    Iterateur debut() const;
    Iterateur fin() const;
    Iterateur rechercher(const K& cle) const;
    Iterateur rechercherEgalOuSuivant(const K& cle) const;
    Iterateur rechercherEgalOuPrecedent(const K& cle) const;
};


/*
 * Vide le contenu de l'ArbreMap.
 */
template <class K, class V>
void ArbreMap<K,V>::vider(){
    entrees.vider();
}

/*
 * Retourne true si l'arbre est vide, sinon retourne false.
 */
template <class K, class V>
bool ArbreMap<K,V>::vide() const{
    return entrees.vide();
}

/*
 * Pas implementer.
 * Enleve une entree de l'arbre.
 *
 * c: La cle associe a la valeur qui doit etre enlever.
 */
template <class K, class V>
void ArbreMap<K,V>::enlever(const K& c)
{
    entrees.enlever(Entree(c));
}

/*
 * Retourne true si l'arbre contient la valeur, sinon retourne false.
 *
 * c: La valeur a verifier.
 */
template <class K, class V>
bool ArbreMap<K,V>::contient(const K& c) const
{
    entrees.contient(Entree(c));
    return false;
}

/*
 * Retourne une valeur.
 *
 * c: La cle pour assigner ou retourner une valeur.
 */
template <class K, class V>
const V& ArbreMap<K,V>::operator[] (const K& c)  const
{
    typename ArbreAVL<Entree>::Iterateur iter=entrees.rechercher(c);
    return entrees[iter].valeur;
}

/*
 * Assigne une valeur.
 *
 * c: La cle pour assigner ou retourner une valeur.
 */
template <class K, class V>
V& ArbreMap<K,V>::operator[] (const K& c) 
{
    typename ArbreAVL<Entree>::Iterateur iter=entrees.rechercher(Entree(c));
    if (!iter) {
        entrees.inserer(Entree(c));
        iter = entrees.rechercher(c);
    }
    return entrees[iter].valeur;
}

/*
 * Retourne un iterateur a partir du debut de l'arbre.
 */
template <class K, class V>
typename ArbreMap<K, V>::Iterateur ArbreMap<K, V>::debut() const {
    return Iterateur(*this);
}

/*
 * Retourne un iterateur representant la fin de l'arbre.
 */
template <class K, class V>
typename ArbreMap<K, V>::Iterateur ArbreMap<K, V>::fin() const {
    typename ArbreAVL<Entree>::Iterateur iter(entrees);
    return Iterateur(iter);
}

/*
 * Recherche une entree dans l'arbre et la retourne.
 *
 * e: L'entree qui doit etre rechercher.
 */
template <class K, class V>
typename ArbreMap<K, V>::Iterateur ArbreMap<K, V>::rechercher(const K &e) const {
    return entrees.rechercherEgalOuSuivant(Entree(e));
}

/*
 * Recherche une entree dans l'arbre et retourne, si elle n'est pas dans l'arbre retourne l'entree suivante.
 *
 * e: L'entree qui doit etre rechercher.
 */
template <class K, class V>
typename ArbreMap<K, V>::Iterateur ArbreMap<K, V>::rechercherEgalOuSuivant(const K &e) const {
    return entrees.rechercherEgalOuSuivant(Entree(e));
}

/*
 * Recherche une entree dans l'arbre et retourne, si elle n'est pas dans l'arbre retourne l'entree precedente.
 *
 * e: L'entree qui doit etre rechercher.
 */
template <class K, class V>
typename ArbreMap<K, V>::Iterateur ArbreMap<K, V>::rechercherEgalOuPrecedent(const K &e) const {
    return entrees.rechercherEgalOuPrecedent(Entree(e));
}


/*
 * Retourne la valeur true/false de l'iterateur.
 */
template <class K, class V>
ArbreMap<K, V>::Iterateur::operator bool() const {
    return iter;
}

/*
 * Retourne l'inverse de la valeur true/false de l'iterateur.
 */
template <class K, class V>
bool ArbreMap<K, V>::Iterateur::operator!() const {
    return !iter;
}

/*
 * Fait le pre increment de l'iterateur.
 */
template <class K, class V>
typename ArbreMap<K, V>::Iterateur& ArbreMap<K, V>::Iterateur::operator++() {
    if (!this) return *this;
    iter++;
    return *this;
}

/*
 * Fait le post increment de l'iterateur.
 */
template <class K, class V>
typename ArbreMap<K, V>::Iterateur ArbreMap<K, V>::Iterateur::operator++(int) {
    Iterateur copie(*this);
    operator++();
    return copie;
}

/*
 * Retourne la cle de l'entree itere.
 */
template <class K, class V>
const K& ArbreMap<K, V>::Iterateur::cle() const {
    return (*iter).cle;
}

/*
 * Retourne la valeur de l'entree itere.
 */
template <class K, class V>
const V& ArbreMap<K, V>::Iterateur::valeur() const {
    return (*iter).valeur;
}

#endif

