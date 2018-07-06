"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const SourceCode_1 = require("./SourceCode");
const StringType_1 = require("./types/StringType");
const NumberType_1 = require("./types/NumberType");
const AnyType_1 = require("./types/AnyType");
const BooleanType_1 = require("./types/BooleanType");
const CustomType_1 = require("./types/CustomType");
const VectorType_1 = require("./types/VectorType");
const Utils_1 = require("./Utils");
const IntBoolType_1 = require("./types/IntBoolType");
const AliasClassScheme_1 = require("./schema/AliasClassScheme");
const CustomPrimitiveType_1 = require("./types/CustomPrimitiveType");
class TypescriptCodeGenerator {
    generateClass(scheme) {
        if (scheme instanceof AliasClassScheme_1.default) {
            return this.generateClassAlias(scheme);
        }
        let code = new SourceCode_1.default();
        let constructor = this.generateClassConstructor(scheme);
        let deserializeMethod = this.generateDeserializeMethod(scheme);
        code.add(`export class ${scheme.name} {`);
        code.append(constructor, 1);
        code.add('');
        code.append(deserializeMethod, 1);
        code.add('}');
        return code;
    }
    generateInterface(scheme) {
        if (scheme instanceof AliasClassScheme_1.default) {
            return this.generateClassAlias(scheme);
        }
        let code = new SourceCode_1.default();
        code.add(`export interface ${Utils_1.toCamelCase(scheme.name, true, '.')} {`);
        scheme.fields.forEach((prop, index) => {
            let coma = this.genComa(scheme.fields, index);
            code.add(`/**`, 1);
            code.add(` * ${prop.description}`, 1);
            code.add(` */`, 1);
            code.add(`${(prop.name)}: ${this.renderType(prop.type, true)}${coma}`, 1);
        });
        code.add('}');
        return code;
    }
    generateCustomPrimitive(scheme, withoutDeserializeFunc) {
        let code = new SourceCode_1.default();
        let typeDeclaration = this.renderType(scheme.type, true);
        code.add(`export type ${scheme.name} = ${typeDeclaration}`);
        if (withoutDeserializeFunc === true)
            return code;
        code.add('');
        code.add(`export function ${scheme.name}_deserialize(raw: any): ${typeDeclaration} {`);
        code.add('return (', 1);
        //let coma = this.genComa(scheme.fields, index)
        let fieldVar = `raw`;
        if (scheme.type instanceof VectorType_1.default)
            code.add(this.renderVectorDeserialize(fieldVar, scheme.type), 2);
        else if (scheme.type instanceof CustomType_1.default)
            code.add(`${fieldVar} ? ${scheme.type.name}.deserialize(${fieldVar}) : undefined`, 2);
        else if (scheme.type instanceof IntBoolType_1.default)
            code.add(`!!${fieldVar}`, 2);
        else
            code.add(fieldVar, 2);
        code.add(')', 1);
        code.add('}');
        return code;
    }
    generateApiMethod(scheme) {
        let code = new SourceCode_1.default();
        let methodName = Utils_1.toCamelCase(scheme.name, false, '.');
        let propsName = `MethodsProps.${Utils_1.toCamelCase(scheme.name, true, '.')}Params`;
        let responseName = this.renderType(scheme.responseType, true);
        code.add(`/**`);
        code.add(` * ${scheme.description}`);
        code.add(' *');
        code.add(' * @param {{');
        scheme.params.forEach((param, index) => {
            let coma = this.genComa(scheme.params, index);
            code.add(` *   ${(param.name)}: (${this.renderType(param.type, true)}${param.required ? '' : '|undefined'})${coma}`);
        });
        code.add(' * }} params');
        code.add(' *');
        code.add(` * @returns {Promise<${responseName}>}`);
        code.add(` */`);
        code.add(`public async ${methodName}(params: ${propsName}): Promise<${responseName}> {`);
        code.add(`return this.call("${scheme.name}", params)`, 1);
        // code.add(`'${scheme.name}',`, 2)
        // code.add(`{`, 2)
        // scheme.params.forEach((param, index) => {
        //     let coma = this.genComa(scheme.params, index)
        //
        //     code.add(`${param.name}: params.${toCamelCase(param.name)}${coma}`, 3)
        // })
        // code.add(`},`, 2)
        // code.add(responseName, 2)
        //code.add(')', 1)
        code.add('}');
        return code;
    }
    generateApiMethodParamsInterface(scheme) {
        let code = new SourceCode_1.default();
        code.add(`export interface ${Utils_1.toCamelCase(scheme.name, true, '.')}Params {`);
        scheme.params.forEach((prop, index) => {
            let coma = this.genComa(scheme.params, index);
            code.add(`/**`, 1);
            code.add(` * ${prop.description}`, 1);
            code.add(` */`, 1);
            code.add(`${(prop.name)}${prop.required ? '' : '?'}: ${this.renderType(prop.type, true)}${coma}`, 1);
        });
        code.add('}');
        return code;
    }
    generateClassConstructor(scheme) {
        let code = new SourceCode_1.default();
        let jsdoc = this.generateClassConstructorJSDoc(scheme);
        code.append(jsdoc);
        code.add('constructor (');
        scheme.fields.forEach((field, index) => {
            let coma = this.genComa(scheme.fields, index);
            code.add(`readonly ${Utils_1.toCamelCase(field.name)}: ${this.renderType(field.type)}${coma}`, 1);
        });
        code.add(') {');
        code.add('');
        code.add('}');
        return code;
    }
    generateClassConstructorJSDoc(scheme) {
        let code = new SourceCode_1.default();
        code.add('/**');
        code.add(' * @class');
        scheme.fields.forEach(field => {
            code.add(` * @property {${this.renderType(field.type)}} ${Utils_1.toCamelCase(field.name)} ${field.description}`);
        });
        code.add(' */');
        return code;
    }
    generateDeserializeMethod(scheme) {
        let code = new SourceCode_1.default();
        code.add('/**');
        code.add(' * @param {Object} raw');
        code.add(` * @returns {${scheme.name}}`);
        code.add(' */');
        code.add(`static deserialize(raw: Object): ${scheme.name} {`);
        code.add(`return new ${scheme.name} (`, 1);
        scheme.fields.forEach((field, index) => {
            let coma = this.genComa(scheme.fields, index);
            let fieldVar = `raw['${field.name}']`;
            if (field.type instanceof VectorType_1.default)
                code.add(this.renderVectorDeserialize(fieldVar, field.type) + coma, 2);
            else if (field.type instanceof CustomType_1.default)
                code.add(`${fieldVar} ? ${field.type.name}.deserialize(${fieldVar}) : undefined${coma}`, 2);
            else if (field.type instanceof CustomPrimitiveType_1.default)
                code.add(`${fieldVar} ? ${field.type.name}_deserialize(${fieldVar}) : undefined${coma}`, 2);
            else if (field.type instanceof IntBoolType_1.default)
                code.add(`!!${fieldVar}${coma}`, 2);
            else
                code.add(fieldVar + coma, 2);
        });
        code.add(`)`, 1);
        code.add('}');
        return code;
    }
    renderType(type, withoutUndefined = false) {
        if (type instanceof StringType_1.default)
            return 'string';
        if (type instanceof NumberType_1.default)
            return 'number';
        if (type instanceof AnyType_1.default)
            return 'any';
        if (type instanceof BooleanType_1.default)
            return 'boolean';
        if (type instanceof IntBoolType_1.default)
            return 'boolean';
        if (type instanceof CustomType_1.default)
            return type.name + `${!withoutUndefined ? '|undefined' : ''}`;
        if (type instanceof CustomPrimitiveType_1.default)
            return type.name + `${!withoutUndefined ? '|undefined' : ''}`;
        if (type instanceof VectorType_1.default) {
            return this.renderType(type.item, true) + `[]${!withoutUndefined ? '|undefined' : ''}`;
        }
        throw { 'UNSUPPORTED TYPE': type };
    }
    genComa(list, index) {
        return (index == list.length - 1) ? '' : ',';
    }
    renderVectorDeserialize(value, type) {
        let code = '';
        if (type instanceof VectorType_1.default)
            code += `${value} ? ${value}.map(v => ${this.renderVectorDeserialize('v', type.item)}) : undefined`;
        else if (type instanceof CustomType_1.default)
            code += `${value} ? ${type.name}.deserialize(${value}) : undefined`;
        else if (type instanceof CustomPrimitiveType_1.default)
            code += `${value} ? ${type.name}_deserialize(${value}) : undefined`;
        else
            code += value;
        return code;
    }
    generateClassAlias(scheme) {
        let code = new SourceCode_1.default();
        let typeDeclaration = this.renderType(new CustomType_1.default(scheme.aliasClass.name), true);
        code.add(`export type ${scheme.name} = ${typeDeclaration}`);
        return code;
    }
}
exports.default = TypescriptCodeGenerator;
//# sourceMappingURL=TypescriptCodeGenerator.js.map