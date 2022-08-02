#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import ast
import os
import sys
from os.path import basename
from inspect import getfile, getsource
from hashlib import md5
from typing import NewType

type_translator = {}
headers = set([])
compiledc = []
compiledh = []
func_enum = []
source_code = ""

u64 = NewType('u64', int)
usize = NewType('usize', int)
u32 = NewType('u32', int)

def auto():
    pass

def c_notation(code):
    pass

def ptr(type):
    pass

def to_json_type(type_name):
    if type_name in type_translator.values():
        return "number"
    
    if type_name[-1] == "*":
        return "ptr"

    match type_name:
        case "char *":
            return "string"
        case "long" | "int" | "uint64_t" | "uintptr_t":
            return "number"
        case "bool":
            return "bool"
        case "void":
            return "null"
        case _:
            raise Exception(f"Unknown type: {type_name}")

def to_ctype(type_name):
    if type_name in type_translator:
        return type_translator[type_name]

    match type_name:
        case "str":
            return "char *"
        case "int":
            return "long"
        case "u32":
            headers.add("stdint.h")
            return "uint32_t"
        case "u64":
            headers.add("stdint.h")
            return "uint64_t"
        case "usize":
            headers.add("stddef.h")
            return "size_t"
        case "bool":
            headers.add("stdbool.h")
            return "bool"
        case _:
            raise Exception(f"Unknown type: {type_name}")


class HGen(ast.NodeTransformer):
    def __init__(self, module_name):
        self.module_name = module_name

        if not os.path.isdir(os.path.join(sys.argv[1], "inc")):
            os.mkdir(os.path.join(sys.argv[1], "inc"))

    def visit_FunctionDef(self, node):
        global source_code
        identifier = "0x"+str(md5(self.module_name.encode()).hexdigest()[:8])
        filename = os.path.join(sys.argv[1], "inc", f"{self.module_name}.h")
        func_args = []

        for arg in node.args.args:
            if type(arg.annotation) == ast.Name:
                func_args.append((arg.arg, to_ctype(arg.annotation.id)))
            elif type(arg.annotation) == ast.Call:
                if arg.annotation.func.id == "ptr":
                    func_args.append((arg.arg, to_ctype(arg.annotation.args[0].id) + " *"))
            else:
                raise Exception(f"Can't use {type(arg.annotation)} node type")

        if len(func_args) == 0:
            args = "void"
        else:
            args = ", ".join(
                [f"{arg_type} {arg_name}" for arg_name, arg_type in func_args]
            )

        if type(node.returns) == ast.Constant:
            if node.returns.value is None:
                return_type = "void"
            else:
                print(f"Unknown return type: {node.returns.value}")
                exit(1)
        elif type(node.returns) == ast.Call:
            if node.returns.func.id == "ptr":
                return_type = f"{to_ctype(node.returns.args[0].id)} *"
            else:
                raise Exception("Unknown return type")
        else:
            return_type = to_ctype(node.returns.id)

        if self.module_name not in compiledh:
            with open(filename, "w") as f:
                f.write(f"#pragma once\n#define ECHO_IDENT {identifier}\n\n")
                f.write("#include <ipc.h>\n#include <json.h>\n\n")

        with open(filename, "a") as f:
            if source_code:
                f.write(source_code)
                source_code = ""
            with open(os.path.join(os.path.dirname(__file__), "endpoint.h"), "r") as endpoint:
                template = endpoint.read()

            structs = "typedef struct \n{\n"
            
            for arg_name, arg_type in func_args:
                structs += f"    {arg_type} {arg_name};\n"
        
            structs += f"}} {self.module_name}_req_{node.name.lower()}_t;"

            f.write(template.format(
                rpc_structs = structs,
                C_response_type = return_type,
                rpc_module = self.module_name,
                rpc_module_upper = self.module_name.upper(),
                rpc_function = node.name,
                rpc_function_upper = node.name.upper(),
                rpc_id = len(compiledh),
                rpc_args = args
            ))

            compiledh.append(self.module_name)
            

class CGen(ast.NodeTransformer):
    def __init__(self, module_name):
        self.module_name = module_name.lower()

        if not os.path.isdir(os.path.join(sys.argv[1], "src")):
            os.mkdir(os.path.join(sys.argv[1], "src"))

    def visit_FunctionDef(self, node):
        func_name = node.name
        func_args = []

        for arg in node.args.args:
            if type(arg.annotation) == ast.Name:
                func_args.append((arg.arg, to_ctype(arg.annotation.id)))
            elif type(arg.annotation) == ast.Call:
                if arg.annotation.func.id == "ptr":
                    func_args.append((arg.arg, to_ctype(arg.annotation.args[0].id) + " *"))

        if len(func_args) == 0:
            args = "void"
        else:
            args = ", ".join(
                [f"{arg_type} {arg_name}" for arg_name, arg_type in func_args]
            )

        if type(node.returns) == ast.Constant:
            if node.returns.value is None:
                return_type = "void"
            else:
                print(f"Unknown return type: {node.returns.value}")
                exit(1)
        elif type(node.returns) == ast.Call:
            if node.returns.func.id == "ptr":
                return_type = f"{to_ctype(node.returns.args[0].id)} *"
            else:
                raise Exception("Unknown return type")
        else:
            return_type = to_ctype(node.returns.id)

        if self.module_name not in compiledc:
            compiledc.append(self.module_name)
            with open(os.path.join(sys.argv[1], "src", f"{self.module_name}.c"), "w") as f:
                # --- HEADERS 
                f.write("#include <assert.h>\n")
                f.write("#include <ipc.h>\n")
                f.write("#include <json.h>\n")
                f.write("#include <unistd.h>\n\n")
                f.write(f'#include "../inc/{self.module_name}.h"\n\n')
                f.write(f"")

        with open(os.path.join(sys.argv[1], "src", f"{self.module_name}.c"), "a") as f:
            # --- CLIENT ENDPOINT
            with open(os.path.join(os.path.dirname(__file__), "endpoint.c"), "r") as endpoint:
                template = endpoint.read()

            f.write(template.format(
                C_response_type = return_type,
                JSON_response_type = to_json_type(return_type),
                rpc_response_type = to_json_type(return_type).upper(),
                rpc_function = func_name,
                rpc_function_UPPER = f"{self.module_name.upper()}_{func_name.upper()}",
                rpc_args = args,
                rpc_module = self.module_name,
                rpc_args_unpack = "\n        ".join([f".{arg_name} = json_get(rpc_args, \"{arg_name}\")._{to_json_type(arg_type)}," for arg_name, arg_type in func_args]),
                rpc_identifier = f"{self.module_name.upper()}_IDENT",
                rpc_args_push = "\n    ".join(f"json_push(&rpc_args, \"{arg_name}\", json_{to_json_type(arg_type)}({arg_name}));" for arg_name, arg_type in func_args),
                rpc_return = "" if return_type == "void" else  f"{return_type} ret = rpc_resp._{to_json_type(return_type)};",
                rpc_ret = "" if return_type == "void" else  f"\n    return ret;"
            ))


def endpoint(cls):
    module_name = basename(getfile(cls)).split(".")[0]
    endpoint_func = filter(lambda x: not x.startswith("__"), dir(cls))

    for func in endpoint_func:
        func_source = getsource(getattr(cls, func)).strip()
        header = HGen(module_name)
        source = CGen(module_name)
        header.visit(ast.parse(func_source))
        source.visit(ast.parse(func_source))


def enum(cls):
    global source_code
    global type_translator

    enum_fields = list(filter(lambda x: not x.startswith("__"), dir(cls)))
    enum_as_dict = {k: getattr(cls, k) for k in enum_fields}

    type_translator[cls.__name__] = f"enum {cls.__name__.upper()}"

    source_code += f"""enum {cls.__name__.upper()}
{{
    {f'{chr(10)}    '.join(f"{k} = {v}," if v else f"{k}," for k, v in enum_as_dict.items())}
}};\n\n"""



class StructGen(ast.NodeTransformer):
    def __init__(self):
        self.struct = ""
        self.name = ""

    def visit_ClassDef(self, node):
        self.generic_visit(node)
        self.name = node.name

    def visit_AnnAssign(self, node):
        self.generic_visit(node)

        if type(node.annotation) == ast.Name:
            self.struct += f"    {to_ctype(node.annotation.id)} {node.target.id};\n"
        elif type(node.annotation) == ast.Call:
            if node.annotation.func.id == "ptr":
                self.struct += f"    {to_ctype(node.annotation.args[0].id)} * {node.target.id};\n"
            elif node.annotation.func.id == "c_notation":
                self.struct += f"    {node.annotation.args[0].value};\n"

    def output(self):
        return (f"typedef struct {self.name.upper()} {{\n{self.struct}}} {self.name}_t;\n\n", self.name)

def struct(cls):
    global source_code
    global type_translator

    gen = StructGen()
    gen.visit(ast.parse(getsource(cls).strip()))
    code, name  = gen.output()
    source_code += code
    type_translator[name] = f"{name}_t"