const { app, BrowserWindow, session } = require('electron');
const fs = require('fs');
const path = require('path');

console.log('[Electron] Lancement de l application');

// Definir le chemin vers le certificat
const certPath = 'H:/Pascal/VisualCodeProjects/JavaScriptWorkspace/SecureMessagingApp/messagerie-crypto/Messagerie_sec/backend/certs/server.crt';
console.log('[Electron] Chemin du certificat :', certPath);

let certData;
try {
    certData = fs.readFileSync(certPath);
    console.log('[Electron] Certificat charge avec succes.');
} catch (err) {
    console.error('[Electron] Erreur lors du chargement du certificat:', err);
}

// Fonction pour accepter les certificats auto-signes pour localhost
function acceptSelfSignedCertificates() {
    console.log('[Electron] Configuration de la verification des certificats');
    session.defaultSession.setCertificateVerifyProc((request, callback) => {
        console.log(`[Electron] Verification du certificat pour: ${request.hostname}`);
        if (request.hostname === 'localhost') {
            console.log('[Electron] Certificat accepte pour localhost');
            callback(0);  // Certificat accepte
        } else {
            console.warn(`[Electron] Certificat refuse pour: ${request.hostname}`);
            callback(-2); // Autres refuses
        }
    });
}

let mainWindow;

function createWindow() {
    console.log('[Electron] Creation de la fenetre');

    mainWindow = new BrowserWindow({
        width: 800,
        height: 600,
        webPreferences: {
            preload: path.join(__dirname, 'preload.js'),
            nodeIntegration: true,
            contextIsolation: true,
            webSecurity: true,
        }
    });

    // Accepter le certificat auto-signe
    acceptSelfSignedCertificates();

    console.log('[Electron] Chargement de index.html');
    mainWindow.loadFile('src/index.html');

    mainWindow.webContents.on('did-finish-load', () => {
        console.log('[Electron] index.html charge dans la fenetre');
    });

    mainWindow.on('closed', () => {
        console.log('[Electron] Fenetre fermee');
        mainWindow = null;
    });
}

app.on('ready', () => {
    console.log('[Electron] App prete');
    createWindow();
});

app.on('window-all-closed', () => {
    console.log('[Electron] Toutes les fenetres sont fermees');
    if (process.platform !== 'darwin') {
        console.log('[Electron] Fermeture de l application');
        app.quit();
    }
});

app.on('activate', () => {
    console.log('[Electron] Reactivation de l application');
    if (mainWindow === null) {
        createWindow();
    }
});
