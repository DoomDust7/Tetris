// ─── Constants ───────────────────────────────────────────────────────────────
const COLS = 10;
const ROWS = 20;
const CELL = 30;
const PREVIEW_CELL = 22;

const COLORS = {
  I: '#00cfcf',
  O: '#f5c800',
  T: '#a020f0',
  S: '#22cc44',
  Z: '#ee3333',
  J: '#2255ee',
  L: '#ff8800',
};

const GRID_COLOR   = '#111122';
const BORDER_COLOR = '#1e1e33';

// ─── Piece Shapes (SRS — offset arrays [col, row] per rotation) ───────────────
const SHAPES = {
  I: [
    [[0,1],[1,1],[2,1],[3,1]],
    [[2,0],[2,1],[2,2],[2,3]],
    [[0,2],[1,2],[2,2],[3,2]],
    [[1,0],[1,1],[1,2],[1,3]],
  ],
  O: [
    [[1,0],[2,0],[1,1],[2,1]],
    [[1,0],[2,0],[1,1],[2,1]],
    [[1,0],[2,0],[1,1],[2,1]],
    [[1,0],[2,0],[1,1],[2,1]],
  ],
  T: [
    [[1,0],[0,1],[1,1],[2,1]],
    [[1,0],[1,1],[2,1],[1,2]],
    [[0,1],[1,1],[2,1],[1,2]],
    [[1,0],[0,1],[1,1],[1,2]],
  ],
  S: [
    [[1,0],[2,0],[0,1],[1,1]],
    [[1,0],[1,1],[2,1],[2,2]],
    [[1,1],[2,1],[0,2],[1,2]],
    [[0,0],[0,1],[1,1],[1,2]],
  ],
  Z: [
    [[0,0],[1,0],[1,1],[2,1]],
    [[2,0],[1,1],[2,1],[1,2]],
    [[0,1],[1,1],[1,2],[2,2]],
    [[1,0],[0,1],[1,1],[0,2]],
  ],
  J: [
    [[0,0],[0,1],[1,1],[2,1]],
    [[1,0],[2,0],[1,1],[1,2]],
    [[0,1],[1,1],[2,1],[2,2]],
    [[1,0],[1,1],[0,2],[1,2]],
  ],
  L: [
    [[2,0],[0,1],[1,1],[2,1]],
    [[1,0],[1,1],[1,2],[2,2]],
    [[0,1],[1,1],[2,1],[0,2]],
    [[0,0],[1,0],[1,1],[1,2]],
  ],
};

// SRS wall kick data (JLSTZ pieces)
const WALL_KICKS = {
  '0->1': [[ 0,0],[-1,0],[-1,-1],[0, 2],[-1, 2]],
  '1->0': [[ 0,0],[ 1,0],[ 1, 1],[0,-2],[ 1,-2]],
  '1->2': [[ 0,0],[ 1,0],[ 1, 1],[0,-2],[ 1,-2]],
  '2->1': [[ 0,0],[-1,0],[-1,-1],[0, 2],[-1, 2]],
  '2->3': [[ 0,0],[ 1,0],[ 1,-1],[0, 2],[ 1, 2]],
  '3->2': [[ 0,0],[-1,0],[-1, 1],[0,-2],[-1,-2]],
  '3->0': [[ 0,0],[-1,0],[-1, 1],[0,-2],[-1,-2]],
  '0->3': [[ 0,0],[ 1,0],[ 1,-1],[0, 2],[ 1, 2]],
};

// SRS wall kick data (I piece — different offsets)
const WALL_KICKS_I = {
  '0->1': [[ 0,0],[-2,0],[ 1,0],[-2, 1],[ 1,-2]],
  '1->0': [[ 0,0],[ 2,0],[-1,0],[ 2,-1],[-1, 2]],
  '1->2': [[ 0,0],[-1,0],[ 2,0],[-1,-2],[ 2, 1]],
  '2->1': [[ 0,0],[ 1,0],[-2,0],[ 1, 2],[-2,-1]],
  '2->3': [[ 0,0],[ 2,0],[-1,0],[ 2,-1],[-1, 2]],
  '3->2': [[ 0,0],[-2,0],[ 1,0],[-2, 1],[ 1,-2]],
  '3->0': [[ 0,0],[ 1,0],[-2,0],[ 1, 2],[-2,-1]],
  '0->3': [[ 0,0],[-1,0],[ 2,0],[-1,-2],[ 2, 1]],
};

const PIECE_TYPES = ['I','O','T','S','Z','J','L'];
const POINTS = [0, 100, 300, 500, 800];

// ─── Canvas contexts ──────────────────────────────────────────────────────────
const boardCanvas   = document.getElementById('board');
const previewCanvas = document.getElementById('preview');
const boardCtx      = boardCanvas.getContext('2d');
const previewCtx    = previewCanvas.getContext('2d');

// ─── Game State ───────────────────────────────────────────────────────────────
let state;

function createState() {
  return {
    board:        createBoard(),
    current:      null,
    next:         null,
    bag:          [],
    score:        0,
    level:        1,
    lines:        0,
    gameOver:     false,
    paused:       false,
    dropInterval: 800,
    lastDrop:     0,
    animId:       null,
    lockDelay:    500,    // ms of "lock delay" before locking on floor
    lockTimer:    null,
    lowestY:      0,      // track lowest reached y to reset lock delay on move
  };
}

function createBoard() {
  return Array.from({ length: ROWS }, () => Array(COLS).fill(0));
}

// ─── 7-Bag Randomizer ─────────────────────────────────────────────────────────
function nextFromBag() {
  if (state.bag.length === 0) {
    state.bag = [...PIECE_TYPES].sort(() => Math.random() - 0.5);
  }
  return state.bag.pop();
}

function makePiece(type) {
  return { type, rotation: 0, x: 3, y: -1 };
}

// ─── Collision Detection ──────────────────────────────────────────────────────
function isValidPosition(type, rotation, x, y) {
  for (const [dc, dr] of SHAPES[type][rotation]) {
    const col = x + dc;
    const row = y + dr;
    if (col < 0 || col >= COLS || row >= ROWS) return false;
    if (row < 0) continue;
    if (state.board[row][col] !== 0) return false;
  }
  return true;
}

function isOnFloor(type, rotation, x, y) {
  return !isValidPosition(type, rotation, x, y + 1);
}

// ─── Spawn & Lock ─────────────────────────────────────────────────────────────
function spawnPiece() {
  state.current = state.next || makePiece(nextFromBag());
  state.next    = makePiece(nextFromBag());
  state.lowestY = state.current.y;

  if (!isValidPosition(state.current.type, state.current.rotation,
                        state.current.x, state.current.y)) {
    triggerGameOver();
  }
}

function lockPiece() {
  clearLockTimer();
  const { type, rotation, x, y } = state.current;
  for (const [dc, dr] of SHAPES[type][rotation]) {
    const row = y + dr;
    const col = x + dc;
    if (row >= 0) state.board[row][col] = COLORS[type];
  }
  clearLines();
  spawnPiece();
}

// ─── Line Clearing ────────────────────────────────────────────────────────────
function clearLines() {
  let cleared = 0;
  const newBoard = state.board.filter(row => {
    if (row.every(cell => cell !== 0)) { cleared++; return false; }
    return true;
  });
  while (newBoard.length < ROWS) newBoard.unshift(Array(COLS).fill(0));
  state.board = newBoard;

  if (cleared > 0) {
    state.score += POINTS[cleared] * state.level;
    state.lines += cleared;
    state.level = Math.min(20, Math.floor(state.lines / 10) + 1);
    state.dropInterval = Math.max(50, 800 - (state.level - 1) * 70);
    updateHUD();
  }
}

// ─── Rotation (SRS) ───────────────────────────────────────────────────────────
function rotatePiece(dir) {
  const { type, rotation, x, y } = state.current;
  const newRot  = (rotation + 4 + dir) % 4;
  const key     = `${rotation}->${newRot}`;
  const kicks   = type === 'I' ? WALL_KICKS_I[key] : WALL_KICKS[key];

  for (const [dx, dy] of kicks) {
    if (isValidPosition(type, newRot, x + dx, y + dy)) {
      state.current = { ...state.current, rotation: newRot, x: x + dx, y: y + dy };
      // If still on floor after rotation, reset lock timer
      if (isOnFloor(type, newRot, x + dx, y + dy)) {
        resetLockTimer();
      } else {
        clearLockTimer();
      }
      return true;
    }
  }
  return false;
}

// ─── Ghost Piece ──────────────────────────────────────────────────────────────
function getGhostY() {
  const { type, rotation, x, y } = state.current;
  let ghostY = y;
  while (isValidPosition(type, rotation, x, ghostY + 1)) ghostY++;
  return ghostY;
}

// ─── Hard Drop ────────────────────────────────────────────────────────────────
function hardDrop() {
  const ghostY = getGhostY();
  state.score += (ghostY - state.current.y) * 2;
  state.current.y = ghostY;
  updateHUD();
  lockPiece();
}

// ─── Lock Delay Timer ─────────────────────────────────────────────────────────
function resetLockTimer() {
  clearLockTimer();
  state.lockTimer = setTimeout(() => {
    if (state.current && isOnFloor(
        state.current.type, state.current.rotation,
        state.current.x, state.current.y)) {
      lockPiece();
    }
  }, state.lockDelay);
}

function clearLockTimer() {
  if (state.lockTimer !== null) {
    clearTimeout(state.lockTimer);
    state.lockTimer = null;
  }
}

// ─── Game Loop ────────────────────────────────────────────────────────────────
function gameLoop(timestamp) {
  if (state.gameOver || state.paused) return;

  if (timestamp - state.lastDrop >= state.dropInterval) {
    state.lastDrop = timestamp;
    const { type, rotation, x, y } = state.current;
    if (isValidPosition(type, rotation, x, y + 1)) {
      state.current.y++;
      // Track lowest y for lock delay reset logic
      if (state.current.y > state.lowestY) {
        state.lowestY = state.current.y;
        clearLockTimer();  // moved to new row — cancel any pending lock
      }
      // Check if now on floor
      if (isOnFloor(type, rotation, x, state.current.y)) {
        resetLockTimer();
      }
    } else if (state.lockTimer === null) {
      // Gravity tick landed on something; start lock timer if not already started
      resetLockTimer();
    }
  }

  render();
  state.animId = requestAnimationFrame(gameLoop);
}

// ─── Rendering ────────────────────────────────────────────────────────────────
function render() {
  drawBoard();
  drawGhost();
  drawCurrentPiece();
  drawPreview();
}

function drawCell(ctx, col, row, color, size) {
  const x = col * size;
  const y = row * size;
  const pad = 1;

  ctx.fillStyle = color;
  ctx.fillRect(x + pad, y + pad, size - pad * 2, size - pad * 2);

  // Top-left highlight
  ctx.fillStyle = 'rgba(255,255,255,0.22)';
  ctx.fillRect(x + pad, y + pad, size - pad * 2, 3);
  ctx.fillRect(x + pad, y + pad, 3, size - pad * 2);

  // Bottom-right shadow
  ctx.fillStyle = 'rgba(0,0,0,0.35)';
  ctx.fillRect(x + pad, y + size - pad - 3, size - pad * 2, 3);
  ctx.fillRect(x + size - pad - 3, y + pad, 3, size - pad * 2);
}

function drawBoard() {
  // Background
  boardCtx.fillStyle = GRID_COLOR;
  boardCtx.fillRect(0, 0, COLS * CELL, ROWS * CELL);

  // Grid lines
  boardCtx.strokeStyle = BORDER_COLOR;
  boardCtx.lineWidth = 0.5;
  for (let r = 0; r < ROWS; r++) {
    for (let c = 0; c < COLS; c++) {
      boardCtx.strokeRect(c * CELL, r * CELL, CELL, CELL);
      if (state.board[r][c] !== 0) {
        drawCell(boardCtx, c, r, state.board[r][c], CELL);
      }
    }
  }
}

function drawGhost() {
  const { type, rotation, x } = state.current;
  const ghostY = getGhostY();
  boardCtx.fillStyle = 'rgba(255,255,255,0.07)';
  boardCtx.strokeStyle = 'rgba(255,255,255,0.18)';
  boardCtx.lineWidth = 1;
  for (const [dc, dr] of SHAPES[type][rotation]) {
    const row = ghostY + dr;
    const col = x + dc;
    if (row >= 0) {
      boardCtx.fillRect(col * CELL + 1, row * CELL + 1, CELL - 2, CELL - 2);
      boardCtx.strokeRect(col * CELL + 1.5, row * CELL + 1.5, CELL - 3, CELL - 3);
    }
  }
}

function drawCurrentPiece() {
  const { type, rotation, x, y } = state.current;
  for (const [dc, dr] of SHAPES[type][rotation]) {
    const row = y + dr;
    const col = x + dc;
    if (row >= 0) drawCell(boardCtx, col, row, COLORS[type], CELL);
  }
}

function drawPreview() {
  previewCtx.fillStyle = '#111';
  previewCtx.fillRect(0, 0, 100, 100);

  const { type } = state.next;
  const cells = SHAPES[type][0];

  // Center the piece in the 4×4 preview area
  const minC = Math.min(...cells.map(([c]) => c));
  const maxC = Math.max(...cells.map(([c]) => c));
  const minR = Math.min(...cells.map(([,r]) => r));
  const maxR = Math.max(...cells.map(([,r]) => r));
  const pieceW = maxC - minC + 1;
  const pieceH = maxR - minR + 1;
  const offsetX = Math.floor((4 - pieceW) / 2) - minC;
  const offsetY = Math.floor((4 - pieceH) / 2) - minR;

  for (const [dc, dr] of cells) {
    drawCell(previewCtx, dc + offsetX, dr + offsetY, COLORS[type], PREVIEW_CELL);
  }
}

// ─── HUD & Overlays ───────────────────────────────────────────────────────────
function updateHUD() {
  document.getElementById('score').textContent = state.score.toLocaleString();
  document.getElementById('level').textContent = state.level;
  document.getElementById('lines').textContent = state.lines;
}

function triggerGameOver() {
  state.gameOver = true;
  cancelAnimationFrame(state.animId);
  clearLockTimer();
  render(); // final frame
  const overlay = document.getElementById('overlay');
  document.getElementById('overlay-title').textContent = 'GAME OVER';
  document.getElementById('overlay-subtitle').textContent =
    `Score: ${state.score.toLocaleString()}\nPress R or Enter to restart`;
  overlay.classList.remove('hidden');
}

function showPauseOverlay() {
  const overlay = document.getElementById('overlay');
  document.getElementById('overlay-title').textContent = 'PAUSED';
  document.getElementById('overlay-subtitle').textContent = 'Press P to resume';
  overlay.classList.remove('hidden');
}

function hideOverlay() {
  document.getElementById('overlay').classList.add('hidden');
}

function togglePause() {
  if (state.gameOver) return;
  state.paused = !state.paused;
  if (state.paused) {
    cancelAnimationFrame(state.animId);
    clearLockTimer();
    showPauseOverlay();
  } else {
    hideOverlay();
    state.lastDrop = performance.now();
    // Restart lock timer if piece is on floor
    if (isOnFloor(state.current.type, state.current.rotation,
                  state.current.x, state.current.y)) {
      resetLockTimer();
    }
    state.animId = requestAnimationFrame(gameLoop);
  }
}

function restartGame() {
  if (state) {
    cancelAnimationFrame(state.animId);
    clearLockTimer();
  }
  hideOverlay();
  state = createState();
  updateHUD();
  spawnPiece();
  state.lastDrop = performance.now();
  state.animId = requestAnimationFrame(gameLoop);
}

// ─── Input Handling ───────────────────────────────────────────────────────────
const DAS_DELAY    = 170;
const DAS_INTERVAL = 50;
let dasTimer = null;
let dasDir   = 0;

function moveLR(dir) {
  const { type, rotation, x, y } = state.current;
  if (isValidPosition(type, rotation, x + dir, y)) {
    state.current.x += dir;
    // If on floor, reset lock timer (player extended their time)
    if (isOnFloor(type, rotation, state.current.x, y)) {
      resetLockTimer();
    }
    render();
  }
}

function startDAS(dir) {
  stopDAS();
  dasDir = dir;
  moveLR(dir);
  dasTimer = setTimeout(() => {
    dasTimer = setInterval(() => moveLR(dasDir), DAS_INTERVAL);
  }, DAS_DELAY);
}

function stopDAS() {
  clearTimeout(dasTimer);
  clearInterval(dasTimer);
  dasTimer = null;
}

document.addEventListener('keydown', (e) => {
  if (state.gameOver) {
    if (e.key === 'Enter' || e.key === 'r' || e.key === 'R') restartGame();
    return;
  }

  switch (e.key) {
    case 'ArrowLeft':
      e.preventDefault();
      startDAS(-1);
      break;

    case 'ArrowRight':
      e.preventDefault();
      startDAS(1);
      break;

    case 'ArrowDown':
      e.preventDefault();
      if (!state.paused) {
        const { type, rotation, x, y } = state.current;
        if (isValidPosition(type, rotation, x, y + 1)) {
          state.current.y++;
          state.score++;
          state.lastDrop = performance.now();
          if (isOnFloor(type, rotation, x, state.current.y)) {
            resetLockTimer();
          } else {
            clearLockTimer();
          }
          render();
          updateHUD();
        }
      }
      break;

    case 'ArrowUp':
      e.preventDefault();
      if (!state.paused) {
        rotatePiece(1);
        render();
      }
      break;

    case 'z':
    case 'Z':
      if (!state.paused) {
        rotatePiece(-1);
        render();
      }
      break;

    case ' ':
      e.preventDefault();
      if (!state.paused) hardDrop();
      break;

    case 'p':
    case 'P':
      togglePause();
      break;
  }
});

document.addEventListener('keyup', (e) => {
  if (e.key === 'ArrowLeft' || e.key === 'ArrowRight') stopDAS();
});

// ─── Start ────────────────────────────────────────────────────────────────────
restartGame();
