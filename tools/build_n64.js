const childProcess = require("child_process");
const path = require("path");
const fs = require("fs");

const mksprite = process.argv[2];

if (process.argv.length < 3) {
    console.log("Usage: node build_n64.js /path/to/libdragon/tools/mksprite")
    process.exit(1);
}

const sprites = [
    {
        input: "player.png",
        bitDepth: 16,
        horizontalSlices: 4,
        verticalSlices: 1
    },
    {
        input: "tilemap.png",
        bitDepth: 16,
        horizontalSlices: 2,
        verticalSlices: 2
    }
];

const fileSystemFolder = path.resolve(__dirname, "../filesystem");
if (!fs.existsSync(fileSystemFolder)) {
    fs.mkdirSync(fileSystemFolder);
}

for (const sprite of sprites) {
    const inputPath = path.resolve(__dirname, "../assets", sprite.input);
    const fileName = path.basename(sprite.input, ".png");
    const outputPath = path.join(fileSystemFolder, `${fileName}.sprite`);
    
    console.log(`Process: ${inputPath} -> ${outputPath}`);
    
    childProcess.spawnSync(mksprite, [sprite.bitDepth, sprite.horizontalSlices, sprite.verticalSlices, inputPath, outputPath], {stdio: 'pipe'});
}


