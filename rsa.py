import random
import math

def is_probable_prime(n, k=20):
    """ Test de primalité de Miller-Rabin avec k itérations.
    Retourne True si n est probablement premier, sinon False.
    """
    if n < 2:
        return False
    if n in (2, 3):
        return True
    if n % 2 == 0:
        return False
    
    # Décomposer n-1 comme d * 2^r
    r, d = 0, n - 1
    while d % 2 == 0:
        d //= 2
        r += 1
    
    # Test de Miller-Rabin
    for _ in range(k):
        a = random.randint(2, n - 2)
        x = pow(a, d, n)
        if x in (1, n - 1):
            continue
        for _ in range(r - 1):
            x = pow(x, 2, n)
            if x == n - 1:
                break
        else:
            return False
    return True

def generate_large_prime(bits):
    """ Génère un nombre premier d'une certaine taille en bits. """
    while True:
        # Génère un nombre aléatoire de taille spécifiée en bits
        n = random.getrandbits(bits)
        # S'assure que le nombre est impair en fixant le dernier bit à 1
        n |= (1 << bits - 1) | 1
        # Vérifie si le nombre est probablement premier
        if is_probable_prime(n):
            return n

# Générer un nombre premier de 1024 bits
# prime_1024 = generate_large_prime(1024)
# print(f"Nombre premier de 1024 bits : {prime_1024}")

def generateE(phiN):
    e=3
    while(True):
        if(math.gcd(e,phiN) == 1):
            return e
        else:
            e+=2 #garantit des valeurs impaires, ce qui est voulu pour rsa

def extended_gcd(a, b):
    """ 
    Implémentation de l'algorithme d'Euclide étendu.
    Renvoie le PGCD de a et b, ainsi que les coefficients x et y 
    tels que ax + by = gcd(a, b).
    """
    if a == 0:
        return b, 0, 1
    gcd, x1, y1 = extended_gcd(b % a, a)
    x = y1 - (b // a) * x1
    y = x1
    return gcd, x, y

def mod_inverse(e, phi):
    """ 
    Trouve l'inverse modulaire de e modulo phi.
    Renvoie l'inverse d, tel que d * e ≡ 1 (mod phi).
    """
    gcd, x, _ = extended_gcd(e, phi)
    if gcd != 1:
        raise ValueError("L'inverse modulaire n'existe pas, e et phi ne sont pas premiers entre eux.")
    else:
        # Assurez-vous que l'inverse est positif
        return x % phi

def generate_rsa_keys(bits):
    """ Génère une paire de clés RSA de taille bits. """
    # Générer deux grands nombres premiers p et q
    p = generate_large_prime(bits // 2)
    q = generate_large_prime(bits // 2)
    n = p * q  # Calculer n
    phiN = (p - 1) * (q - 1)  # Calculer phi(n)
    e = generateE(phiN)  # Trouver e
    d = mod_inverse(e, phiN)  # Trouver d
    return (n, e), d  # Retourner la clé publique et la clé privée

# Générer une paire de clés RSA de 1024 bits
public_key, private_key = generate_rsa_keys(1024)
print(f"Clé publique (n, e): {public_key}")
print(f"Clé privée (d): {private_key}")
(n1,e1) = public_key

# Générer une paire de clés RSA de 1024 bits
public_key2, private_key2 = generate_rsa_keys(1024)
print(f"Clé publique (n, e): {public_key2}")
print(f"Clé privée (d): {private_key2}")
(n2,e2) = public_key2

message = input("Enter your message to Encrypt ")

message_encoded = [ord(ch) for ch in message]

print ("Message in ASCII code: ", message_encoded)

# (m ^ e) mod n = c 
ciphertext = [pow(ch, e1, n1) for ch in message_encoded]
print(f"{message} chiffré en: {ciphertext}")

# Déchiffrement du message
Decodemsg = [pow(ch, private_key, n1) for ch in ciphertext] 
print("Retour en ASCII: ", Decodemsg)

# Conversion de l'ASCII en texte
msg = "".join(chr(ch) for ch in Decodemsg)
print("Du ASCII au TEXTE: ", msg)





