const {prepareLevel} = require("./prepare_assets");

const path = require("path");

const levelPath = process.argv[2];
const baseName = path.basename(levelPath, ".level.json");
const assetDir = path.dirname(levelPath);
const buildPath = path.join("assets", "build_sdl", assetDir.substring(7), `${baseName}.level`);

prepareLevel(levelPath, buildPath, {littleEndian: true, destDir: "build_sdl"});