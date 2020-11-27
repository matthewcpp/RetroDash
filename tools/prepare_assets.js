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

const levelList = [];

const musicFormats = new Set([".mod", ".xm"]);

function writeUint16(value, buffer, offset, littleEndian) {
    if (littleEndian)
        return buffer.writeUInt16LE(value, offset);
    else
        return buffer.writeUInt16BE(value, offset);
}

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

function validatePieceWidth(rows) {
    const expectedWidth = rows[0].length;

    for (let i = 1; i < rows.length; i++) {
        if (rows[i].length !== expectedWidth)
            throw new Error(`Expected row: ${i} to have width: ${expectedWidth}`);
    }
}

function processPieces(level, levelDir) {
    const levelInfo = {
        width: 0,
        height: -1,
        pieces: []
    };

    for (const piece of level.pieces) {
        const pieceFile = path.join(levelDir, piece);
        const levelTiles = fs.readFileSync(pieceFile, "utf8")

        const rows = levelTiles.split(/\r?\n/);
        validatePieceWidth(rows);
        levelInfo.width += rows[0].length

        if (levelInfo.height < 0)
            levelInfo.height = rows.length
        else if (rows.length !== levelInfo.height)
            throw new Error(`Expected Piece: ${piece} to have height: ${level.height}`);

        levelInfo.pieces.push(rows);
    }

    return levelInfo;
}

function stitchLevelPieces(level, srcPath) {
    const levelDir = path.dirname(srcPath);
    const levelInfo = processPieces(level, levelDir);
    const buffer = Buffer.alloc(levelInfo.width * levelInfo.height);

    let processedWidth = 0;

    for (const rows of levelInfo.pieces) {
        for (let r = 0; r < rows.length; r ++) {
            const row = rows[r];

            for (let col = 0; col < row.length; col++) {
                const index = (levelInfo.width * r)  + processedWidth + col
                const char = row.charCodeAt(col);

                if (char === 32)
                    buffer.writeUInt8(255, index);
                else if (char >= 65 && char <= 90)
                    buffer.writeUInt8(parseInt(char - 65), index);
            }
        }

        processedWidth += rows[0].length;
    }

    return {width: levelInfo.width, height: levelInfo.height, tiles: buffer};
}

function prepareLevel(srcPath, destPath, options) {
    const sourceFile = fs.readFileSync(srcPath, "utf8");
    const level = JSON.parse(sourceFile);
    const tileData = stitchLevelPieces(level, srcPath);
    level.name = level.name.toUpperCase(); // right now fonts only support UPPER CASE

    const nameLength = Buffer.byteLength(level.name, "utf8");
    const tileSetLength = Buffer.byteLength(level.tileSet, "utf8");
    const musicLength = Buffer.byteLength(level.music, "utf8");
    const bufferSize = 12 + nameLength + tileSetLength + musicLength + 8 /*level dimensions */ + 12 /*level start & goal*/;

    const buffer = Buffer.alloc(bufferSize);
    let offset = writeUint32(nameLength, buffer, 0, options.littleEndian);
    offset += buffer.write(level.name, offset, nameLength, "utf8");
    offset = writeUint32(tileSetLength, buffer, offset, options.littleEndian);
    offset += buffer.write(level.tileSet, offset, options.tileSetLength, "utf8");
    offset = writeUint32(musicLength, buffer, offset, options.littleEndian);
    offset += buffer.write(level.music, offset, musicLength, "utf8");
    offset = writeUint32(tileData.width, buffer, offset, options.littleEndian);
    offset = writeUint32(tileData.height, buffer, offset, options.littleEndian);
    offset = writeFloat(level.startPos.x, buffer, offset, options.littleEndian);
    offset = writeFloat(level.startPos.y, buffer, offset, options.littleEndian);
    writeFloat(level.goal, buffer, offset, options.littleEndian);

    fs.writeFileSync(destPath, Buffer.concat([buffer, tileData.tiles]));

    levelList.push({
        name: level.name.toUpperCase(),
        path: destPath.substring(options.destDir.length),
        music: level.music,
        order: level.order
    })
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

function prepareFont(srcPath, destPath, littleEndian) {
    const fontInfo = JSON.parse(fs.readFileSync(srcPath, "utf8"));

    // font size + glyph count + 4 bytes for each char bounding info and 4 bytes for texture pos
    let bufferSize = 8 + (fontInfo.glyphs.length * 8);

    const buffer = Buffer.alloc(bufferSize);

    let offset = writeUint32(fontInfo.size, buffer, 0, littleEndian);
    offset = writeUint32(fontInfo.glyphs.length, buffer, offset, littleEndian);

    for (const char of fontInfo.glyphs) {
        offset = buffer.writeInt8(char.top, offset);
        offset = buffer.writeInt8(char.bottom, offset);
        offset = buffer.writeInt8(char.left, offset);
        offset = buffer.writeInt8(char.width, offset);

        offset = writeUint16(char.x, buffer, offset, littleEndian);
        offset = writeUint16(char.y, buffer, offset, littleEndian);
    }

    fs.writeFileSync(destPath, buffer);
}

function prepareLevelList(destDir, littleEndian) {
    const buildPath = path.join(destDir, "level_list");
    levelList.sort((a,b) => {return a.order - b.order});

    let dataIndices = [];

    // determine the payload size and the index of all the strings within the large payload
    let bufferPayloadSize = 0;
    for (const level of levelList) {
        const levelDataIndices = {};

        levelDataIndices.name = bufferPayloadSize;
        bufferPayloadSize += Buffer.byteLength(level.name, "utf8") + 1;

        levelDataIndices.path = bufferPayloadSize;
        bufferPayloadSize += Buffer.byteLength(level.path, "utf8") + 1;

        levelDataIndices.music = bufferPayloadSize;
        bufferPayloadSize += Buffer.byteLength(level.music, "utf8") + 1;

        dataIndices.push(levelDataIndices);
    }

    let bufferIndicesSize = 4 + levelList.length * 12; // level count + 3 payload indicies per level info

    const buffer = Buffer.alloc(4 /* payload size prefix*/ + bufferPayloadSize + bufferIndicesSize);

    // write all strings into the payload
    let offset = writeUint32(bufferPayloadSize, buffer, 0, littleEndian);
    for (const level of levelList) {
        offset += buffer.write(level.name, offset);
        offset = buffer.writeUInt8(0, offset);

        offset += buffer.write(level.path, offset);
        offset = buffer.writeUInt8(0, offset);

        offset += buffer.write(level.music, offset);
        offset = buffer.writeUInt8(0, offset);
    }

    // write all indices into the payload
    offset = writeUint32(dataIndices.length, buffer, offset, littleEndian);
    for (const levelDataIndices of dataIndices) {
        offset = writeUint32(levelDataIndices.name, buffer, offset, littleEndian);
        offset = writeUint32(levelDataIndices.path, buffer, offset, littleEndian);
        offset = writeUint32(levelDataIndices.music, buffer, offset, littleEndian);
    }

    fs.writeFileSync(buildPath, buffer);
}

const ignore = new Set(["build_sdl", "build_n64"]);

function processAssetDirectory(sourceDir, destDir, options) {
    console.log(`Process Asset Directory: ${sourceDir}`);

    const items = fs.readdirSync(sourceDir, {withFileTypes: true});

    for (const item of items) {
        if (ignore.has(item.name))
            continue;

        const currentPath = path.join(sourceDir, item.name);

        if (item.isDirectory()) {
            const destPath = path.join(destDir, item.name);
            fs.mkdirSync(destPath);
            processAssetDirectory(currentPath, destPath, options)
        }
        else if (item.isFile()){
            const asset = item.name;

            if (asset.endsWith(".tileset.json"))
                options.tilesetFunc(currentPath, destDir, asset);
            else if (asset.endsWith(".level.json"))
                options.levelFunc(currentPath, destDir, asset);
            else if (asset.endsWith(".animation.json"))
                options.animationFunc(currentPath, destDir, asset);
            else if (asset.endsWith(".png"))
                options.spriteFunc(currentPath, destDir, asset);
            else if (asset.endsWith(".font.json"))
                options.fontFunc(currentPath, destDir, asset);
            else if (musicFormats.has(path.extname(asset)))
                options.musicFunc(currentPath, destDir, asset);
        }
    }
}

function prepareAssets(sourceDir, destDir, params) {
    let options = {
        littleEndian: true,
        sourceDir: sourceDir,
        destDir: destDir,
        spriteFunc: (sourceFile, destDir, assetName) => {
            const buildPath = path.join(destDir, assetName);
            prepareSprite(sourceFile, buildPath, options.littleEndian);
        },
        levelFunc: (sourceFile, destDir, assetName) =>  {
            const baseName = path.basename(assetName, ".level.json");
            const buildPath = path.join(destDir, `${baseName}.level`);
            prepareLevel(sourceFile, buildPath, options);
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
        },
        fontFunc: (sourceFile, destDir, assetName) => {
            const baseName = path.basename(assetName, ".font.json");
            const buildPath = path.join(destDir, `${baseName}.font`);
            prepareFont(sourceFile, buildPath, options.littleEndian);
        }
    };

    if (params)
        Object.assign(options, params);

    processAssetDirectory(sourceDir, destDir, options);
    prepareLevelList(destDir, options.littleEndian);
}

module.exports = {
    prepareAssets: prepareAssets,
    prepareLevel: prepareLevel
};