# messagerie-crypto
cmake -S . -B H:\BuildsVisualStudio -A x64   
cmake --build H:\BuildsVisualStudio
On se place ensuite dans : H:\BuildsVisualStudio avec le terminal VisualStudio
.\build\Debug\backend.exe

On reveint dans le dossier frontend, car c'est là que npm est reconnu.
Donc pour moi : C:\Pascal\VisualCodeProjects\JavaScriptWorkspace\SecureMessagingApp\messagerie-crypto\Messagerie_sec\frontend
npm install
npm start
set PORT=3001 && npm start  # (Windows)

