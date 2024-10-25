# messagerie-crypto
Explication RSA :
1ère étape : choisir deux nombres premiers de plus de 1024 bits p et q.
2ème étape : les multiplier pour trouver n (n sera la valeur max du message m)
3ème étape : trouver phi(n) = (p-1)*(q-1)
4ème étape : trouver e tel que 1<e<phi(n) et gcd(e,phi(n)) = 1. e correspond à la clé publique (e impaire)
5ème étape : trouver d, la clé privée en utilisant l’algorithme d’Euclide étendu permettant de retrouver d tel que e*d = 1 mod(phi(n))
