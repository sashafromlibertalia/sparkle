import { IConfig } from '../types/global';
import dotenv from 'dotenv';
dotenv.config({ path: `.env.${process.env.NODE_ENV}` });

export const config: IConfig = {
    token: process.env.TOKEN ?? '',
    pollingGroupId: 0,
    parserUrl: '',
    groupName: '',
    universityName: '',
    admin: {
        name: '',
        shortLink: '',
        list: [],
    },
};
