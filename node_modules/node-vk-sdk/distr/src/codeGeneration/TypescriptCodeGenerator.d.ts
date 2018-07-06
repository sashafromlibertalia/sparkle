import { CodeGenerator } from "./CodeGenerator";
import ClassScheme from "./schema/ClassScheme";
import SourceCode from "./SourceCode";
import ApiMethodScheme from "./schema/ApiMethodScheme";
import CustomPrimitiveScheme from "./schema/CustomPrimitiveScheme";
export default class TypescriptCodeGenerator implements CodeGenerator {
    generateClass(scheme: ClassScheme): SourceCode;
    generateInterface(scheme: ClassScheme): SourceCode;
    generateCustomPrimitive(scheme: CustomPrimitiveScheme, withoutDeserializeFunc: boolean): SourceCode;
    generateApiMethod(scheme: ApiMethodScheme): SourceCode;
    generateApiMethodParamsInterface(scheme: ApiMethodScheme): SourceCode;
    private generateClassConstructor(scheme);
    private generateClassConstructorJSDoc(scheme);
    private generateDeserializeMethod(scheme);
    private renderType(type, withoutUndefined?);
    private genComa(list, index);
    private renderVectorDeserialize(value, type);
    private generateClassAlias(scheme);
}
