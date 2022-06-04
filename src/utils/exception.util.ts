export const throwException = (error: any) => {
    return `Упс... Что-то пошло не так. Ошибка: ${error?.toString()}`;
};
