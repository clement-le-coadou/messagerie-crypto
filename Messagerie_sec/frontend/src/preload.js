// preload.js
const { contextBridge } = require('electron');
const axios = require('axios');

// Log to verify axios is loaded correctly
console.log('axios loaded in preload.js:', axios);

contextBridge.exposeInMainWorld('electron', {
    get: axios.get, // Expose the 'get' method from axios
    post: axios.post, // Expose the 'post' method from axios
    // You can add other methods like put, delete if needed
});

console.log("done exposing axios methods");
