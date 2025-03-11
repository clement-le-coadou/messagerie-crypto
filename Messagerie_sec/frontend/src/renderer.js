// Attendre que le DOM soit complètement chargé
document.addEventListener('DOMContentLoaded', () => {
    console.log('window.electron:', window.electron);

    // Vérifier si l'utilisateur est déjà connecté
    const jwtToken = localStorage.getItem('jwtToken');
    if (jwtToken) {
        console.log('Utilisateur déjà connecté, redirection vers chat.html...');
        window.location.href = "chat.html"; // Rediriger automatiquement
    }

    // Fonction pour enregistrer un utilisateur
    document.getElementById('registerForm').addEventListener('submit', (event) => {
        event.preventDefault();

        const username = document.getElementById('username').value;
        const password = document.getElementById('password').value;
        const publicKey = document.getElementById('publicKey').value;

        window.electron.post('http://localhost:8080/register', {
            username: username,
            password: password,
            publicKey: publicKey
        }, {
            headers: {
                'Content-Type': 'application/json'
            }
        })
        .then(response => {
            alert('Utilisateur enregistré avec succès !');
        })
        .catch(error => {
            console.error("Erreur lors de l'enregistrement de l'utilisateur !", error);
        });
    });

    // Fonction pour se connecter
    document.getElementById('loginForm').addEventListener('submit', (event) => {
        event.preventDefault();
    
        const username = document.getElementById('loginUsername').value;
        const password = document.getElementById('loginPassword').value;
    
        window.electron.post('http://localhost:8080/login', {
            username: username,
            password: password
        }, {
            headers: {
                'Content-Type': 'application/json'
            }
        })
        .then(data => {  // `data` est directement la réponse JSON
            console.log("Réponse du serveur:", data);
            console.log("token",data.data.token);
            console.log('userId',data.data.user_id);
            console.log("pb",data.data.public_key);
    
            if (data) {
                localStorage.setItem('jwtToken', data.data.token); // Stocker le JWT
                localStorage.setItem('username', username); // Stocker le username
                localStorage.setItem('user_Id',data.data.user_id);
                localStorage.setItem('public_key',data.data.public_key);
                alert('Connexion réussie !');
                window.location.href = "chat.html"; // Rediriger vers la messagerie
            } else {
                alert("Erreur d'authentification.");
            }
        })
        .catch(error => {
            console.error("Erreur lors de la connexion !", error);
        });
    });
    


});
