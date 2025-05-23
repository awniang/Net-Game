const canvas = document.getElementById("game-canvas");
const ctx = canvas.getContext("2d");
let game;
const images = {};

const shapeNames = ["empty.jpg", "endpoint.png", "segment.jpg", "corner.png", "tee.jpg", "cross.jpg"];

function loadImages() {
    let loadedCount = 0;
    const totalImages = shapeNames.length;

    for (let name of shapeNames) {
        const img = new Image();
        img.src = "images/" + name;
        img.onload = () => {
            images[name] = img;
            loadedCount++;
            if (loadedCount === totalImages) {
                drawGame();
            }
        };
        img.onerror = () => {
            console.error(`Failed to load image: ${name}`);
            loadedCount++;
            if (loadedCount === totalImages) {
                drawGame();
            }
        };
    }
}

function calculateCellSize(rows, cols) {
    const canvasWidth = canvas.offsetWidth;
    const canvasHeight = canvas.offsetHeight;
    const cellSize = Math.min(canvasWidth / cols, canvasHeight / rows);
    const offsetX = (canvasWidth - cols * cellSize) / 2;
    const offsetY = (canvasHeight - rows * cellSize) / 2;
    return { cellSize, offsetX, offsetY };
}

function drawGame() {
    const rows = Module._nb_rows(game);
    const cols = Module._nb_cols(game);
    const { cellSize, offsetX, offsetY } = calculateCellSize(rows, cols);

    if (canvas.width !== canvas.offsetWidth || canvas.height !== canvas.offsetHeight) {
        canvas.width = canvas.offsetWidth;
        canvas.height = canvas.offsetHeight;
    }
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    for (let row = 0; row < rows; row++) {
        for (let col = 0; col < cols; col++) {
            const shapeNumber = Module._get_piece_shape(game, row, col);
            const rotation = Module._get_piece_orientation(game, row, col);
            const imageName = shapeNames[shapeNumber];
            const img = images[imageName];

            if (img) {
                ctx.save();
                ctx.translate(col * cellSize + cellSize / 2 + offsetX, row * cellSize + cellSize / 2 + offsetY);
                ctx.rotate(rotation * Math.PI / 2);
                ctx.drawImage(img, -cellSize / 2, -cellSize / 2, cellSize, cellSize);
                ctx.restore();
            }
        }
    }

    if (Module._won(game)) {
        const winMessage = "🎉You Win!🎉";
        const messageWidth = ctx.measureText(winMessage).width;
        const margin = 50;
        const maxFontSize = 48;

        let fontSize = maxFontSize;
        if (messageWidth > canvas.width - 2 * margin) {
            fontSize = Math.floor(maxFontSize * (canvas.width - 2 * margin) / messageWidth);
        }

        const centerX = canvas.width / 2;
        const centerY = canvas.height / 2 + 15;

        ctx.font = `${fontSize}px Arial`;
        ctx.fillStyle = "lime";
        ctx.textAlign = "center";
        ctx.fillText(winMessage, centerX, centerY);
    }
}

Module.onRuntimeInitialized = () => {
    console.log("WebAssembly module initialized successfully");
    loadImages();
    game = Module._new_default();
    drawGame();
};

Module.onAbort = () => {
    console.error("WebAssembly module failed to load");
};

canvas.addEventListener("click", (event) => {
    const rect = canvas.getBoundingClientRect();
    const scaleX = canvas.width / rect.width; 
    const scaleY = canvas.height / rect.height;

    const x = (event.clientX - rect.left) * scaleX;
    const y = (event.clientY - rect.top) * scaleY;

    const rows = Module._nb_rows(game);
    const cols = Module._nb_cols(game);
    const { cellSize, offsetX, offsetY } = calculateCellSize(rows, cols);

    const col = Math.floor((x - offsetX) / cellSize);
    const row = Math.floor((y - offsetY) / cellSize);

    console.log(`Click at x: ${x}, y: ${y}, row: ${row}, col: ${col}, rows: ${rows}, cols: ${cols}, cellSize: ${cellSize}, offsetX: ${offsetX}, offsetY: ${offsetY}`);

    if (row >= 0 && row < rows && col >= 0 && col < cols) {
        console.log(`Playing move at row: ${row}, col: ${col}`);
        Module._play_move(game, row, col, 1);
        drawGame();
    } else {
        console.log("Click outside valid grid");
    }
});

function undo() {
    Module._undo(game);
    drawGame();
}

function redo() {
    Module._redo(game);
    drawGame();
}

function restart() {
    Module._restart(game);
    drawGame();
}

function solve() {
    Module._solve(game);
    drawGame();
}

function random() {
    const nb_rows = Math.floor(Math.random() * 9) + 2;
    const nb_cols = Math.floor(Math.random() * 9) + 2;
    const wrapping = Math.random() < 0.5 ? 0 : 1;
    const max_empty = Math.max(nb_rows * nb_cols - 2, 0);
    const nb_empty = Math.floor(Math.random() * (Math.min(max_empty, 10) + 1));
    const nb_extra = Math.floor(Math.random() * 10) + 1;

    game = Module._new_random(nb_rows, nb_cols, wrapping, nb_empty, nb_extra);

    const rows = Module._nb_rows(game);
    const cols = Module._nb_cols(game);
    if (rows <= 0 || cols <= 0) {
        console.warn("Random game creation failed, falling back to default");
        game = Module._new_default();
    }

    Module._shuffle(game);
    drawGame();
}