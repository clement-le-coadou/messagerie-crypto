document.addEventListener("DOMContentLoaded", () => {
    let jwtToken = localStorage.getItem("jwtToken");
    let jwtUserId = localStorage.getItem("user_Id");
    let jwtPublicKey = localStorage.getItem("public_key");

    const contactList = document.getElementById("contact-list");
    const searchInput = document.getElementById("searchInput");
    const addContactButton = document.getElementById("addContact");
    const requestList = document.getElementById("request-list");

    if (!jwtToken || !jwtUserId || !jwtPublicKey) {
        alert("Veuillez vous connecter d'abord.");
        console.log("pb",jwtPublicKey);
        window.location.href = "index.html";
        return;
    }

    // Rafraîchir les contacts et les demandes toutes les 10 secondes
    fetchContacts();
    fetchRequests();
    setInterval(() => {
        fetchContacts();
        fetchRequests();
    }, 10000);

    /** Affiche un contact dans la liste **/
    function displayContact(id, username) {
        const contactElement = document.createElement("li");
        contactElement.textContent = username;
        contactElement.dataset.contactId = id;
        contactElement.classList.add("contact-item");
        contactList.appendChild(contactElement);
    }

    /** Récupère la liste des contacts **/
    function fetchContacts() {
        fetch("https://localhost:8080/get_contacts", {
            method: "POST",
            headers: {
                "Content-Type": "application/json",
                "Authorization": `Bearer ${jwtToken}`
            },
            body: JSON.stringify({ user_id: jwtUserId })
        })
        .then(response => response.json())
        .then(contacts => {
            contactList.innerHTML = ""; // Vider la liste avant de la remplir
            contacts.forEach(contact => displayContact(contact.id, contact.username));
        })
        .catch(error => console.error("Erreur lors de la récupération des contacts :", error));
    }

    /** Récupère la liste des demandes de contact en attente **/
    function fetchRequests() {
        fetch("https://localhost:8080/get_contact_requests", {
            method: "POST",
            headers: {
                "Content-Type": "application/json",
                "Authorization": `Bearer ${jwtToken}`
            },
            body: JSON.stringify({ user_id: parseInt(jwtUserId) })
        })
        .then(response => response.json())
        .then(requests => {
            requestList.innerHTML = ""; // Vider la liste avant de la remplir
            requests.forEach(req => displayRequest(req.id, req.username));
        })
        .catch(error => console.error("Erreur lors de la récupération des demandes :", error));
    }

    /** Affiche une demande de contact avec les boutons Accepter/Refuser **/
    function displayRequest(id, username) {
        const requestElement = document.createElement("li");
        requestElement.textContent = username;

        const acceptButton = document.createElement("button");
        acceptButton.textContent = "Accepter";
        acceptButton.onclick = () => handleRequest(id, true);

        const rejectButton = document.createElement("button");
        rejectButton.textContent = "Refuser";
        rejectButton.onclick = () => handleRequest(id, false);

        requestElement.appendChild(acceptButton);
        requestElement.appendChild(rejectButton);
        requestList.appendChild(requestElement);
    }

    /** Gère l'acceptation ou le refus d'une demande **/
    function handleRequest(requestId, accept) {
        fetch("https://localhost:8080/handle_request", {
            method: "POST",
            headers: {
                "Content-Type": "application/json",
                "Authorization": `Bearer ${jwtToken}`
            },
            body: JSON.stringify({ user_id: parseInt(jwtUserId), request_id: parseInt(requestId), accept })
        })
        .then(response => {
            if (!response.ok) throw new Error("Échec du traitement de la demande");
            return response.json();
        })
        .then(() => {
            fetchRequests(); // Rafraîchir la liste des demandes
            fetchContacts(); // Rafraîchir les contacts en cas d'acceptation
        })
        .catch(error => console.error("Erreur lors du traitement de la demande :", error));
    }

    /** Envoie une demande de contact **/
    addContactButton.addEventListener("click", () => {
        const publicKey = searchInput.value.trim();
        if (!publicKey) {
            alert("Veuillez entrer une clé publique.");
            return;
        }

        // Désactiver le bouton pour éviter les requêtes multiples
        addContactButton.disabled = true;

        fetch("https://localhost:8080/send_contact_request", {
            method: "POST",
            headers: {
                "Content-Type": "application/json",
                "Authorization": `Bearer ${jwtToken}`
            },
            body: JSON.stringify({ user_id: parseInt(jwtUserId), public_key: publicKey })
        })
        .then(response => {
            if (!response.ok) throw new Error("Échec de l'envoi de la demande");
            return response.json();
        })
        .then(() => {
            alert("Demande envoyée !");
            searchInput.value = ""; // Réinitialiser le champ après l'envoi
        })
        .catch(error => {
            console.error("Erreur lors de l'ajout de contact :", error);
            alert("Erreur lors de l'envoi de la demande.");
        })
        .finally(() => {
            addContactButton.disabled = false; // Réactiver le bouton après la requête
        });
    });
});
