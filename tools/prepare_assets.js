const fs = require("fs");
const path = require("path");

// Note: This should match up with the TileType enum in level.h
const TileTypeValues = {
    none: 0,
    solid: 1,
    triangle: 2,
    tunnel: 3,
    brick: 4
}

const musicFormats = new Set([".mod", ".xm"]);

function writeUint32(value, buffer, offset, littleEndian) {
    if (littleEndian)
        return buffer.writeUInt32LE(value, offset);
    else
        return buffer.writeUInt32BE(value, offset);
}

function writeFloat(value, buffer, offset, littleEndian) {
    if (littleEndian)
        return buffer.writeFloatLE(value, offset);
    else
        return buffer.writeFloatBE(value, offset);
}

function prepareTileSet(srcPath, destPath, littleEndian) {
    const sourceFile = fs.readFileSync(srcPath, "utf8");
    const tileSet = JSON.parse(sourceFile);

    const spriteLength = Buffer.byteLength(tileSet.sprite, "utf8");
    const bufferSize = 4 + spriteLength + 4 + (tileSet.tiles.length * 8);
    const buffer = Buffer.alloc(bufferSize);

    let offset = writeUint32(spriteLength, buffer, 0, littleEndian);
    offset += buffer.write(tileSet.sprite, offset, spriteLength, "utf8");

    offset = writeUint32(tileSet.tiles.length, buffer, offset, littleEndian);
    
    for (let i = 0; i < tileSet.tiles.length; i++) {
        const tile = tileSet.tiles[i];
        offset = writeUint32(i, buffer, offset, littleEndian);

        if (TileTypeValues.hasOwnProperty(tile))
            offset = writeUint32(TileTypeValues[tile], buffer, offset, littleEndian);
        else
            throw new Error(`Unexpected attribute for tile ${i}: ${tile}`)
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
    const musicLength = Buffer.byteLength(level.music, "utf8");
    const bufferSize = 12 + nameLength + tileSetLength + musicLength + 8 + (level.width * level.height) + 4 /*level.goal*/;

    const buffer = Buffer.alloc(bufferSize);
    let offset = writeUint32(nameLength, buffer, 0, littleEndian);
    offset += buffer.write(level.name, offset, nameLength, "utf8");
    offset = writeUint32(tileSetLength, buffer, offset, littleEndian);
    offset += buffer.write(level.tileSet, offset, tileSetLength, "utf8");
    offset = writeUint32(musicLength, buffer, offset, littleEndian);
    offset += buffer.write(level.music, offset, tileSetLength, "utf8");
    offset = writeUint32(level.width, buffer, offset, littleEndian);
    offset = writeUint32(level.height, buffer, offset, littleEndian);
    offset = writeFloat(level.goal, buffer, offset, littleEndian);

    for (let i = 0; i < levelTiles.length; i++){
        const char = levelTiles.charCodeAt(i);
        if (char === 32)
            offset = buffer.writeUInt8(255, offset);
        else if (char >= 65 && char <= 90)
            offset = buffer.writeUInt8(parseInt(char - 65), offset);
    }

    fs.writeFileSync(destPath, buffer);
}

function prepareSprite(srcPath, destPath, littleEndian) {
    const baseName = path.basename(srcPath, ".png");
    const spriteJsonFile = path.join(path.dirname(srcPath), `${baseName}.sprite.json`);
    const sprite = JSON.parse(fs.readFileSync(spriteJsonFile, "utf8"));

    const buffer = Buffer.alloc(8);
    writeUint32(parseInt(sprite.horizonalFrames), buffer, 0, littleEndian);
    writeUint32(parseInt(sprite.verticalFrames), buffer, 4, littleEndian);

    fs.writeFileSync(path.join(path.dirname(destPath), `${baseName}.sprite`), buffer);

    fs.copyFileSync(srcPath, destPath);
}

function prepareAnimation(srcPath, destPath){
    const animations = JSON.parse(fs.readFileSync(srcPath, "utf8"));

    let bufferSize = 1;
    for (const animation of animations) {
        bufferSize += 1 + animation.frames.length;
    }

    const buffer = Buffer.alloc(bufferSize);

    let offset = buffer.writeUInt8(animations.length, 0);
    for (const animation of animations) {
        offset = buffer.writeUInt8(animation.frames.length, offset); // write frame count

        for (const frame of animation.frames) {
            offset = buffer.writeUInt8(frame, offset); // write frames
        }
    }

    fs.writeFileSync(destPath, buffer);
}

function prepareAssets(sourceDir, destDir, params) {
    let options = {
        littleEndian: true,
        spriteFunc: (sourceFile, destDir, assetName) => {
            const buildPath = path.join(destDir, assetName);
            prepareSprite(sourceFile, buildPath, options.littleEndian);
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
        },
        animationFunc: (sourceFile, destDir, assetName) => {
            const baseName = path.basename(assetName, ".animation.json");
            const buildPath = path.join(destDir, `${baseName}.animation`);
            prepareAnimation(sourceFile, buildPath);
        },
        musicFunc: (sourceFile, destDir, assetName) => {
            const buildPath = path.join(destDir, assetName);
            fs.copyFileSync(sourceFile, buildPath);
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
        else if (asset.endsWith(".animation.json")) 
            options.animationFunc(sourceFile, destDir, asset);
        else if (asset.endsWith(".png"))
            options.spriteFunc(sourceFile, destDir, asset);
        else if (musicFormats.has(path.extname(asset)))
            options.musicFunc(sourceFile, destDir, asset);
    }
}

module.exports = prepareAssets;