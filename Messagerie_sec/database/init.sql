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
    signature TEXT NOT NULL,  -- Signature numérique du message
    status VARCHAR(20) DEFAULT 'sent',  -- Statut du message (sent, delivered, read)
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

