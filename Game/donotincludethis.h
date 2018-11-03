#include <stdio.h>

extern "C" {
	struct fraction {
		int numerateur;
		int denominateur;
	};

	typedef struct fraction Fraction;

	Fraction add(Fraction f1, Fraction f2) {
		Fraction resultat;
		if (f1.denominateur != f2.denominateur) {
			//on gère si les dénominateur ne sont pas égaux
		}
		else { //Allez je suis gentil je te fait celui la ^^
			resultat.denominateur = f1.denominateur;
			resultat.numerateur = f1.numerateur + f2.numerateur;
		}
		return resultat;
	}

	int main() {
		int entree;
		int entree1;
		scanf("%d", &entree);
		scanf("%d", &entree1);
		Fraction f1;
		Fraction f2;

		f1.numerateur = entree;
		f1.denominateur = entree1;

		scanf("%d", &entree);
		scanf("%d", &entree1);

		f2.numerateur = entree;
		f2.denominateur = entree1;

		Fraction resultat = add(f1, f2);

		//affichage du resultat

		return 0;

	}
}