const fs = require("fs");
const path = require("path");

const TileSetAttributes = {
    solid: 1
}

function writeUint32(value, buffer, offset, littleEndian) {
    if (littleEndian)
        return buffer.writeUInt32LE(value, offset);
    else
        return buffer.writeUInt32BE(value, offset);
}


function prepareTileSet(srcPath, destPath, littleEndian) {
    const sourceFile = fs.readFileSync(srcPath, "utf8");
    const tileSet = JSON.parse(sourceFile);

    const spriteLength = Buffer.byteLength(tileSet.sprite, "utf8");
    const bufferSize = 4 + spriteLength + 8 + 4 + (tileSet.tiles.length * 8);
    const buffer = Buffer.alloc(bufferSize);

    let offset = writeUint32(spriteLength, buffer, 0, littleEndian);
    offset += buffer.write(tileSet.sprite, offset, spriteLength, "utf8");

    offset = writeUint32(tileSet.horizontalCount, buffer, offset, littleEndian);
    offset = writeUint32(tileSet.verticalCount, buffer, offset, littleEndian);

    offset = writeUint32(tileSet.tiles.length, buffer, offset, littleEndian);
    for (const tile of tileSet.tiles) {
        offset = writeUint32(tile.index, buffer, offset, littleEndian);

        let bitmask = 0;

        for (const attribute of tile.attributes) {
            switch(attribute) {
                case "solid":
                    bitmask |= TileSetAttributes.solid;
                break;
            }
            
        }

        offset = writeUint32(bitmask, buffer, offset, littleEndian);
    }
    
    fs.writeFileSync(destPath, buffer);
}

function readLevelTiles(srcPath) {
    const basename = path.basename(srcPath, ".level.json");
    const dirname = path.dirname(srcPath);
    const tilesFile = path.join(dirname, `${basename}.level.tiles`);
    return fs.readFileSync(tilesFile, "utf8")
}

function prepareLevel(srcPath, destPath, littleEndian) {
    const sourceFile = fs.readFileSync(srcPath, "utf8");
    const level = JSON.parse(sourceFile);
    const levelTiles = readLevelTiles(srcPath);

    const nameLength = Buffer.byteLength(level.name, "utf8");
    const tileSetLength = Buffer.byteLength(level.tileSet, "utf8");
    const bufferSize = 8 + nameLength + tileSetLength + 8 + (level.width * level.height);

    const buffer = Buffer.alloc(bufferSize);
    let offset = writeUint32(nameLength, buffer, 0, littleEndian);
    offset += buffer.write(level.name, offset, nameLength, "utf8");
    offset = writeUint32(tileSetLength, buffer, offset, littleEndian);
    offset += buffer.write(level.tileSet, offset, tileSetLength, "utf8");
    offset = writeUint32(level.width, buffer, offset, littleEndian);
    offset = writeUint32(level.height, buffer, offset, littleEndian);

    for (const char of levelTiles) {
        if (char === ' ')
            offset = buffer.writeUInt8(255, offset);
        else if (char >= '0' && char <= '9')
            offset = buffer.writeUInt8(parseInt(char), offset);
    }

    fs.writeFileSync(destPath, buffer);
}

function prepareAssets(sourceDir, destDir, params) {
    let options = {
        littleEndian: true,
        spriteFunc: (sourceFile, destDir, assetName) => {
            const buildPath = path.join(destDir, assetName);
            fs.copyFileSync(sourceFile, buildPath);
        },
        levelFunc: (sourceFile, destDir, assetName) =>  {
            const baseName = path.basename(assetName, ".level.json");
            const buildPath = path.join(destDir, `${baseName}.level`);
            prepareLevel(sourceFile, buildPath, options.littleEndian);
        },
        tilesetFunc: (sourceFile, destDir, assetName) =>  {
            const baseName = path.basename(assetName, ".tileset.json");
            const buildPath = path.join(destDir, `${baseName}.tileset`);
            prepareTileSet(sourceFile, buildPath, options.littleEndian);
        }
    };

    if (params)
        Object.assign(options, params);

    const assets = fs.readdirSync(sourceDir);

    for (const asset of assets) {
        const sourceFile = path.join(sourceDir, asset);

        if (asset.endsWith(".tileset.json")) 
            options.tilesetFunc(sourceFile, destDir, asset);
        else if (asset.endsWith(".level.json")) 
            options.levelFunc(sourceFile, destDir, asset);
        else if (asset.endsWith(".png")) {
            options.spriteFunc(sourceFile, destDir, asset);
        }
    }
}

module.exports = prepareAssets;