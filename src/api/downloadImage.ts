import fs from 'fs';
import * as https from 'https';

export const downloadImage = async (url: string, path: string, callback: () => void) => {
    https.get(url, (res) => {
        fs.mkdir('temp', { recursive: true }, (err) => {
            if (err) throw err;
        });

        const writeStream = fs.createWriteStream(path);
        res.pipe(writeStream);

        writeStream.on('finish', callback);
    });
};
