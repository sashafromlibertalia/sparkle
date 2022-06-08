import { IsEnum, IsNotEmpty, IsString } from 'class-validator';
import { BaseEntity, Column, Entity, PrimaryGeneratedColumn } from 'typeorm';

enum Weekdays {
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY,
}

@Entity('subject')
export class SubjectEntity extends BaseEntity {
    @PrimaryGeneratedColumn()
        id: number;

    @IsString()
    @IsNotEmpty()
    @Column()
        name: string;

    @IsString()
    @IsNotEmpty()
    @Column()
        description: string;

    @IsEnum(Weekdays)
    @IsNotEmpty()
    @Column()
        weekDay: Weekdays;
}
