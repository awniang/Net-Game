const canvas = document.getElementById("game-canvas");
const ctx = canvas.getContext("2d");
let game;
const images = {};


const shapeNames = ["empty.jpg", "endpoint.png", "segment.jpg", "corner.png", "tee.jpg", "cross.jpg"];


function loadImages() {
    for (let name of shapeNames) {
        const img = new Image();
        img.src = "images/" + name;
        images[name] = img;
    }
}
function start() {
    console.log("call start routine");
    var g = Module._new_default();
    Module._play_move(g, 0, 0, 1);
    printGame(g);
    loadImages();
    drawGame();
    Module._delete(g);
}

function drawGame() {
    const rows = Module._nb_rows(game);
    const cols = Module._nb_cols(game);

    canvas.width = cols * cellSize;
    canvas.height = rows * cellSize;
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    for (let row = 0; row < rows; row++) {
        for (let col = 0; col < cols; col++) {
            const shapeNumber = Module._get_piece_shape(game, row, col);
            const rotation = Module._get_piece_orientation(game, row, col);
            const imageName = shapeNames[shapeNumber];
            const img = images[imageName];

            if (img) {
                ctx.save();
                ctx.translate(col * cellSize + cellSize / 2, row * cellSize + cellSize / 2);
                ctx.rotate(rotation * Math.PI / 2);
                ctx.drawImage(img, -cellSize / 2, -cellSize / 2, cellSize, cellSize);
                ctx.restore();
            }
        }
    }
    loadImages()
}

// Démarrer le jeu
Module.onRuntimeInitialized = () => {start();
  /*  loadImages();
    game = Module._new_default();
    drawGame();*/
};

canvas.addEventListener("click", (event) => {
    const rect = canvas.getBoundingClientRect();
    const x = event.clientX - rect.left;
    const y = event.clientY - rect.top;

    const rows = Module._nb_rows(game);
    const cols = Module._nb_cols(game);

    const col = Math.floor(x / cellSize);
    const row = Math.floor(y / cellSize);
    
    if (row >= 0 && row < rows && col >= 0 && col < cols) {
        Module._play_move(game, row, col, 1);
        drawGame();
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
    if (game) {
        Module._delete(game);
    }

    const nbRows = Math.floor(Math.random() * 1) + 3;  // ex: entre 3 et 7
    const nbCols = Math.floor(Math.random() * 5) + 3;
    const wrapping = Math.random() < 0.5;
    const nbEmpty = Math.floor(Math.random() * (nbRows * nbCols));
    const nbExtra = Math.floor(Math.random() * 5);

    game = Module._new_random(nbRows, nbCols, wrapping, nbEmpty, nbExtra);
    drawGame();
}
