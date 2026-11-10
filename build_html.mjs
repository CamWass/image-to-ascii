
import { readFileSync, writeFileSync } from 'node:fs'

const text = readFileSync("./out/out.txt", "utf8");

const template = readFileSync("./template.html", "utf8");

const output = template.replace("CONTENT", text);

writeFileSync("./out/out.html", output);