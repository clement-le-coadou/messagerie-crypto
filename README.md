<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Messagerie Crypto - README</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            line-height: 1.6;
            background-color: #f4f4f4;
            margin: 0;
            padding: 0;
            color: #333;
        }
        header {
            background-color: #2c3e50;
            color: white;
            padding: 10px;
            text-align: center;
        }
        section {
            margin: 20px;
            padding: 20px;
            background-color: white;
            border-radius: 8px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
        }
        code {
            background-color: #e7e7e7;
            padding: 4px;
            border-radius: 4px;
            font-size: 1.1em;
        }
        pre {
            background-color: #333;
            color: white;
            padding: 10px;
            border-radius: 4px;
            overflow-x: auto;
            white-space: pre-wrap;
            word-wrap: break-word;
        }
        h1, h2 {
            color: #2c3e50;
        }
        ul {
            list-style-type: none;
            padding-left: 0;
        }
        li {
            margin: 10px 0;
        }
        .note {
            background-color: #f39c12;
            color: white;
            padding: 10px;
            border-radius: 4px;
            margin-top: 20px;
        }
    </style>
</head>
<body>
    <header>
        <h1>Messagerie Crypto</h1>
    </header>
    
    <section>
        <h2>Introduction</h2>
        <p>Bienvenue dans le projet de messagerie crypto. Ce projet permet de créer une application de messagerie sécurisée basée sur la cryptographie. Ce guide explique comment compiler et exécuter les différents composants du projet.</p>
    </section>

    <section>
        <h2>Installation et Compilation</h2>
        <p>Pour commencer, vous devez installer toutes les dépendances nécessaires et compiler le projet.</p>

        <h3>1. Compiler le backend</h3>
        <p>Pour compiler le backend du projet (exécutable backend.exe), utilisez les commandes suivantes :</p>
        <pre>
cmake -S . -B build/
cmake --build build
        </pre>
        <p>Cette commande génère le fichier exécutable dans le dossier <code>build/Debug</code>.</p>
    </section>

    <section>
        <h2>Installation des Dépendances Frontend</h2>
        <p>Ensuite, vous devez installer les dépendances nécessaires au frontend du projet (application Node.js).</p>

        <h3>2. Installer les dépendances</h3>
        <p>Dans le dossier du frontend, exécutez la commande suivante pour installer toutes les dépendances :</p>
        <pre>
npm install
        </pre>

        <h3>3. Lancer l'application</h3>
        <p>Pour démarrer l'application frontend, utilisez la commande suivante :</p>
        <pre>
npm start
        </pre>
        <p>Si vous êtes sur Windows et souhaitez spécifier un port particulier (par exemple, le port 3001), utilisez :</p>
        <pre>
set PORT=3001 && npm start
        </pre>
        <p>Cela démarre l'application frontend sur le port 3001. Modifiez le port selon vos préférences.</p>
    </section>

    <section class="note">
        <h3>Note Importante</h3>
        <p>Assurez-vous d'avoir installé toutes les dépendances nécessaires et que vous avez configuré votre environnement avant d'exécuter ces commandes.</p>
    </section>
</body>
</html>
