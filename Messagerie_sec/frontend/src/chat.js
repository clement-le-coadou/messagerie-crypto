document.addEventListener('DOMContentLoaded', () => {
    const messagesDiv = document.getElementById("messages");
    const loginForm = document.getElementById("loginForm");
    const messageContent = document.getElementById("messageContent");
    const sendMessageButton = document.getElementById("sendMessage");
    const contactList = document.getElementById("contact-list");
    let selectedContactId = null; // Contact actuellement sélectionné


    let jwtToken = localStorage.getItem("jwtToken"); // Check for existing JWT token
    let jwtUsername = localStorage.getItem('username'); // Stocker le username
    let jwtUserId = localStorage.getItem('user_Id');
    if (jwtUserId === null || isNaN(jwtUserId)) {
        alert("Invalid user ID");
    }
    fetchContacts(); // Charger les contacts au démarrage
    setInterval(fetchContacts, 10000);

    // // Function to login
    // function login() {
    //     const username = document.getElementById("username").value;
    //     const password = document.getElementById("password").value;

    //     fetch("http://localhost:8080/login", {
    //         method: "POST",
    //         headers: {
    //             "Content-Type": "application/json"
    //         },
    //         body: JSON.stringify({ username, password })
    //     })
    //     .then(response => response.text())
    //     .then(token => {
    //         if (token) {
    //             jwtToken = token;
    //             localStorage.setItem("jwtToken", jwtToken); // Store JWT token
    //             alert("Login successful");
    //             loginForm.style.display = "none"; // Hide login form
    //         } else {
    //             alert("Login failed");
    //         }
    //     })
    //     .catch(error => console.error("Login error:", error));
    // }

    function displayContact(id, username) {
        const contactElement = document.createElement("li");
        contactElement.textContent = username;
        contactElement.dataset.contactId = id;
        contactElement.classList.add("contact-item");
    
        contactElement.addEventListener("click", () => {
            document.querySelectorAll(".contact-item").forEach(el => el.classList.remove("active"));
            contactElement.classList.add("active");
    
            selectedContactId = id;
            console.log(selectedContactId);
            fetchMessages(selectedContactId);
        });
    
        contactList.appendChild(contactElement);
    }
    

    

    function fetchContacts() {
        if (!jwtToken) {
            alert("Veuillez vous connecter d'abord.");
            return;
        }
    
        fetch("http://localhost:8080/get_contacts", {
            method: "POST",
            headers: {
                "Content-Type": "application/json",
                "Authorization": `Bearer ${jwtToken}`
            },
            body: JSON.stringify({ user_id: jwtUserId })
        })
        .then(response => response.json())
        .then(contacts => {
            contactList.innerHTML = ""; // Nettoyer la liste avant d'ajouter les contacts
            contacts.forEach(contact => displayContact(contact.id, contact.username));
        })
        .catch(error => console.error("Erreur lors de la récupération des contacts :", error));
    }
    
    function fetchMessages(contactId) {
        console.log(contactId);
        if (!jwtToken) {
            alert("Veuillez vous connecter d'abord.");
            return;
        }
    
        if (!contactId) {
            console.error("Aucun contact sélectionné, impossible de récupérer les messages.");
            return;
        }
    
        fetch("http://localhost:8080/get_messages", {
            method: "POST",
            headers: {
                "Content-Type": "application/json",
                "Authorization": `Bearer ${jwtToken}`
            },
            body: JSON.stringify({ user_id: parseInt(jwtUserId), contact_id: parseInt(contactId) }) // ✅ Convertir en nombre
        })
        .then(response => response.json())
        .then(messages => {
            messagesDiv.innerHTML = "";
            messages.forEach(msg => displayMessage(msg.sender, msg.content));
        })
        .catch(error => console.error("Erreur lors de la récupération des messages :", error));
    }
    
    

// Function to display messages
function displayMessage(sender, content) {
    const messageElement = document.createElement("div");
    messageElement.classList.add("message");

    // Vérifier si le message a été envoyé par l'utilisateur actuel
    if (sender == jwtUserId) {
        messageElement.classList.add("sent"); // Ajoute la classe "sent"
    } else {
        messageElement.classList.add("received"); // Ajoute la classe "received"
    }

    messageElement.innerHTML = `</b> ${content}`;
    messagesDiv.appendChild(messageElement);
    messagesDiv.scrollTop = messagesDiv.scrollHeight; // Scroll vers le bas automatiquement
}


    function sendMessage() {
        if (!jwtToken) {
            alert("Veuillez vous connecter d'abord.");
            return;
        }
    
        if (!selectedContactId) {
            alert("Veuillez sélectionner un contact.");
            return;
        }
    
        const content = messageContent.value;
        if (!content.trim()) return;
    
        const message = {
            sender_id: parseInt(jwtUserId), // ✅ Ajouter le sender_id depuis localStorage
            recipient: selectedContactId,  // ✅ Destinataire
            content
        };
    
        fetch("http://localhost:8080/send_message", {
            method: "POST",
            headers: {
                "Content-Type": "application/json",
                "Authorization": `Bearer ${jwtToken}`
            },
            body: JSON.stringify(message)
        })
        .then(response => {
            if (response.ok) {
                displayMessage("Vous", content);
                messageContent.value = "";
            } else {
                console.error("Erreur lors de l'envoi du message");
            }
        })
        .catch(error => console.error("Erreur :", error));
    }
    

    
    // Send message on button click
    sendMessageButton.addEventListener("click", (event) => {
        event.preventDefault();
        sendMessage();
    });

    // Fonction pour se déconnecter
    document.getElementById('logoutButton')?.addEventListener('click', () => {
        localStorage.removeItem('jwtToken'); // Supprimer le JWT
        localStorage.removeItem('jwtUsername'); // Supprimer le username
        localStorage.removeItem('jwtUserId')
        alert("Déconnexion réussie !");
        window.location.href = "index.html"; // Rediriger vers la page de connexion
    });

    setInterval(() => {
        if (selectedContactId) {
            fetchMessages(selectedContactId);
        }
    }, 1000); // ⏳ 5 secondes pour éviter trop de requêtes
    
});
