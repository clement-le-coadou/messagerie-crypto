import { app, BrowserWindow, ipcMain } from "electron";
import Store from 'electron-store';  // Importer electron-store
import pkg from 'pg';
const { Client } = pkg;
import bcrypt from 'bcrypt';  // Pour sécuriser les mots de passe

// Création d'une instance de client PostgreSQL
const client = new Client({
    host: "localhost",  // Adresse du serveur PostgreSQL
    port: 5432,         // Port par défaut de PostgreSQL
    user: "postgres",   // Utilisateur
    password: "cytech0001",  // Remplace par ton mot de passe
    database: "ProjetCrypto"  // Nom de la base de données
});

client.connect(); // Connexion à PostgreSQL

// Création de l'instance de Store
const store = new Store();

function createWindow() {
    const win = new BrowserWindow({
        width: 800,
        height: 600,
        webPreferences: {
            nodeIntegration: true,
            contextIsolation: false
        }
    });

    // Vérifier si un utilisateur est connecté
    if (store.has("currentUser")) {
        win.loadFile("messages.html");
    } else {
        win.loadFile("connexion.html");
    }

    // Enregistrement d'un nouvel utilisateur
    ipcMain.on("register", async (event, userData) => {
        try {
            // Vérifier si l'email existe déjà
            const res = await client.query("SELECT * FROM users WHERE email = $1", [userData.email]);
            if (res.rows.length > 0) {
                event.reply("register-failed", "Cet email est déjà utilisé !");
                return;
            }

            // Hachage du mot de passe avant de l'enregistrer
            const hashedPassword = await bcrypt.hash(userData.password, 10);  // 10 est le nombre de salt rounds

            // Insérer le nouvel utilisateur avec mot de passe haché
            const insertQuery = "INSERT INTO users(nom, prenom, email, password) VALUES($1, $2, $3, $4) RETURNING id";
            const result = await client.query(insertQuery, [userData.nom, userData.prenom, userData.email, hashedPassword]);
            const userId = result.rows[0].id;

            // Sauvegarder les infos dans electron-store
            store.set("currentUser", { ...userData, id: userId });

            win.loadFile("messages.html");
        } catch (error) {
            console.error(error);
            event.reply("register-failed", "Une erreur s'est produite. Veuillez réessayer.");
        }
    });

    // Connexion d'un utilisateur
    ipcMain.on("login", async (event, loginData) => {
        try {
            const res = await client.query("SELECT * FROM users WHERE email = $1", [loginData.email]);

            if (res.rows.length > 0) {
                const user = res.rows[0];

                // Comparer le mot de passe avec celui haché
                const isPasswordValid = await bcrypt.compare(loginData.password, user.password);

                if (isPasswordValid) {
                    store.set("currentUser", user);
                    win.loadFile("messages.html");
                } else {
                    event.reply("login-failed", "Email ou mot de passe incorrect !");
                }
            } else {
                event.reply("login-failed", "Email ou mot de passe incorrect !");
            }
        } catch (error) {
            console.error(error);
            event.reply("login-failed", "Une erreur s'est produite. Veuillez réessayer.");
        }
    });

    // Déconnexion
    ipcMain.on("logout", () => {
        store.delete("currentUser");
        win.loadFile("connexion.html");
    });
}

app.whenReady().then(createWindow);

// Arrêter le client PostgreSQL lors de la fermeture de l'application
app.on("before-quit", () => {
    client.end();
});
