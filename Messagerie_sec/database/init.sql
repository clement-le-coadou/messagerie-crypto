-- Création de la base de données
CREATE DATABASE secure_messenger;

\c secure_messenger;

-- Table des utilisateurs
CREATE TABLE users (
    id SERIAL PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    password_hash TEXT NOT NULL,
    public_key TEXT NOT NULL
);

-- Table des messages
CREATE TABLE messages (
    id SERIAL PRIMARY KEY,
    sender_id INT REFERENCES users(id) ON DELETE CASCADE,
    receiver_id INT REFERENCES users(id) ON DELETE CASCADE,
    content TEXT NOT NULL,
    signature TEXT NOT NULL,  -- Signature numérique du message
    status VARCHAR(20) DEFAULT 'sent',  -- Statut du message (sent, delivered, read)
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Table des contacts
CREATE TABLE contacts (
    user_id INT REFERENCES users(id) ON DELETE CASCADE,  -- Référence à l'utilisateur
    contact_id INT REFERENCES users(id) ON DELETE CASCADE,  -- Référence au contact
    PRIMARY KEY (user_id, contact_id),
    CONSTRAINT contact_unique CHECK (user_id != contact_id)  -- Empêche un utilisateur d'être son propre contact
);

CREATE TABLE contact_requests (
    sender_id INT,
    receiver_id INT,
    status VARCHAR(10), -- 'pending', 'accepted', 'rejected'
    PRIMARY KEY (sender_id, receiver_id),
    FOREIGN KEY (sender_id) REFERENCES users(id),
    FOREIGN KEY (receiver_id) REFERENCES users(id)
);


-- Insertion des utilisateurs
INSERT INTO users (username, password_hash, public_key) VALUES
('alice', 'password1', 'public_key_1'),
('bob', 'password2', 'public_key_2'),
('charlie', 'password3', 'public_key_3'),
('david', 'password4', 'public_key_4'),
('eva', 'password5', 'public_key_5');

-- Insertion des messages
INSERT INTO messages (sender_id, receiver_id, content, status, signature) VALUES
(1, 2, 'Salut Bob, comment ça va ?', 'sent', 'signature1'),
(2, 1, 'Salut Alice, je vais bien et toi ?', 'sent', 'signature2'),
(3, 4, 'Charlie ici, test de message.', 'sent', 'signature3'),
(4, 5, 'David envoie un message à Eva.', 'sent', 'signature4'),
(5, 1, 'Eva envoie un message à Alice.', 'sent', 'signature5'),
(1, 3, 'Alice envoie un message à Charlie.', 'sent', 'signature6'),
(2, 5, 'Bob envoie un message à Eva.', 'sent', 'signature7'),
(3, 2, 'Charlie envoie un message à Bob.', 'sent', 'signature8');

-- Insertion des contacts (relations entre utilisateurs)
-- Alice ajoute Bob
INSERT INTO contacts (user_id, contact_id) VALUES (1, 2);
-- Bob ajoute Alice
INSERT INTO contacts (user_id, contact_id) VALUES (2, 1);

-- Alice ajoute Charlie
INSERT INTO contacts (user_id, contact_id) VALUES (1, 3);
-- Charlie ajoute Alice
INSERT INTO contacts (user_id, contact_id) VALUES (3, 1);

-- Alice ajoute David
INSERT INTO contacts (user_id, contact_id) VALUES (1, 4);
-- David ajoute Alice
INSERT INTO contacts (user_id, contact_id) VALUES (4, 1);

-- Alice ajoute Eva
INSERT INTO contacts (user_id, contact_id) VALUES (1, 5);
-- Eva ajoute Alice
INSERT INTO contacts (user_id, contact_id) VALUES (5, 1);

-- Bob ajoute Charlie
INSERT INTO contacts (user_id, contact_id) VALUES (2, 3);
-- Charlie ajoute Bob
INSERT INTO contacts (user_id, contact_id) VALUES (3, 2);

-- Bob ajoute David
INSERT INTO contacts (user_id, contact_id) VALUES (2, 4);
-- David ajoute Bob
INSERT INTO contacts (user_id, contact_id) VALUES (4, 2);

-- Bob ajoute Eva
INSERT INTO contacts (user_id, contact_id) VALUES (2, 5);
-- Eva ajoute Bob
INSERT INTO contacts (user_id, contact_id) VALUES (5, 2);

-- Charlie ajoute David
INSERT INTO contacts (user_id, contact_id) VALUES (3, 4);
-- David ajoute Charlie
INSERT INTO contacts (user_id, contact_id) VALUES (4, 3);

-- Charlie ajoute Eva
INSERT INTO contacts (user_id, contact_id) VALUES (3, 5);
-- Eva ajoute Charlie
INSERT INTO contacts (user_id, contact_id) VALUES (5, 3);

-- David ajoute Eva
INSERT INTO contacts (user_id, contact_id) VALUES (4, 5);
-- Eva ajoute David
INSERT INTO contacts (user_id, contact_id) VALUES (5, 4);

-- Vérification des données
SELECT * FROM users;
SELECT * FROM messages;
SELECT * FROM contacts;
