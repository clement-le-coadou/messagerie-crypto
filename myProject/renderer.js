const messageInput = document.getElementById('message');
const messagesList = document.getElementById('messages');

function sendMessage() {
    const msg = messageInput.value;
    if (msg.trim() !== '') {
        const li = document.createElement('li');
        li.textContent = msg;
        messagesList.appendChild(li);
        messageInput.value = '';
    }
}
