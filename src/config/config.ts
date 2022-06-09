import dotenv from 'dotenv';
import fs from 'fs';
dotenv.config({ path: `.env.${process.env.NODE_ENV}` });

const envConfig = dotenv.parse(fs.readFileSync(`.env.${process.env.NODE_ENV}`));

export const config: IConfig = {
    token: envConfig.TOKEN ?? '',
    pollingGroupId: +envConfig.GROUP_ID ?? 0,
};
