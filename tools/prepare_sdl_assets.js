const fs = require("fs");
const path = require("path");
const rimraf = require("rimraf");

const {prepareAssets} = require("./prepare_assets");

const assetsFolder = path.resolve(__dirname, "..", "assets");
const buildFolder = path.join(assetsFolder, "build_sdl");
if (fs.existsSync(buildFolder)) {
    rimraf.sync(buildFolder);
}

fs.mkdirSync(buildFolder);


prepareAssets(assetsFolder, buildFolder, null);