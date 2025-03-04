# Messagerie Crypto

## Introduction
Bienvenue dans le projet **Messagerie Crypto**. Ce projet permet de créer une application de messagerie sécurisée basée sur la cryptographie.  
Ce guide explique comment compiler et exécuter les différents composants du projet.

---

## Installation et Compilation

### 1. Compiler le backend
Pour compiler le backend du projet (exécutable `backend.exe`), utilisez les commandes suivantes :

cmake -S . -B build/
cmake --build build

Cette commande génère le fichier exécutable dans le dossier build/Debug.
Installation des Dépendances Frontend

### 2. Installer les dépendances

Dans le dossier du frontend, exécutez la commande suivante pour installer toutes les dépendances :

npm install


### 3. Lancer l'application

Pour démarrer l'application frontend, utilisez la commande suivante :

npm start

Si vous êtes sur Windows et souhaitez spécifier un port particulier (par exemple, le port 3001), utilisez :

set PORT=3001 && npm start

Cela démarre l'application frontend sur le port 3001. Modifiez le port selon vos préférences.
