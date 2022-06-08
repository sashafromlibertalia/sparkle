import { SubjectEntity } from '../entities';
import { Connection, createConnection, FindConditions, Repository } from 'typeorm';

export class SubjectService {
    private connection: Connection;
    private subjectRepository: Repository<SubjectEntity>;

    constructor() {
        this.createDbConnection().then(() => {
            this.subjectRepository = this.connection.getRepository(SubjectEntity);
        });
    };

    private async createDbConnection() {
        this.connection = await createConnection();
    }

    public async selectMany(
        conditions?: FindConditions<SubjectEntity>,
    ): Promise<SubjectEntity[]> {
        return this.subjectRepository.find(conditions).catch(() => {
            throw new Error('Subject not found');
        });
    }
}
