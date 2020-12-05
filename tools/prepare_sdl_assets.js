const fs = require("fs");
const path = require("path");
const rimraf = require("rimraf");

const {prepareAssets} = require("./prepare_assets");

function prepareSdlAssets(buildFolder) {
    const assetsFolder = path.resolve(__dirname, "..", "assets");

    if (fs.existsSync(buildFolder)) {
        rimraf.sync(buildFolder);
    }

    fs.mkdirSync(buildFolder);

    const params = {
        musicFunc(sourceFile, destDir, assetName) {
            if (!assetName.endsWith(".ogg"))
                return;

            const buildPath = path.join(destDir, assetName);
            fs.copyFileSync(sourceFile, buildPath);
        }
    }

    prepareAssets(assetsFolder, buildFolder, params);
}

module.exports = {
    prepareSdlAssets: prepareSdlAssets
};

if (require.main === module) {
    const buildFolder = path.join(__dirname, "..", "assets", "build_sdl");
    prepareSdlAssets(buildFolder);
}