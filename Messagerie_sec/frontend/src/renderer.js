// Attendre que le DOM soit complètement chargé
document.addEventListener('DOMContentLoaded', () => {
    console.log('window.electron:', window.electron);
    
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
            alert('User registered successfully!');
        })
        .catch(error => {
            console.error('There was an error registering the user!', error);
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
        .then(response => {
            alert('User logged in successfully!');
            // Stocker le nom d'utilisateur en local (optionnel)
            localStorage.setItem('username', username);

            // Rediriger vers la page de messagerie
            window.location.href = "chat.html";
        })
        .catch(error => {
            console.error('There was an error logging in!', error);
        });
    });
});
