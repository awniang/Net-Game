// demo.js

Module.onRuntimeInitialized = () => { start(); }

let game=null;
const canvas = document.getElementById("game-canvas");
const ctx = canvas.getContext("2d");
const images = {};
const shapeNames = ["empty.jpg", "endpoint.png", "segment.jpg", "corner.png", "tee.jpg", "cross.jpg"];

/* 
function loadImages() {
    for (let name of shapeNames) {
        const img = new Image();
        img.src = "images/" + name;
        images[name] = img;
    }
} */
/*
function loadImages(remind) {
    let count = 0;
    for (let name of shapeNames) {
        const img = new Image();
        img.src = "images/" + name;
        img.onload = function () {
            count++;
            images[name] = img;

            if (count === total) {
                remind(); 
            }
        };
    }
}
function loadImages() {
    let count = 0;
    for (let name of shapeNames) {
        const img = new Image();
        img.src = "images/" + name;

        img.onload =  () => {
            images[name] = img;
            count++;

            if (count === shapeNames.length) {
                console.log("Toutes les images sont chargées");
            }
        };
    }
}*/
function loadImages(callback) {
    let count = 0;
    for (let name of shapeNames) {
        const img = new Image();
        img.src = "images/" + name;

        img.onload = function () {
            count++;
            images[name] = img;

            if (count === shapeNames.length) {
                callback(); // Appelle le callback (qui est drawGame) une fois que toutes les images sont chargées
            }
        };
    }
}

function start() {
    game = Module._new_default();
    const rows = Module._nb_rows(game);
    const cols = Module._nb_cols(game);
    canvas.width = 600;
    canvas.height = 600;
    const cellSize = Math.min(canvas.width / cols, canvas.height / rows);
    drawGame(cellSize);
    loadImages();
    CanvasEvents(cellSize);
}

function CanvasEvents(cellSize){
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
            drawGame(cellSize);
        }
    });

}

function drawGame(cellSize) {
    const rows = Module._nb_rows(game);
    const cols = Module._nb_cols(game);

    /* canvas.width = cols * cellSize;
    canvas.height = rows * cellSize; */
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
}

function undo() {
    Module._undo(game);
    drawGame(cellSize);

}

function redo() {
    Module._redo(game);
    drawGame(cellSize);
   
}

function restart() {
    Module._restart(game);
    drawGame(cellSize);
}

function solve() {
    Module._solve(game);
    drawGame(cellSize);
}
// Ajout des écouteurs d'événements pour les boutons
document.getElementById("undo-button").addEventListener("click", undo);
document.getElementById("redo-button").addEventListener("click", redo);
document.getElementById("restart-button").addEventListener("click", restart);
document.getElementById("solve-button").addEventListener("click", solve);
