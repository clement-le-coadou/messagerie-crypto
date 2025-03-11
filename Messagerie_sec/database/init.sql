CREATE DATABASE secure_messenger;

\c secure_messenger;

CREATE TABLE users (
    id SERIAL PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    password_hash TEXT NOT NULL,
    public_key TEXT NOT NULL
);

CREATE TABLE messages (
    id SERIAL PRIMARY KEY,
    sender_id INT REFERENCES users(id) ON DELETE CASCADE,
    receiver_id INT REFERENCES users(id) ON DELETE CASCADE,
    content TEXT NOT NULL,
    signature TEXT NOT NULL DEFAULT '',  -- Signature numérique du message
    status VARCHAR(20) DEFAULT 'sent',  -- Statut du message (sent, delivered, read)
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);


-- Insertion de plusieurs utilisateurs
INSERT INTO users (username, password_hash, public_key) VALUES
('alice', 'password1', 'public_key_1'),
('bob', 'password2', 'public_key_2'),
('charlie', 'password3', 'public_key_3'),
('david', 'password4', 'public_key_4'),
('eva', 'password5', 'public_key_5');

-- Insertion de messages entre utilisateurs
INSERT INTO messages (sender_id, receiver_id, content, status, signature) VALUES
(1, 2, 'Salut Bob, comment ça va ?', 'sent', 'signature1'),
(2, 1, 'Salut Alice, je vais bien et toi ?', 'sent', 'signature2'),
(3, 4, 'Charlie ici, test de message.', 'sent', 'signature3'),
(4, 5, 'David envoie un message à Eva.', 'sent', 'signature4'),
(5, 1, 'Eva envoie un message à Alice.', 'sent', 'signature5'),
(1, 3, 'Alice envoie un message à Charlie.', 'sent', 'signature6'),
(2, 5, 'Bob envoie un message à Eva.', 'sent', 'signature7'),
(3, 2, 'Charlie envoie un message à Bob.', 'sent', 'signature8');

-- Vérification des données
SELECT * FROM users;
SELECT * FROM messages;