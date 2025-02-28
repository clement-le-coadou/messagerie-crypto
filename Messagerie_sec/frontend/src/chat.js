

// Wait for the DOM to be fully loaded
document.addEventListener('DOMContentLoaded', () => {
    const socket = new WebSocket("ws://localhost:8080");

// A variable to track the WebSocket state
let isConnected = false;
    // WebSocket connection
    socket.addEventListener("open", () => {
        console.log("WebSocket connecté");
        isConnected = true; // Set connection state to true when WebSocket is open
    });

    // Listen for incoming messages
    socket.addEventListener("message", (event) => {
        const data = JSON.parse(event.data);
        displayMessage(data.sender, data.content, data.status);
    });

    // Function to display a message
    function displayMessage(sender, content, status) {
        const messagesDiv = document.getElementById("messages");
        const messageElement = document.createElement("div");

        // Define the style based on whether the message is sent or received
        messageElement.classList.add("message");
        messageElement.classList.add(status === "sent" ? "sent" : "received");

        messageElement.innerHTML = `<b>${sender}:</b> ${content} <i>(${status})</i>`;
        messagesDiv.appendChild(messageElement);

        // Scroll to the bottom of the chat
        messagesDiv.scrollTop = messagesDiv.scrollHeight;
    }

    // Function to send a message
    function sendMessage() {
        const messageContent = document.getElementById("messageContent").value;
        if (!messageContent.trim()) return;

        if (!isConnected) {
            console.error("WebSocket is not connected yet.");
            alert("WebSocket connection is still establishing. Please try again later.");
            return;
        }

        const message = {
            sender: "User", // Replace with actual user
            content: messageContent,
            status: "sent"
        };

        // Send the message only if WebSocket is connected
        socket.send(JSON.stringify(message));

        // Display the message immediately on the user's side
        displayMessage("You", messageContent, "sent");

        // Clear the input field
        document.getElementById("messageContent").value = '';
    }

    // Event listener for the send button
    document.getElementById("sendMessage").addEventListener("click", (event) => {
        event.preventDefault();
        sendMessage();
    });

    // Optionally, press "Enter" to send the message
    document.getElementById("messageContent").addEventListener("keypress", (e) => {
        if (e.key === "Enter") {
            sendMessage();
        }
    });
});
