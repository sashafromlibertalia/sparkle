import { vk } from '../config';

export class Bot {
    constructor() {}

    public async startPolling(): Promise<void> {
        await vk.updates.startPolling().catch(e => console.error(e));
    }
}
