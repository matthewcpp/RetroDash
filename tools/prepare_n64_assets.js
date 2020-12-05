const childProcess = require("child_process");
const path = require("path");
const fs = require("fs");
const rimraf = require("rimraf");

const {prepareAssets} = require("./prepare_assets");

const mksprite = process.argv[2];

if (process.argv.length < 3) {
    console.log("Usage: node build_n64.js /path/to/libdragon/tools/mksprite")
    process.exit(1);
}

const mkspritePath = process.argv[2];

if (!fs.existsSync(mkspritePath)) {
    console.log(`Invalid path specified for mksprite: ${mkspritePath}`);
    process.exit(1);
}

const assetsFolder = path.resolve(__dirname, "..", "assets");
const buildFolder = path.join(assetsFolder, "build_n64");
if (fs.existsSync(buildFolder)) {
    rimraf.sync(buildFolder);
}

fs.mkdirSync(buildFolder);


prepareAssets(assetsFolder, buildFolder,  {
    littleEndian: false,
    spriteFunc: (srcPath, destDir, assetName) => {
        const baseName = path.basename(assetName, ".png");
        const spriteFile = path.join(path.dirname(srcPath), `${baseName}.sprite.json`);
        const sprite = JSON.parse(fs.readFileSync(spriteFile, "utf8"));

        const buildPath = path.join(destDir, `${baseName}.sprite`);
        
        childProcess.spawnSync(mksprite, [16, sprite.horizonalFrames, sprite.verticalFrames, srcPath, buildPath], {stdio: 'pipe'});
    },
    musicFunc(sourceFile, destDir, assetName) {
        if (assetName.endsWith(".ogg"))
            return;

        const buildPath = path.join(destDir, assetName);
        fs.copyFileSync(sourceFile, buildPath);
    }
});
