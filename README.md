# Tetris

A fully playable browser-based Tetris game built with vanilla HTML5 Canvas and JavaScript. No frameworks, no dependencies — open `index.html` in any modern browser and play instantly.

The original Turbo C++ / BGI implementation is preserved in [`legacy/`](./legacy/).

---

## Play

```bash
# Option A: open directly
open index.html

# Option B: local server
python3 -m http.server 8080
# then visit http://localhost:8080
```

---

## Controls

| Key | Action |
|-----|--------|
| `←` `→` | Move left / right |
| `↑` | Rotate clockwise |
| `Z` | Rotate counter-clockwise |
| `↓` | Soft drop (+1 pt/row) |
| `Space` | Hard drop (+2 pts/row) |
| `P` | Pause / resume |
| `R` / `Enter` | Restart (on Game Over) |

---

## File Structure

```
├── index.html      # Shell: canvas elements, HUD, overlay, loads CSS + JS
├── style.css       # Dark theme, layout, CRT glow, overlay toggle
├── game.js         # All game logic, rendering, input handling (~350 lines)
└── legacy/
    ├── TETR.CPP    # Original Turbo C++ source (BGI graphics)
    ├── SCORE10.DAT # Binary high-score file (original)
    └── README.md   # Original project README
```

---

## Implementation

### Board

The board is a 2D array `board[20][10]` — 20 rows, 10 columns — matching the standard Tetris guideline. Each cell stores either `0` (empty) or a CSS color string (the color of the locked piece that filled it).

```js
function createBoard() {
  return Array.from({ length: ROWS }, () => Array(COLS).fill(0));
}
```

Storing color strings instead of integer piece IDs eliminates a separate color lookup at render time.

---

### Piece Representation

All 7 tetrominoes (I, O, T, S, Z, J, L) are defined as flat offset arrays — four `[col, row]` pairs relative to a pivot point, one array per rotation state (0°, 90°, 180°, 270°).

```js
const SHAPES = {
  T: [
    [[1,0],[0,1],[1,1],[2,1]],   // 0° (spawn)
    [[1,0],[1,1],[2,1],[1,2]],   // 90° CW
    [[0,1],[1,1],[2,1],[1,2]],   // 180°
    [[1,0],[0,1],[1,1],[1,2]],   // 270° CCW
  ],
  // ... other pieces
};
```

Using pre-defined offset arrays per rotation avoids matrix math at runtime and makes collision checks trivially fast — just iterate the 4 offsets and test each against the board.

---

### Collision Detection

`isValidPosition(type, rotation, x, y)` iterates the 4 cell offsets of the given piece state and checks:

1. Column is within `[0, COLS)` — left/right walls
2. Row is within `(-∞, ROWS)` — floor (rows above 0 are allowed for spawning)
3. `board[row][col] !== 0` — no overlap with a locked piece

```js
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
```

This is the equivalent of `isDrawable()` in the original C++ source.

---

### SRS Rotation with Wall Kicks

Rotation uses the **Super Rotation System (SRS)** — the standard used in modern Tetris. When a clockwise or counter-clockwise rotation is requested:

1. Compute the new rotation index: `newRot = (rotation ± 1 + 4) % 4`
2. Look up the wall kick test offsets for the `old->new` rotation transition
3. Try each `[dx, dy]` offset in order; use the first one that passes `isValidPosition`
4. If all 5 tests fail, the rotation is blocked (piece does not move)

The I-piece uses a separate kick table since its pivot geometry differs from the other pieces.

```js
function rotatePiece(dir) {
  const { type, rotation, x, y } = state.current;
  const newRot = (rotation + 4 + dir) % 4;
  const key    = `${rotation}->${newRot}`;
  const kicks  = type === 'I' ? WALL_KICKS_I[key] : WALL_KICKS[key];

  for (const [dx, dy] of kicks) {
    if (isValidPosition(type, newRot, x + dx, y + dy)) {
      state.current = { ...state.current, rotation: newRot, x: x + dx, y: y + dy };
      return;
    }
  }
}
```

---

### Line Clearing

`clearLines()` filters out any row where every cell is non-zero, prepends the same number of empty rows at the top, and updates score/level/speed.

```js
function clearLines() {
  let cleared = 0;
  const newBoard = state.board.filter(row => {
    if (row.every(cell => cell !== 0)) { cleared++; return false; }
    return true;
  });
  while (newBoard.length < ROWS) newBoard.unshift(Array(COLS).fill(0));
  state.board = newBoard;
  // ...
}
```

This replaces the original C++ `CollapseFullRow()` which detected full rows by checking `sum == 2 * 13` (13 cells × value 2) and shifted rows down in-place with BGI redraws.

---

### Scoring

Classic Tetris point values scaled by the current level:

| Lines cleared | Points |
|:---:|:---:|
| 1 | 100 × level |
| 2 | 300 × level |
| 3 | 500 × level |
| 4 (Tetris) | 800 × level |

Soft drop adds **+1 pt** per row; hard drop adds **+2 pts** per row dropped.

---

### Progressive Difficulty

Level advances every 10 lines (capped at 20). Drop interval shrinks linearly:

```js
state.level        = Math.min(20, Math.floor(state.lines / 10) + 1);
state.dropInterval = Math.max(50, 800 - (state.level - 1) * 70);  // ms
```

Level 1 drops every 800 ms; level 11 drops every 100 ms; level 12+ is capped at 50 ms.

---

### 7-Bag Randomizer

Piece selection uses the **7-bag** algorithm: shuffle all 7 piece types into a bag, deal them one at a time, then reshuffle when the bag empties. This guarantees every piece appears exactly once per cycle of 7, preventing droughts (e.g. no I-piece for 20+ pieces).

```js
function nextFromBag() {
  if (state.bag.length === 0) {
    state.bag = [...PIECE_TYPES].sort(() => Math.random() - 0.5);
  }
  return state.bag.pop();
}
```

---

### Lock Delay

When a piece lands on the floor or a locked piece, it does not lock instantly. A **500 ms lock delay** timer starts. Moving or rotating the piece while it's on the floor resets the timer, giving the player time to slide or rotate before locking. If the piece drops to a new lower row under gravity, the timer clears (no lock yet).

```js
function resetLockTimer() {
  clearLockTimer();
  state.lockTimer = setTimeout(() => {
    if (isOnFloor(...)) lockPiece();
  }, state.lockDelay);
}
```

---

### Ghost Piece

The ghost piece shows where the current piece will land. It's computed by scanning downward from the current position until the next step would be invalid:

```js
function getGhostY() {
  const { type, rotation, x, y } = state.current;
  let ghostY = y;
  while (isValidPosition(type, rotation, x, ghostY + 1)) ghostY++;
  return ghostY;
}
```

Rendered as a semi-transparent outline before drawing the active piece.

---

### DAS (Delayed Auto Shift)

Holding left or right doesn't move the piece every frame — it uses **DAS** to feel like a real Tetris game:

- First press: immediate move
- After **170 ms** hold: repeat every **50 ms**

```js
function startDAS(dir) {
  stopDAS();
  moveLR(dir);  // immediate
  dasTimer = setTimeout(() => {
    dasTimer = setInterval(() => moveLR(dasDir), DAS_INTERVAL);
  }, DAS_DELAY);
}
```

---

### Rendering

Everything is drawn on an HTML5 Canvas (`300 × 600 px`, 30 px/cell). Each locked or active cell gets a beveled 3D look:

```js
function drawCell(ctx, col, row, color, size) {
  ctx.fillStyle = color;
  ctx.fillRect(col * size + 1, row * size + 1, size - 2, size - 2);

  // top-left highlight
  ctx.fillStyle = 'rgba(255,255,255,0.22)';
  ctx.fillRect(col * size + 1, row * size + 1, size - 2, 3);
  ctx.fillRect(col * size + 1, row * size + 1, 3, size - 2);

  // bottom-right shadow
  ctx.fillStyle = 'rgba(0,0,0,0.35)';
  ctx.fillRect(col * size + 1, row * size + size - 4, size - 2, 3);
  ctx.fillRect(col * size + size - 4, row * size + 1, 3, size - 2);
}
```

Score / Level / Lines are plain DOM `<span>` elements updated via `textContent` — cheaper than redrawing them on canvas every frame.

The next-piece preview is a separate 100 × 100 canvas. The piece is centered in its bounding box before drawing.

---

### Game Loop

The main loop uses `requestAnimationFrame` with timestamp-based gravity — no `setInterval`, so it never drifts and pauses cleanly:

```js
function gameLoop(timestamp) {
  if (state.gameOver || state.paused) return;

  if (timestamp - state.lastDrop >= state.dropInterval) {
    state.lastDrop = timestamp;
    if (isValidPosition(type, rotation, x, y + 1)) {
      state.current.y++;
    } else if (state.lockTimer === null) {
      resetLockTimer();
    }
  }

  render();
  state.animId = requestAnimationFrame(gameLoop);
}
```

---

### Game Over Detection

After every piece locks, a new piece spawns at `(x=3, y=-1)`. If `isValidPosition` fails immediately at the spawn position, the board is full and the game ends.

---

## Piece Colors

| Piece | Color |
|:-----:|-------|
| I | Cyan `#00cfcf` |
| O | Yellow `#f5c800` |
| T | Purple `#a020f0` |
| S | Green `#22cc44` |
| Z | Red `#ee3333` |
| J | Blue `#2255ee` |
| L | Orange `#ff8800` |

---

## Legacy

The `legacy/` folder contains the original implementation in Turbo C++ using the BGI (Borland Graphics Interface) library. It used a 35×13 board, DOS-era keyboard polling via `kbhit()`/`getch()`, and binary file I/O for a top-10 high score list. It is preserved as-is for reference.
