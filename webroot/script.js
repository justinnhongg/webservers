const logBox = document.getElementById("log");
const agent = document.getElementById("agent");
const arena = document.getElementById("arena");

const messages = [
  "🌐 Rotating proxy... IP: 192.168.1.10",
  "🔍 Performing JA3 fingerprint obfuscation...",
  "✅ Stealth passed: No automation detected",
  "🤖 Simulating human mouse movement...",
  "🧠 LLM agent selected next action: Click login",
  "🔒 CAPTCHA intercepted and bypassed",
  "📦 Payload received: Extracting session token"
];

function log(msg) {
  const time = new Date().toLocaleTimeString();
  logBox.innerHTML += `[${time}] ${msg}<br>`;
  logBox.scrollTop = logBox.scrollHeight;
}

function randomPosition() {
  const x = Math.random() * (arena.clientWidth - 40);
  const y = Math.random() * (arena.clientHeight - 40);
  agent.style.left = `${x}px`;
  agent.style.top = `${y}px`;
}

function startSession() {
  logBox.innerHTML = '';
  let steps = 0;
  const maxSteps = 10;
  const interval = setInterval(() => {
    randomPosition();
    log(messages[Math.floor(Math.random() * messages.length)]);
    steps++;
    if (steps >= maxSteps) clearInterval(interval);
  }, 1000);
}

