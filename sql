CREATE DATABASE secure_messenger;

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
    signature TEXT NOT NULL DEFAULT '',
    status VARCHAR(20) DEFAULT 'sent',
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE contacts (
    user_id INT REFERENCES users(id) ON DELETE CASCADE,
    contact_id INT REFERENCES users(id) ON DELETE CASCADE,
    PRIMARY KEY (user_id, contact_id),
    CONSTRAINT contact_unique CHECK (user_id != contact_id)
);

INSERT INTO users (username, password_hash, public_key) VALUES
('alice', 'password1', 'public_key_1'),
('bob', 'password2', 'public_key_2'),
('charlie', 'password3', 'public_key_3'),
('david', 'password4', 'public_key_4'),
('eva', 'password5', 'public_key_5');

INSERT INTO messages (sender_id, receiver_id, content, status, signature) VALUES
(1, 2, 'Salut Bob, comment ça va ?', 'sent', 'signature1'),
(2, 1, 'Salut Alice, je vais bien et toi ?', 'sent', 'signature2'),
(3, 4, 'Charlie ici, test de message.', 'sent', 'signature3'),
(4, 5, 'David envoie un message à Eva.', 'sent', 'signature4'),
(5, 1, 'Eva envoie un message à Alice.', 'sent', 'signature5'),
(1, 3, 'Alice envoie un message à Charlie.', 'sent', 'signature6'),
(2, 5, 'Bob envoie un message à Eva.', 'sent', 'signature7'),
(3, 2, 'Charlie envoie un message à Bob.', 'sent', 'signature8');

INSERT INTO contacts (user_id, contact_id) VALUES (1, 2);
INSERT INTO contacts (user_id, contact_id) VALUES (2, 1);

INSERT INTO contacts (user_id, contact_id) VALUES (1, 3);
INSERT INTO contacts (user_id, contact_id) VALUES (3, 1);

INSERT INTO contacts (user_id, contact_id) VALUES (1, 4);
INSERT INTO contacts (user_id, contact_id) VALUES (4, 1);

INSERT INTO contacts (user_id, contact_id) VALUES (1, 5);
INSERT INTO contacts (user_id, contact_id) VALUES (5, 1);

INSERT INTO contacts (user_id, contact_id) VALUES (2, 3);
INSERT INTO contacts (user_id, contact_id) VALUES (3, 2);

INSERT INTO contacts (user_id, contact_id) VALUES (2, 4);
INSERT INTO contacts (user_id, contact_id) VALUES (4, 2);

INSERT INTO contacts (user_id, contact_id) VALUES (2, 5);
INSERT INTO contacts (user_id, contact_id) VALUES (5, 2);

INSERT INTO contacts (user_id, contact_id) VALUES (3, 4);
INSERT INTO contacts (user_id, contact_id) VALUES (4, 3);

INSERT INTO contacts (user_id, contact_id) VALUES (3, 5);
INSERT INTO contacts (user_id, contact_id) VALUES (5, 3);

INSERT INTO contacts (user_id, contact_id) VALUES (4, 5);
INSERT INTO contacts (user_id, contact_id) VALUES (5, 4);

SELECT * FROM users;
SELECT * FROM messages;
SELECT * FROM contacts;
