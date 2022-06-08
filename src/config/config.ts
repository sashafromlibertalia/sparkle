import dotenv from 'dotenv';
import fs from 'fs';
dotenv.config({ path: `.env.${process.env.NODE_ENV}` });

const envConfig = dotenv.parse(fs.readFileSync('.env.development'));

export const config: IConfig = {
    token: envConfig.TOKEN ?? '',
    pollingGroupId: +envConfig.GROUP_ID ?? 0,
    parserUrl: '',
    groupName: '',
    universityName: '',
    admin: {
        name: '',
        shortLink: '',
        list: [],
    },
};
