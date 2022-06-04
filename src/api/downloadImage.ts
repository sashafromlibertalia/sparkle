import fs from 'fs';
import * as https from 'https';

export const downloadImage = async (url: string) => {
    https.get(url, (res) => {
        fs.mkdir('temp', { recursive: true }, (err) => {
            if (err) throw err;
        });
        const path = 'temp/image.png';
        const writeStream = fs.createWriteStream(path);

        res.pipe(writeStream);

        writeStream.on('finish', () => writeStream.close());
    });
};
