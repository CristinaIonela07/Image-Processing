# Image Processing

Structura de la arbore este asemanatoare cu cea de la vector. Am adaugat 
o structura aditionala pentru a usura lucrul cu pixeli.

Functiile "medium_collor" si "mean" le-am ales de tip long long int pentru 
a evita overflow-ul cauzat de dimensiunea mare a imaginilor. Parametrul "c"
din prima functie indica pentru ce culoare se doreste aflarea valorii mediii:
1 - red, 2 - green, 3 - blue. Se parcurge matricea in blocul despre care dorim
sa aflam informatii si se calculeaza suma canalului respectiv(R, G sau B), 
fiind apoi impartita la dimensiunea totala a blocului.

In functia "mean" este ultimul pas inainte de a se decide daca blocul va fi 
divizat sau nu. Se calculeaza scorul similaritatii conform formulei.

In functia "matrix_tree" se construieste arborele pe baza informatiilor din
matrice. Daca scorul similaritatii este mai mare decat un prag impus, atunci
arborele va avea 4 fii, fiecaruia fiindu-i asociati pixelii corespunzatori, 
si se continua divizatia pana ce scorul devine mai mic. Culorile ajung astfel 
sa fie continute de nodurile terminale din arbore.
