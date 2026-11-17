/* Targeted read-only extraction of original UI input assets. */
import fs from "node:fs";
import path from "node:path";

const root = path.resolve(process.argv[2] || path.resolve(import.meta.dirname, "..", ".."));
const listUiEntries = process.argv.includes("--list-ui-ini") || process.argv.some((arg) => arg.startsWith("--contains="));
const requestedIdArg = process.argv.find((arg) => arg.startsWith("--id="));
const requestedPathArg = process.argv.find((arg) => arg.startsWith("--path="));
const requestedOutArg = process.argv.find((arg) => arg.startsWith("--out="));
const containsArg = process.argv.find((arg) => arg.startsWith("--contains="));
const requestedPakArg = process.argv.find((arg) => arg.startsWith("--pak="));
const pakPath = path.join(root, "Client_Original", "data",
  requestedPakArg ? requestedPakArg.slice(6) : listUiEntries ? "ui.pak" : "vng00.pak");
const requestedPath = requestedPathArg ? requestedPathArg.slice(7) : "\\Ui\\autoexec.lua";

function filenameToId(value) {
  let edx = 0, position = 0;
  for (const byte of Buffer.from(value, "latin1")) {
    const signed = byte >= 0x80 ? byte - 0x100 : byte;
    const folded = byte >= 0x41 && byte <= 0x5a ? byte + 0x20 : signed;
    position++;
    const eax = (folded * position + edx) >>> 0;
    edx = Number((BigInt(eax) % 0x8000000bn) * 0xffffffefn & 0xffffffffn);
  }
  return (edx ^ 0x12345678) >>> 0;
}

function decodeUcl(source, expectedSize) {
  let input = 0, bitBuffer = 0, lastOffset = 1;
  const output = Buffer.allocUnsafe(expectedSize);
  let written = 0;
  const bit = () => {
    if (bitBuffer & 0x7f) bitBuffer = (bitBuffer * 2) & 0x1ff;
    else {
      if (input >= source.length) throw new Error("UCL input overrun");
      bitBuffer = source[input++] * 2 + 1;
    }
    return (bitBuffer >> 8) & 1;
  };
  while (true) {
    while (bit()) {
      if (input >= source.length || written >= expectedSize) throw new Error("UCL literal overrun");
      output[written++] = source[input++];
    }
    let offset = 1;
    while (true) {
      offset = offset * 2 + bit();
      if (offset > 0x1000002) throw new Error("UCL invalid offset");
      if (bit()) break;
    }
    if (offset === 2) offset = lastOffset;
    else {
      if (input >= source.length) throw new Error("UCL offset overrun");
      offset = (offset - 3) * 256 + source[input++];
      if (offset === 0xffffffff) break;
      offset++;
      lastOffset = offset;
    }
    let length = bit();
    length = length * 2 + bit();
    if (length === 0) {
      length = 1;
      while (true) {
        length = length * 2 + bit();
        if (length >= expectedSize) throw new Error("UCL invalid length");
        if (bit()) break;
      }
      length += 2;
    }
    if (offset > 0xd00) length++;
    if (offset > written || written + length + 1 > expectedSize) throw new Error("UCL copy overrun");
    let copyPosition = written - offset;
    for (let i = 0; i <= length; i++) output[written++] = output[copyPosition++];
  }
  if (written !== expectedSize) throw new Error(`UCL size mismatch ${written}/${expectedSize}`);
  return output;
}

const fd = fs.openSync(pakPath, "r");
try {
  const header = Buffer.alloc(32);
  fs.readSync(fd, header, 0, header.length, 0);
  if (header.toString("ascii", 0, 4) !== "PACK") throw new Error("not a Kingsoft PAK");
  const count = header.readUInt32LE(4), indexOffset = header.readUInt32LE(8);
  const index = Buffer.alloc(count * 16);
  fs.readSync(fd, index, 0, index.length, indexOffset);
  const readEntry = (entryOffset) => {
    const dataOffset = index.readUInt32LE(entryOffset + 4);
    const originalSize = index.readUInt32LE(entryOffset + 8);
    const packed = index.readUInt32LE(entryOffset + 12);
    const storedSize = packed & 0x00ffffff;
    const method = packed & 0xff000000;
    const source = Buffer.alloc(storedSize);
    fs.readSync(fd, source, 0, source.length, dataOffset);
    return method === 0 ? source : method === 0x01000000 ? decodeUcl(source, originalSize) : null;
  };
  if (listUiEntries) {
    const rows = [];
    for (let n = 0; n < count; n++) {
      const entryOffset = n * 16;
      const data = readEntry(entryOffset);
      if (!data) continue;
      const text = data.toString("latin1");
      if (text.includes("[") && text.includes("]") && text.includes("=") &&
          (!containsArg || text.toLowerCase().includes(containsArg.slice(11).toLowerCase()))) {
        rows.push(`0x${index.readUInt32LE(entryOffset).toString(16).padStart(8, "0")}\t${data.length}\t${text.slice(0, 180).replace(/[\r\n\t]+/g, " ")}`);
      }
    }
    const listPath = path.join(root, "analysis", "repair", "ui_original_probe", "ui.pak.ini-index.txt");
    fs.mkdirSync(path.dirname(listPath), { recursive: true });
    fs.writeFileSync(listPath, rows.join("\n") + "\n", "utf8");
    console.log(`PAK=${path.basename(pakPath)} TEXT_INI_ENTRIES=${rows.length} OUTPUT=${listPath}`);
    process.exit(0);
  }
  const id = requestedIdArg ? Number.parseInt(requestedIdArg.slice(5), 16) >>> 0 : filenameToId(requestedPath);
  if (!Number.isFinite(id)) throw new Error("--id must be a hexadecimal PAK entry id");
  let entryOffset = -1;
  for (let n = 0; n < count; n++) if (index.readUInt32LE(n * 16) === id) { entryOffset = n * 16; break; }
  if (entryOffset < 0) throw new Error(`missing exact original path ${requestedPath}`);
  const data = readEntry(entryOffset);
  if (!data) throw new Error("unsupported compression method");
  const outputPath = requestedOutArg
    ? path.resolve(root, requestedOutArg.slice(6))
    : path.join(root, "analysis", "repair", "ui_original_probe",
      requestedIdArg ? `ui-entry-${id.toString(16).padStart(8, "0")}.bin` : "autoexec.original.lua");
  fs.mkdirSync(path.dirname(outputPath), { recursive: true });
  fs.writeFileSync(outputPath, data);
  console.log(`ORIGINAL_PATH=${requestedIdArg ? "PAK_ID" : requestedPath}`);
  console.log(`PAK=${path.basename(pakPath)} ENTRY=${entryOffset / 16} BYTES=${data.length}`);
  console.log(`OUTPUT=${outputPath}`);
} finally { fs.closeSync(fd); }
