import { citgenConfig } from '../config';
import { createCanvas, loadImage } from 'canvas';
import { downloadImage } from '../api';
import fs from 'fs';
import { Context } from 'vk-io';
import { IUser } from '../types/global';

export const generateCitgenImage = async (context: Context, url: string, text: string, user: IUser) => {
    const { width, height, background, fill, title } = citgenConfig;

    const citgenImageName = `temp/citgen-${new Date().getTime()}.png`;
    const userPicImageName = `temp/pic-${user.last_name}-${user.last_name}.png`;

    const canvas = createCanvas(width, height);
    const ctx = canvas.getContext('2d');

    await downloadImage(url, userPicImageName, () => {
        ctx.fillStyle = background;
        ctx.fillRect(0, 0, width, height);
        ctx.font = 'bold 28px';
        ctx.fillStyle = fill;

        ctx.fillText(title, 30, 42);
        ctx.fillText(`© ${user.first_name} ${user.last_name}`, 30, 470);

        ctx.font = 'bold 20px';
        ctx.fillText(`«${text}»`, 260, 110);

        loadImage(url).then(image => {
            ctx.drawImage(image, 30, 85);
            const buffer = canvas.toBuffer('image/png');
            fs.writeFileSync(citgenImageName, buffer);

            context.sendPhotos({
                value: citgenImageName,
            });

            setTimeout(() => {
                fs.unlink(citgenImageName, () => {});
                fs.unlink(userPicImageName, () => {});
            });
        });
    });
};
