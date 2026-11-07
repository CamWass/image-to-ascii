
import { readFileSync, writeFileSync } from 'node:fs'

const text = readFileSync("./out/out.txt", "utf8");

const template = readFileSync("./template.html", "utf8");

let content = "";

const tail = text.split("").reduce((acc, cur) => {
    if (cur != "\n") {

        return acc + `<span>${cur}</span>`;

    } else {
        if (acc.length !== 0) {
            content += `<div>${acc}</div>`;
        }
        content += cur;
        return "";
    }
}, "");

if (tail.length !== 0) {
    content += `<div>${tail}</div>`;
}

const output = template.replace("CONTENT", content);

writeFileSync("./out/out.html", output);