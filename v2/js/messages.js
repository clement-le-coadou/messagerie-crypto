const newConversationBtn = document.getElementById("new-conversation-btn");
const chatHeaderTitle = document.getElementById("chat-header-title");
const messagesContainer = document.querySelector(".messages");
const messageInput = document.getElementById("message-input");
const sendMessageBtn = document.getElementById("send-message-btn");

let currentConversationId = null;
let conversations = [
    {
        id: '1',
        name: 'John Doe',
        messages: [
            { sender: 'sent', text: 'Salut, comment ça va ?', status: 'Envoyé' },
            { sender: 'received', text: 'Ça va bien, et toi ?', status: 'Reçu' }
        ]
    },
    {
        id: '2',
        name: 'Alice',
        messages: [
            { sender: 'sent', text: 'À bientôt !', status: 'Délivré' }
        ]
    }
];

// Affichage d'une conversation
function selectConversation(conversationId) {
    const conversation = conversations.find(c => c.id === conversationId);
    currentConversationId = conversationId;
    chatHeaderTitle.textContent = `Discussion avec ${conversation.name}`;
    messagesContainer.innerHTML = '';

    conversation.messages.forEach(message => {
        const messageElement = document.createElement('div');
        messageElement.classList.add('message', message.sender);
        messageElement.innerHTML = `
            <p>${message.text}</p>
            <span class="message-status">${message.status}</span>
        `;
        messagesContainer.appendChild(messageElement);
    });

    messageInput.focus();
}

// Envoi d'un message
sendMessageBtn.addEventListener('click', () => {
    const messageText = messageInput.value.trim();
    if (messageText && currentConversationId) {
        const conversation = conversations.find(c => c.id === currentConversationId);
        const message = { sender: 'sent', text: messageText, status: 'Envoyé' };
        conversation.messages.push(message);

        selectConversation(currentConversationId);  // Rechargement de la conversation avec le nouveau message
        messageInput.value = '';
    }
});

// Création d'une nouvelle conversation (fonctionnalité de base)
newConversationBtn.addEventListener('click', () => {
    const newConvName = prompt("Entrez le nom du nouveau contact :");
    if (newConvName) {
        const newConversation = {
            id: `${conversations.length + 1}`,
            name: newConvName,
            messages: []
        };
        conversations.push(newConversation);
        renderConversationList();
    }
});

// Initialiser la liste des conversations
function renderConversationList() {
    const conversationList = document.querySelector(".conversation-list");
    conversationList.innerHTML = '';

    conversations.forEach(conversation => {
        const conversationElement = document.createElement('div');
        conversationElement.classList.add('conversation');
        conversationElement.onclick = () => selectConversation(conversation.id);
        conversationElement.innerHTML = `
            <div class="conversation-avatar">${conversation.name[0]}</div>
            <div class="conversation-info">
                <p class="conversation-name">${conversation.name}</p>
                <p class="conversation-last-message">${conversation.messages[conversation.messages.length - 1]?.text || ''}</p>
            </div>
        `;
        conversationList.appendChild(conversationElement);
    });
}

// Initialisation
renderConversationList();
