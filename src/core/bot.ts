import { helpCommand, allCommand, trollCommand } from '../commands';
import { vk } from '../config';

export class Bot {
    constructor() {}

    public async startPolling(): Promise<void> {
        await vk.updates.startPolling().catch(e => console.error(e));
    }

    public async runCommands(): Promise<void> {
        await Promise.all([
            helpCommand(),
            allCommand(),
            trollCommand(),
        ]);
    }
};
