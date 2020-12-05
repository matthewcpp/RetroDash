const fs = require("fs");
const path = require("path");

const {prepareSdlAssets} = require("./prepare_sdl_assets");

const emscriptenBuildDir = path.join(__dirname, "..", "build-emscripten");
if (!fs.existsSync(emscriptenBuildDir)) {
    fs.mkdirSync(emscriptenBuildDir);
}

const emscriptenAssetDir = path.join(emscriptenBuildDir, "build_sdl");
prepareSdlAssets(emscriptenAssetDir);